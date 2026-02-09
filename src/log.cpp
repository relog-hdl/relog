#include "log.hpp"

#include <aliases.hpp>

#include <cstdio>

namespace relog
{

class LogDomain
{
	private:
		LogDomain* parent;
		std::string name;

	public:
		LogDomain(LogDomain& parent, std::string& name)
		{
			this.parent = &parent;
			this.name = name; // TODO explicit deep copy
		}

		std::string getName()
		{
			return name; // TODO explicit deep copy?
		}

		// TODO variadic-function based constructor, i.e. LogDomain("lex", "lex.cpp")

		// TODO comparison, i.e. LogDomain("lex", "lex.cpp", "do_something()") > LogDomain("lex", "lex.cpp")
}

struct ILoggerBackend
{
	virtual void log(std::string& plain_message) = 0;
	virtual ~ILoggerBackend() = 0;
}

class ConsoleLoggerBackend final : public ILoggerBackend
{
	// TODO log by outputting to console
}

class FileLoggerBackend final : public ILoggerBackend
{
	// TODO log by outputting to file, close file in destructor
}

struct ILoggerFormatter
{
	virtual std::string& format(LogDomain& domain, LogLevel level, std::string& message) = 0;
	virtual ~ILoggerFormatter() = 0;
}

class CompilerStyleFormatter final : public ILoggerFormatter
{
	// TODO format messages like a compiler would
}

class Logger
{
	public:
		void log(LogDomain& domain, LogLevel level, std::string& message);

	// TODO multiple backends and formatters per log domain supported, enable/disable logging by domain
}

class DomainLogger
{
	private:
		Logger* logger;
		LogDomain domain;

	public:
		DomainLogger(Logger& logger, LogDomain domain)
		{
			this.logger = &logger;
			this.domain = domain; // TODO Make a deep copy
		}

		void log(LogLevel level, std::string& message)
		{
			logger->log(domain, level, message);
		}

		void log(LogDomain& branch, LogLevel level, std::string& message)
		{
			// TODO add branch onto domain and log
		}
}

} // namespace relog
