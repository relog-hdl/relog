#ifndef RELOG_SERVICE_LOGGING_LOGGER_FORMATTER_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_FORMATTER_HPP

#include <service/logging/log_domain.hpp>

namespace relog::service::logging
{

template<typename TMessage>
class LoggerFormatter
{
	public:
		virtual ~LoggerFormatter() = default;

		virtual std::string format(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message) = 0;
};

} // namespace relog::service::logging

#endif
