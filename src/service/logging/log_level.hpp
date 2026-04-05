#ifndef RELOG_SERVICE_LOGGING_LOG_LEVEL_HPP
#define RELOG_SERVICE_LOGGING_LOG_LEVEL_HPP

namespace relog::service::logging
{

enum log_level
{
	FLOW,
	DEBUG,
	INFO,
	NOTICE,
	WARNING,
	ERROR,
	CRITICAL,
	DISABLED__
};

typedef enum log_level LogLevel;

} // namespace relog::service::logging

#endif
