#include "terminal_logger.hpp"

#include <cstdio>

namespace relog::service::logging
{

LoggerBackendTerminal::LoggerBackendTerminal(FILE* stream) : m_stream(stream) {}

LoggerBackendTerminal::LoggerBackendTerminal() : LoggerBackendTerminal(stdout) {}

LoggerBackendTerminal::~LoggerBackendTerminal() {}

void
LoggerBackendTerminal::log(std::string plain_message)
{
	int fret;

	fret = fputs(plain_message.c_str(), m_stream);

	if(fret == EOF)
	{
		perror("TerminalLoggerBackend: log: fputs");
		return;
	}

	if(m_flush)
	{
		fret = fflush(m_stream);

		if(fret == EOF)
			perror("TerminalLoggerBackend: log: fflush");
	}
}

bool
LoggerBackendTerminal::getFlush()
{
	return m_flush;
}

void
LoggerBackendTerminal::setFlush(bool flush)
{
	m_flush = flush;
}

LoggerFormatterTerminalPlain::LoggerFormatterTerminalPlain() {}

LoggerFormatterTerminalPlain::~LoggerFormatterTerminalPlain() {}

std::string
LoggerFormatterTerminalPlain::format(LogDomainProxy<std::string> domain, LogLevel level, std::string message)
{
	std::string level_str;

	switch(level)
	{
		case FLOW:
			level_str = "FLW";
			break;
		case DEBUG:
			level_str = "DBG";
			break;
		case INFO:
			level_str = "INF";
			break;
		case NOTICE:
			level_str = "NOT";
			break;
		case WARNING:
			level_str = "WRN";
			break;
		case ERROR:
			level_str = "ERR";
			break;
		case CRITICAL:
			level_str = "CRT";
			break;
	}

	return level_str + " | " + domain.getFullyQualifiedName() + " - " + message + '\n';
}

} // namespace relog::service::logging
