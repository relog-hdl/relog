#ifndef RELOG_LOGGING_TERMINAL_LOGGER_HPP
#define RELOG_LOGGING_TERMINAL_LOGGER_HPP

#include "logger.hpp"
#include "logging_backend.hpp"

namespace relog::logging
{

class TerminalLoggerBackend : public ILoggingBackend
{
};

class TerminalLogger : public Logger
{
};

} // namespace relog::logging

#endif
