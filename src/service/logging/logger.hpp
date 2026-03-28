#ifndef RELOG_SERVICE_LOGGING_LOGGER_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_HPP

namespace relog::service::logging
{

enum log_level
{
	DEBUG,
	INFO,
	NOTICE,
	WARNING,
	ERROR,
	CRITICAL
};

typedef enum log_level LogLevel;

class Logger
{
};

} // namespace relog::service::logging

#endif
