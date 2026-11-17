#include "logger_backend_terminal.hpp"

namespace relog::service::logging
{

LoggerBackendTerminal::LoggerBackendTerminal(FILE* stream) : m_stream(stream) {}

LoggerBackendTerminal::LoggerBackendTerminal() : LoggerBackendTerminal(stdout) {}

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

} // namespace relog::service::logging
