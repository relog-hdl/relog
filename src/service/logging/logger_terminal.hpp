#ifndef RELOG_SERVICE_LOGGING_LOGGER_TERMINAL_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_TERMINAL_HPP

#include "logger.hpp"

#include <cstdio>

namespace relog::service::logging
{

class LoggerBackendTerminal final : public ILoggerBackend
{
		FILE* m_stream;
		bool m_flush = false;

	public:
		LoggerBackendTerminal(FILE* stream);

		LoggerBackendTerminal();

		virtual ~LoggerBackendTerminal();

		void log(std::string plain_message);

		bool getFlush();

		void setFlush(bool flush);
};

class LoggerFormatterTerminalPlain final : public LoggerFormatter<std::string>
{
	public:
		LoggerFormatterTerminalPlain();

		~LoggerFormatterTerminalPlain();

		std::string format(LogDomainProxy<std::string> domain, LogLevel level, std::string message) override;
};

} // namespace relog::service::logging

#endif
