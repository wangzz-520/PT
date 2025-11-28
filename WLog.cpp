#include "WLog.h"
#include <QApplication>

WLog::~WLog()
{

}

WLog * WLog::getInstance()
{
	static WLog log;
	return &log;
}

void WLog::info(const std::string &str)
{
    m_logger->info(str);
}

void WLog::info(const QString &str)
{
    info(str.toStdString());
}

void WLog::debug(const std::string &str)
{
    m_logger->debug(str);
}

void WLog::debug(const QString &str)
{
    debug(str.toStdString());
}

void WLog::warn(const std::string &str)
{
    m_logger->warn(str);
}

void WLog::warn(const QString &str)
{
    warn(str.toStdString());
}

void WLog::error(const std::string &str)
{
    m_logger->error(str);
}

void WLog::error(const QString &str)
{
    error(str.toStdString());
}

WLog::WLog()
{
    m_logger = spdlog::rotating_logger_mt("logger", "./logs/log.txt", 1048576 * 20, 10);//50M
	spdlog::flush_every(std::chrono::seconds(3));
}
