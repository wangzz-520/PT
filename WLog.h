#ifndef _WLOG_H_
#define _WLOG_H_

#include <QString>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

class WLog
{
public:
	~WLog();
	static WLog *getInstance();

public:
	void info(const std::string &str);
    void info(const QString &str);

	void debug(const std::string &str);
    void debug(const QString &str);

	void warn(const std::string &str);
    void warn(const QString &str);

	void error(const std::string &str);
    void error(const QString &str);

private:
	explicit WLog();

	std::shared_ptr<spdlog::logger> m_logger;
};

#endif

