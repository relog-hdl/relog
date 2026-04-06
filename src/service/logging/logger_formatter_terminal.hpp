#ifndef RELOG_SERVICE_LOGGING_LOGGER_FORMATTER_TERMINAL_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_FORMATTER_TERMINAL_HPP

#include "logger_formatter_terminal.h"

#include <service/logging/log_domain.hpp>
#include <service/logging/log_level.hpp>
#include <service/logging/logger_formatter.hpp>

#include <string>

namespace relog::service::logging
{

class LoggerFormatterTerminalPlain final : public LoggerFormatter<std::string>
{
	public:
		LoggerFormatterTerminalPlain() = default;

		virtual ~LoggerFormatterTerminalPlain() = default;

		std::string format(LogDomainProxy<std::string> domain, LogLevel level, std::string message) override;
};

} // namespace relog::service::logging

#endif
