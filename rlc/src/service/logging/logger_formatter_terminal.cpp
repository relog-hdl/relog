#include "logger_formatter_terminal.hpp"

namespace relog::service::logging
{

std::string
LoggerFormatterTerminalPlain::format(LogDomainProxy<std::string> domain, LogLevel level, std::string message)
{
	std::string level_str;

	switch(level)
	{
		case FLOW:
			level_str = "FLW";
			break;
		case DEBUG:
			level_str = "DBG";
			break;
		case INFO:
			level_str = "INF";
			break;
		case NOTICE:
			level_str = "NOT";
			break;
		case WARNING:
			level_str = "WRN";
			break;
		case ERROR:
			level_str = "ERR";
			break;
		case CRITICAL:
			level_str = "CRT";
			break;
		default:
			level_str = "";
	}

	return level_str + " | " + domain.getFullyQualifiedName() + " - " + message + '\n';
}

} // namespace relog::service::logging
