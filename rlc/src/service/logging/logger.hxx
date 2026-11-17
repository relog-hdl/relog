#ifndef RELOG_SERVICE_LOGGING_LOGGER_HXX
#define RELOG_SERVICE_LOGGING_LOGGER_HXX

#include "logger.hpp"

namespace relog::service::logging
{

template<typename TMessage>
Logger<TMessage>::Logger(LogDomainProxy<TMessage> domain, LogLevel level) : m_domain(domain), m_level(level) {}

template<typename TMessage>
void
Logger<TMessage>::log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message)
{
	domain.log(domain, level, message);
}

template<typename TMessage>
void
Logger<TMessage>::log(LogLevel level, TMessage message)
{
	this->log(m_domain, level, message);
}

template<typename TMessage>
void
Logger<TMessage>::log(TMessage message)
{
	this->log(m_level, message);
}

template<typename TMessage>
void
Logger<TMessage>::flow(TMessage message)
{
	this->log(FLOW, message);
}

template<typename TMessage>
void
Logger<TMessage>::debug(TMessage message)
{
	this->log(DEBUG, message);
}

template<typename TMessage>
void
Logger<TMessage>::info(TMessage message)
{
	this->log(INFO, message);
}

template<typename TMessage>
void
Logger<TMessage>::notice(TMessage message)
{
	this->log(NOTICE, message);
}

template<typename TMessage>
void
Logger<TMessage>::warning(TMessage message)
{
	this->log(WARNING, message);
}

template<typename TMessage>
void
Logger<TMessage>::error(TMessage message)
{
	this->log(ERROR, message);
}

template<typename TMessage>
void
Logger<TMessage>::critical(TMessage message)
{
	this->log(CRITICAL, message);
}

} // namespace relog::service::logging

#endif
