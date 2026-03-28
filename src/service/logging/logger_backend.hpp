#ifndef RELOG_SERVICE_LOGGING_LOGGER_BACKEND_HPP
#define RELOG_SERVICE_LOGGING_LOGGER_BACKEND_HPP

namespace relog::service::logging
{

struct ILoggerBackend
{
		virtual ~ILoggerBackend() = 0;
};

} // namespace relog::service::logging

#endif
