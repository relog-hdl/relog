#include "logger.hpp"

#include <cstdio>
#include <memory>

namespace relog::service::logging
{

template<typename TMessage>
void
LogDomain<TMessage>::setThreshold(LogLevel level, bool hierarchical)
{
	m_level = level;
	if(hierarchical)
	{
		for(auto it = this->m_children.begin(); it != this->m_children.end(); ++it)
			it->second->setThreshold(level, hierarchical);
	}
}

template<typename TMessage>
void
LogDomain<TMessage>::changePipeline(std::shared_ptr<ILoggerBackend> backend, std::shared_ptr<LoggerFormatter<TMessage>> formatter)
{
	std::list<std::pair<std::shared_ptr<ILoggerBackend>, std::shared_ptr<LoggerFormatter<TMessage>>>> prev_pairs;
	for(auto it = m_pipeline.begin(); it != m_pipeline.end(); ++it)
		if(it->first == backend || it->second == formatter)
			prev_pairs.push_back(*it);
	for(auto it = prev_pairs.begin(); it != prev_pairs.end(); ++it)
		m_pipeline.erase(*it);
	m_pipeline.insert(std::make_pair(backend, formatter));
}

template<typename TMessage>
std::set<std::pair<std::weak_ptr<ILoggerBackend>, std::weak_ptr<LoggerFormatter<TMessage>>>>
LogDomain<TMessage>::getPipeline()
{
	std::set<std::pair<std::weak_ptr<ILoggerBackend>, std::weak_ptr<LoggerFormatter<TMessage>>>> ret;
	for(auto it = m_pipeline.begin(); it != m_pipeline.end(); ++it)
		ret.insert(std::make_pair(std::weak_ptr(it->first), std::weak_ptr(it->second)));
	return ret;
}

template<typename TMessage>
void
LogDomain<TMessage>::log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message)
{
	if(level >= m_level)
	{
		for(auto it = m_pipeline.begin(); it != m_pipeline.end(); ++it)
			it->first->log(it->second->format(domain, level, message));
		if(!this->isRoot())
		{
			auto parent = this->m_parent.lock();
			parent->log(domain, level, message);
		}
	}
}

template<typename TMessage>
void
LogDomain<TMessage>::onCreated()
{
	if(!this->isRoot())
	{
		auto parent = this->m_parent.lock();
		m_level = parent->m_level;
	}
}

template<typename TMessage>
void
LogDomainProxy<TMessage>::setThreshold(LogLevel level, bool hierarchical)
{
	this->m_domain->setThreshold(level, hierarchical);
}

template<typename TMessage>
void
LogDomainProxy<TMessage>::changePipeline(std::shared_ptr<ILoggerBackend> backend, std::shared_ptr<LoggerFormatter<TMessage>> formatter)
{
	this->m_domain->changePipeline(backend, formatter);
}

template<typename TMessage>
std::set<std::pair<std::weak_ptr<ILoggerBackend>, std::weak_ptr<LoggerFormatter<TMessage>>>>
LogDomainProxy<TMessage>::getPipeline()
{
	return this->m_domain->getPipeline();
}

ILoggerBackend::~ILoggerBackend() {}

template<typename TMessage>
LoggerFormatter<TMessage>::~LoggerFormatter() {}

template<typename TMessage>
void
LogDomainProxy<TMessage>::log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message)
{
	this->m_domain->log(domain, level, message);
}

template<typename TMessage>
Logger<TMessage>::Logger(LogDomainProxy<TMessage> domain, LogLevel level) : m_domain(domain), m_level(level) {}

template<typename TMessage>
Logger<TMessage>::~Logger() {}

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
