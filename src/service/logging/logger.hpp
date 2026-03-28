#ifndef RELOG_SERVICE_LOGGING_LOGGER_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_HPP

#include <lib/domain/domain.hpp>

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

class LogDomain final : public relog::lib::Domain<LogDomain>
{
		using relog::lib::Domain<LogDomain>::Domain;
};

class LogDomainProxy final : public relog::lib::DomainProxy<LogDomainProxy, LogDomain>
{
		using relog::lib::DomainProxy<LogDomainProxy, LogDomain>::DomainProxy;
};

class LogDomainOwner final : public relog::lib::DomainOwner<LogDomainProxy, LogDomain>
{
		using relog::lib::DomainOwner<LogDomainProxy, LogDomain>::DomainOwner;
};

struct ILoggerBackend
{
		virtual ~ILoggerBackend() = 0;

		virtual void log(std::string& plain_message) = 0;
};

template<typename TMessage>
struct ILoggerFormatter
{
		virtual ~ILoggerFormatter() = 0;

		virtual std::string format(LogDomain& domain, LogLevel level, TMessage message) = 0;
};

template<typename TMessage>
struct ILogger
{
		virtual ~ILogger() = 0;

		virtual void log(LogDomain& domain, LogLevel level, TMessage message) = 0;
};

} // namespace relog::service::logging

#endif
