#include "DataCollectionDS80.h"
#include <opencv2/opencv.hpp>

DataCollectionDS80::DataCollectionDS80()
{

}
DataCollectionDS80::~DataCollectionDS80()
{

}

bool DataCollectionDS80::initialize(const DataCollectionConfig& config)
{
	m_dataCollectionConfig = config;

	//设置日志等级
	xv::setLogLevel((xv::LogLevel)config.log_level);

	auto devices = xv::getDevices(5.0);

	if (devices.empty()) {
		printf("device not exist\n");
		return false;
	}

	m_device = devices.begin()->second;

	auto slam = m_device->slam();
	if (!slam) {
		printf("not support SLAM\n");
		m_device.reset();
		return false;
	}

	// 注册位姿回调
	//	m_slam_callbackId = slam->registerCallback([](xv::Pose const& pose) {
	//	auto t = pose.translation();
	//	auto r = xv::rotationToPitchYawRoll(pose.rotation());
	//	std::cout << "位置: (" << t[0] << ", " << t[1] << ", " << t[2] << ")" << std::endl;
	//	std::cout << "旋转: (" << r[0] << ", " << r[1] << ", " << r[2] << ")" << std::endl;
	//	std::cout << "置信度: " << pose.confidence() << std::endl;
	//});

	m_slam = slam;

	auto fisheye = m_device->fisheyeCameras();
	if (!fisheye) {
		printf("not support fisheye\n");
		m_device.reset();
		m_slam.reset();
		return false;
	}

	fisheye->registerCallback([this](xv::FisheyeImages const& images) {
		auto& leftInput = images.images[0];
		auto& rightInput = images.images[1];
		cv::Mat left = cv::Mat::zeros(leftInput.height, leftInput.width, CV_8UC1);
		cv::Mat right = cv::Mat::zeros(rightInput.height, rightInput.width, CV_8UC1);

		if (leftInput.data != nullptr) {
			std::memcpy(left.data, leftInput.data.get(), static_cast<size_t>(left.rows * left.cols));
		}
		if (rightInput.data != nullptr) {
			std::memcpy(right.data, rightInput.data.get(), static_cast<size_t>(right.rows * right.cols));
		}

		cv::cvtColor(left, left, cv::COLOR_GRAY2BGR);
		cv::cvtColor(right, right, cv::COLOR_GRAY2BGR);

		if (m_dataCollectionConfig.cb_img_data)
			m_dataCollectionConfig.cb_img_data(left, right);
		});

	m_fisheye = fisheye;

	std::map<std::string, std::string> info = m_device->info();
	for (std::map<std::string, std::string>::iterator ite = info.begin(); ite != info.end(); ite++) {
		printf("%s:%s\n", ite->first.c_str(), ite->second.c_str());
	}

	return true;
}

void DataCollectionDS80::cleanup()
{
	stop();

	if(m_fisheye)
		m_fisheye.reset();
	if (m_slam)
		m_slam.reset();
	if(m_device)
		m_device.reset();
}

void DataCollectionDS80::poseThread()
{
	int cnt = 0;
	while (m_run_posethread){
		auto now = std::chrono::steady_clock::now();
		xv::Pose pose;
		if (m_slam->getPose(pose, m_dataCollectionConfig.getpose_prediction / 1000.0f)) {  // 预测
			m_lock_pose_data.lock();
			if (m_pose_data.size() > m_dataCollectionConfig.buffer_size)
				m_pose_data.pop();
			m_pose_data.push(pose);
			m_lock_pose_data.unlock();

			if (pose.confidence() > 0.7){
				cnt++;
				if(cnt > 10)
					m_isMapStable = true;
			}
			else{
				cnt = 0;
				m_isMapStable = false;
			}
		}
		double t = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		std::this_thread::sleep_until(now + std::chrono::microseconds(long(1.0 / 60.0 * 1e6)));
	}
}

bool DataCollectionDS80::start()
{
	if(!m_device || !m_fisheye || !m_slam)
		return false;



	if (m_slam) {
		m_slam->start();
	}

	if (m_fisheye){
		m_fisheye->start();
	}
	return true;
}

void DataCollectionDS80::stop()
{
	if (m_fisheye){
		m_fisheye->stop();
	}
	if (m_slam){
		m_slam->stop();
		//m_slam->unregisterCallback(m_slam_callbackId);
		//m_slam_callbackId = 0;
	}

	if (m_run_posethread){
		m_run_posethread = false;
		m_pose_thread.join();
	}
	if (m_device->imuSensor()) {
		m_device->imuSensor()->unregisterCallback(m_imu_callbackId);
		m_device->imuSensor()->stop();
		m_imu_callbackId = 0;
	}

}

bool DataCollectionDS80::find_nearest_imu(int64_t pose_timestamp_ns, xv::Imu& nearest_imu, double& pair_delta_us, double& pair_quality)
{
	pair_delta_us = 9999.99f;
	pair_quality = 0.00f;

	bool is_find = false;
	double min_delta = INFINITY;

	std::list<xv::Imu> tmpList;
	m_lock_imu_data.lock();
	tmpList = m_imu_data;
	m_lock_imu_data.unlock();


	for (std::list<xv::Imu>::iterator ite = tmpList.begin(); ite != tmpList.end(); ite++) {
		double delta = abs(ite->edgeTimestampUs - pose_timestamp_ns);
		if (delta < min_delta) {
			min_delta = delta;
			nearest_imu = *ite;
		}
	}

	if (min_delta <= m_dataCollectionConfig.pair_tolerance_us) {
		pair_delta_us = min_delta;
		pair_quality = 1-(min_delta / m_dataCollectionConfig.pair_tolerance_us);
		return true;
	}

	printf("find_nearest_imu failed!\n");
	return false;  // 配对失败
}

PairedFrame DataCollectionDS80::getFrame()
{
	PairedFrame frame;

	frame.valid_frame = false;
	xv::Pose pose;
	m_lock_pose_data.lock();
	if (m_pose_data.size() < 1){
		m_lock_pose_data.unlock();
		return frame;
	}
	pose = m_pose_data.front();
	m_pose_data.pop();
	m_lock_pose_data.unlock();

	xv::Imu imu;
	double imu_pair_delta_us = 0.00f;
	double imu_pair_quality = 0.00f;
	bool imu_find = find_nearest_imu(pose.edgeTimestampUs(), imu, imu_pair_delta_us, imu_pair_quality);

	frame.valid_frame = true;

	frame.position.x() = pose.translation()[0];
	frame.position.y() = pose.translation()[1];
	frame.position.z() = pose.translation()[2];
	frame.rotation = Eigen::Quaterniond(pose.quaternion()[3], pose.quaternion()[0], pose.quaternion()[1], pose.quaternion()[2]);
	frame.linear_velocity.x() = pose.linearVelocity()[0];
	frame.linear_velocity.y() = pose.linearVelocity()[1];
	frame.linear_velocity.z() = pose.linearVelocity()[2];
	frame.angular_velocity.x() = pose.angularVelocity()[0];
	frame.angular_velocity.y() = pose.angularVelocity()[1];
	frame.angular_velocity.z() = pose.angularVelocity()[2];
	frame.confidence = pose.confidence();
	frame.timestamp_ns = pose.edgeTimestampUs() * 1000;

	if (imu_find){
		frame.imu_angular_velocity.x() = imu.gyro[0];
		frame.imu_angular_velocity.y() = imu.gyro[1];
		frame.imu_angular_velocity.z() = imu.gyro[2];
		frame.imu_acceleration.x() = imu.accel[0];
		frame.imu_acceleration.y() = imu.accel[1];
		frame.imu_acceleration.z() = imu.accel[2];
		frame.imu_timestamp_ns = imu.edgeTimestampUs * 1000;
		frame.pair_delta_us = imu_pair_delta_us;
		frame.pair_quality = imu_pair_quality;
	}
	static uint32_t static_frame_id = 0;
	frame.frame_id = ++static_frame_id;
	frame.source = "DS80";
	frame.work_mode = m_dataCollectionConfig.work_mode;
	frame.map_loaded = m_map_load;
	
	return frame;
}

bool DataCollectionDS80::setWorkMode(WorkMode mode, std::string map_file_path)
{
	if (!m_device || !m_fisheye || !m_slam)
		return false;

	if (!map_file_path.empty()){
		m_map_load = false;
		if (!m_device)
			return false;

		m_slam->stop();
		if (!m_slam->start(xv::Slam::Mode::Mixed)){
			printf("slam start failed!\n");
			return false;
		}

		printf("load cslam map and switch to cslam\n");

		if (m_mapStream.open(map_file_path, std::ios::binary | std::ios::in) == nullptr) {
			printf("open map file:%s failed!\n", map_file_path.c_str());
			return false;
		}
		m_slam->loadMapAndSwitchToCslam(
			m_mapStream,
			[this](int map_quality) {
				printf("load map quality:%d\n", map_quality);
				m_mapStream.close();
			}, [this](float percent) {
				if (m_dataCollectionConfig.cb_map_percent){
					if (1 == percent)
						m_map_load = true;
					m_dataCollectionConfig.cb_map_percent(percent * 100);
				}
			}
		);
		return false;
	}
	
	if (m_dataCollectionConfig.work_mode == WorkMode::VIO){

		m_slam->stop();
		// 启动SLAM
		if (!m_slam->start()) {
			printf("SLAM start failed!\n");
			return false;
		}

		// 启动IMU流
		if(m_imu_callbackId != 0){ 
			m_device->imuSensor()->unregisterCallback(m_imu_callbackId);
			m_imu_callbackId = 0;
		}
			
		m_device->imuSensor()->stop();

		if (m_device->imuSensor()) {
			m_imu_callbackId = m_device->imuSensor()->registerCallback(
				[this](xv::Imu const& imu) {
					// 处理IMU数据
					m_lock_imu_data.lock();
					if (m_imu_data.size() > m_dataCollectionConfig.buffer_size)
						m_imu_data.pop_front();
					m_imu_data.push_back(imu);
					m_lock_imu_data.unlock();
				}
			);
			m_device->imuSensor()->start();
		}

		if (m_run_posethread){
			m_run_posethread = false;
			m_pose_thread.join();
		}

		m_run_posethread = true;
		std::thread poseThr(&DataCollectionDS80::poseThread, this);
		m_pose_thread.swap(poseThr);
	}
	else if (m_dataCollectionConfig.work_mode == WorkMode::MAP){
		if (m_dataCollectionConfig.map_file_path.size() < 1){
			printf("map_file_path is empty!\n");
			return false;
		}
		if (m_mapStream.open(m_dataCollectionConfig.map_file_path, std::ios::binary | std::ios::out | std::ios::trunc) == nullptr) {
			printf("open map_file_path:%s failed!\n", m_dataCollectionConfig.map_file_path.c_str());
			return false;
		}

		m_slam->saveMapAndSwitchToCslam(m_mapStream, [this](int status_of_saved_map, int map_quality) {
			printf("Save map (quality is %d/100) and switch to CSlam:");
			switch (status_of_saved_map)
			{
			case  2: printf(" Map well saved. \n"); break;
			case -1: printf(" Map cannot be saved, an error occured when trying to save it. \n"); break;
			default: printf(" Unrecognized status of saved map \n"); break;
			}
			m_mapStream.close();
			}, [this](float percent) {
				if (m_dataCollectionConfig.cb_map_percent)
				{
					m_dataCollectionConfig.cb_map_percent(percent * 100);
				}
			}
		);
	}
	else if (m_dataCollectionConfig.work_mode == WorkMode::MAP_INIT_VIO){

	}
	else{
		return false;
	}

	return true;
}

bool DataCollectionDS80::isMapLoaded() const
{
	return m_map_load;
}

bool DataCollectionDS80::isMapStable() const
{
	return m_isMapStable;
}

void DataCollectionDS80::setVIOInitialPose(const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation)
{

}

MappingQuality DataCollectionDS80::analyzeMappingQuality(int64_t duration_ns)
{
	MappingQuality mapQuality;
	mapQuality.overall_score = 0;      // 总体质量分数（0.0-1.0）
	mapQuality.confidence_avg = 0;     // 平均置信度
	mapQuality.confidence_min = 0;      // 最小置信度
	mapQuality.confidence_max = 0;      // 最大置信度
	mapQuality.stability_score = 0;    // 稳定性分数
	mapQuality.analysis_duration = 0; // 分析时长（纳秒）
	return mapQuality;
}

void DataCollectionDS80::setExposureMode(ExposureMode mode)
{
	if (m_fisheye)
	{
		m_fisheye->setExposure((int)mode);
	}
}

void DataCollectionDS80::setExposureValue(double exposure_value)
{
	if (m_fisheye)
	{
		m_fisheye->setExposure((int)ExposureMode::MANUAL, exposure_value);
	}
}
