#ifndef RELOG_SERVICE_LOGGING_LOG_DOMAIN_HPP
#define RELOG_SERVICE_LOGGING_LOG_DOMAIN_HPP

#include <service/logging/log_level.hpp>

#include <lib/domain/domain.hpp>

#include <cstdbool>
#include <memory>
#include <set>

namespace relog::service::logging
{

/* Forward declarations for classes of this header */
template<typename TMessage>
class LogDomain;

template<typename TMessage>
class LogDomainProxy;

template<typename TMessage>
class LogDomainOwner;

/* Forward declarations for domain-attached objects */
struct ILoggerBackend;

template<typename TMessage>
class LoggerFormatter;

template<typename TMessage>
class LogDomain final : public relog::lib::Domain<LogDomain<TMessage>>
{
		using relog::lib::Domain<LogDomain<TMessage>>::Domain;

	protected:
		LogLevel m_level = DISABLED__;
		std::set<std::pair<std::shared_ptr<ILoggerBackend>, std::shared_ptr<LoggerFormatter<TMessage>>>> m_pipeline;

	public:
		void setThreshold(LogLevel level, bool hierarchical = true);

		void changePipeline(std::shared_ptr<ILoggerBackend> backend, std::shared_ptr<LoggerFormatter<TMessage>> formatter);

		std::set<std::pair<std::weak_ptr<ILoggerBackend>, std::weak_ptr<LoggerFormatter<TMessage>>>> getPipeline();

		void log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message);

	protected:
		void onCreated() override;
};

template<typename TMessage>
class LogDomainProxy final : public relog::lib::DomainProxy<LogDomainProxy<TMessage>, LogDomain<TMessage>>
{
		using relog::lib::DomainProxy<LogDomainProxy<TMessage>, LogDomain<TMessage>>::DomainProxy;

	public:
		void setThreshold(LogLevel level, bool hierarchical = true);

		void changePipeline(std::shared_ptr<ILoggerBackend> backend, std::shared_ptr<LoggerFormatter<TMessage>> formatter);

		std::set<std::pair<std::weak_ptr<ILoggerBackend>, std::weak_ptr<LoggerFormatter<TMessage>>>> getPipeline();

		void log(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message);
};

template<typename TMessage>
class LogDomainOwner final : public relog::lib::DomainOwner<LogDomainProxy<TMessage>, LogDomain<TMessage>>
{
		using relog::lib::DomainOwner<LogDomainProxy<TMessage>, LogDomain<TMessage>>::DomainOwner;
};

} // namespace relog::service::logging

#endif
