#include "DataCollectionA1088.h"

DataCollectionA1088::DataCollectionA1088()
{

}
DataCollectionA1088::~DataCollectionA1088()
{

}

bool DataCollectionA1088::initialize(const DataCollectionConfig& config)
{
	m_dataCollectionConfig = config;
	return false;
}

void DataCollectionA1088::cleanup()
{

}

bool DataCollectionA1088::start()
{
	return false;
}

void DataCollectionA1088::stop()
{

}

PairedFrame DataCollectionA1088::getFrame()
{
	PairedFrame frame;
	return frame;
}

bool DataCollectionA1088::setWorkMode(WorkMode mode, std::string map_file_path)
{
	return false;
}

bool DataCollectionA1088::isMapLoaded() const
{
	return false;
}

bool DataCollectionA1088::isMapStable() const
{
	return false;
}

void DataCollectionA1088::setVIOInitialPose(const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation)
{

}

MappingQuality DataCollectionA1088::analyzeMappingQuality(int64_t duration_ns)
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

void DataCollectionA1088::setExposureMode(ExposureMode mode)
{

}

void DataCollectionA1088::setExposureValue(double exposure_value)
{

}
