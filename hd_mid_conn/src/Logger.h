#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"

using namespace spdlog;
class Logger
{
public:
	static Logger& GetInstance()
	{
		static Logger instance;
		return instance;
	}

	template<typename FormatString, typename... Args>
	void Info(const FormatString& fmt, Args&&...args)
	{
		m_InfoLogger->log(level::info, fmt, std::forward<Args>(args)...);
	}
	
	template<typename FormatString, typename... Args>
	void Warn(const FormatString& fmt, Args&&...args)
	{
		m_SysLogger->log(level::warn, fmt, std::forward<Args>(args)...);
	}
private:
	Logger()
	{
		m_InfoLogger = rotating_logger_mt<async_factory>("ReqRsp", m_InfoLogPath, m_MaxFileSize, m_MaxFileCnt);
		m_SysLogger = rotating_logger_mt<async_factory>("SysInfo", m_SysLogPath, m_MaxFileSize, m_MaxFileCnt);

		flush_every(std::chrono::seconds(5));
	}

	~Logger()
	{
		spdlog::shutdown();
	}

private:
	const std::string m_InfoLogPath = "mid/info.log";
	const std::string m_SysLogPath = "mid/sys.log";
	const size_t m_MaxFileSize = 100 * 1024 * 1024;
	const size_t m_MaxFileCnt = 5;

	std::shared_ptr<logger> m_InfoLogger;
	std::shared_ptr<logger> m_SysLogger;
};
