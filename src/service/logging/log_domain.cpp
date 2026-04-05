#include "log_domain.hpp"

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

template<typename TMessage>
void
LogDomainProxy<TMessage>::log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message)
{
	this->m_domain->log(domain, level, message);
}

} // namespace relog::service::logging
