#ifndef RELOG_LOG_HPP
#define RELOG_LOG_HPP

namespace relog
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

} // namespace relog

#endif
