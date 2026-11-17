#include "loggerf.hpp"

#include <cstdarg>

namespace relog::service::logging
{

void
Loggerf::log(LogDomainProxy<std::string> domain, LogLevel level, const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vlog(domain, level, format, argp);
	va_end(argp);
}

void
Loggerf::vlog(LogDomainProxy<std::string> domain, LogLevel level, const char* format, va_list argp)
{
	char buf[RELOG_SERVICE_LOGGING_LOGGER_STANDARD_BUFFER_SIZE];

	int ret = std::vsnprintf(buf, RELOG_SERVICE_LOGGING_LOGGER_STANDARD_BUFFER_SIZE, format, argp);

	if(ret < 0)
	{
		throw std::runtime_error("Formatting error: unexpected std::vsnprintf result");
	}
	else if(ret < RELOG_SERVICE_LOGGING_LOGGER_STANDARD_BUFFER_SIZE)
		Logger::log(domain, level, buf);
	else
	{
		auto size = static_cast<size_t>(ret) + 1;
		std::unique_ptr<char[]> new_buf{new char[size]};
		std::vsnprintf(new_buf.get(), size, format, argp);
		Logger::log(domain, level, new_buf.get());
	}
}

void
Loggerf::log(LogLevel level, const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vlog(level, format, argp);
	va_end(argp);
}

void
Loggerf::vlog(LogLevel level, const char* format, va_list argp)
{
	this->vlog(this->m_domain, level, format, argp);
}

void
Loggerf::log(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vlog(format, argp);
	va_end(argp);
}

void
Loggerf::vlog(const char* format, va_list argp)
{
	this->vlog(this->m_level, format, argp);
}

void
Loggerf::flow(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vflow(format, argp);
	va_end(argp);
}

void
Loggerf::vflow(const char* format, va_list argp)
{
	this->vlog(FLOW, format, argp);
}

void
Loggerf::debug(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vdebug(format, argp);
	va_end(argp);
}

void
Loggerf::vdebug(const char* format, va_list argp)
{
	this->vlog(DEBUG, format, argp);
}

void
Loggerf::info(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vinfo(format, argp);
	va_end(argp);
}

void
Loggerf::vinfo(const char* format, va_list argp)
{
	this->vlog(INFO, format, argp);
}

void
Loggerf::notice(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vnotice(format, argp);
	va_end(argp);
}

void
Loggerf::vnotice(const char* format, va_list argp)
{
	this->vlog(NOTICE, format, argp);
}

void
Loggerf::warning(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vwarning(format, argp);
	va_end(argp);
}

void
Loggerf::vwarning(const char* format, va_list argp)
{
	this->vlog(WARNING, format, argp);
}

void
Loggerf::error(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	verror(format, argp);
	va_end(argp);
}

void
Loggerf::verror(const char* format, va_list argp)
{
	this->vlog(ERROR, format, argp);
}

void
Loggerf::critical(const char* format...)
{
	va_list argp;
	va_start(argp, format);
	vcritical(format, argp);
	va_end(argp);
}

void
Loggerf::vcritical(const char* format, va_list argp)
{
	this->vlog(CRITICAL, format, argp);
}

} // namespace relog::service::logging
