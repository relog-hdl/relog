#ifndef RELOG_SERVICE_LOGGING_LOGGER_BACKEND_TERMINAL_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_BACKEND_TERMINAL_HPP

#include <service/logging/log_domain.hxx>
#include <service/logging/log_level.hpp>
#include <service/logging/logger_backend.hpp>

#include <cstdbool>
#include <cstdio>
#include <string>

namespace relog::service::logging
{

class LoggerBackendTerminal final : public ILoggerBackend
{
		FILE* m_stream;
		bool m_flush = false;

	public:
		LoggerBackendTerminal(FILE* stream);

		LoggerBackendTerminal();

		virtual ~LoggerBackendTerminal() = default;

		void log(std::string plain_message);

		bool getFlush();

		void setFlush(bool flush);
};

} // namespace relog::service::logging

#endif
