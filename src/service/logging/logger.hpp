#ifndef RELOG_SERVICE_LOGGING_LOGGER_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_HPP

#include <lib/domain/domain.hpp>

#include <cstdbool>
#include <map>
#include <memory>
#include <set>

namespace relog::service::logging
{

template<typename TMessage>
class LogDomain;

template<typename TMessage>
class LogDomainProxy;

template<typename TMessage>
class LogDomainOwner;

struct ILoggerBackend;

template<typename TMessage>
class LoggerFormatter;

template<typename TMessage>
class Logger;

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

struct ILoggerBackend
{
		virtual ~ILoggerBackend();

		virtual void log(std::string plain_message) = 0;
};

template<typename TMessage>
class LoggerFormatter
{
	public:
		virtual ~LoggerFormatter();

		virtual std::string format(LogDomainProxy<TMessage> domain, LogLevel level, TMessage message) = 0;
};

template<typename TMessage>
class Logger
{
	protected:
		LogDomainProxy<TMessage> m_domain;
		LogLevel m_level;

	public:
		Logger(LogDomainProxy<TMessage> domain, LogLevel level);

		virtual ~Logger();

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
