#ifndef RELOG_SERVICE_LOGGING_LOGGER_BACKEND_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_BACKEND_HPP

#include <string>

namespace relog::service::logging
{

struct ILoggerBackend
{
		virtual ~ILoggerBackend() = default;

		virtual void log(std::string plain_message) = 0;
};

} // namespace relog::service::logging

#endif
