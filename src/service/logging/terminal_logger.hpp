#ifndef RELOG_SERVICE_LOGGING_TERMINAL_LOGGER_HPP
#define RELOG_SERVICE_LOGGING_TERMINAL_LOGGER_HPP

#include "logger.hpp"
#include "logger_backend.hpp"

namespace relog::service::logging
{

class TerminalLoggerBackend : public ILoggerBackend
{
};

class TerminalLogger : public Logger
{
};

} // namespace relog::service::logging

#endif
