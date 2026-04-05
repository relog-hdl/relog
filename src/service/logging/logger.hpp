#ifndef RELOG_SERVICE_LOGGING_LOGGER_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_HPP

#include <service/logging/log_domain.hpp>
#include <service/logging/log_level.hpp>
#include <service/logging/logger_backend.hpp>
#include <service/logging/logger_formatter.hpp>

#include <lib/domain/domain.hpp>

#include <string>

namespace relog::service::logging
{

template<typename TMessage>
class Logger
{
	protected:
		LogDomainProxy<TMessage> m_domain;
		LogLevel m_level;

	public:
		Logger(LogDomainProxy<TMessage> domain, LogLevel level);

		virtual ~Logger() = default;

		void log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message);

		void log(LogLevel level, TMessage message);

		void log(TMessage message);

		void flow(TMessage message);

		void debug(TMessage message);

		void info(TMessage message);

		void notice(TMessage message);

		void warning(TMessage message);

		void error(TMessage message);

		void critical(TMessage message);
};

} // namespace relog::service::logging

#endif
