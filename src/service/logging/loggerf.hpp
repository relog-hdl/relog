#ifndef RELOG_SERVICE_LOGGING_LOGGERF_HPP
#define RELOG_SERVICE_LOGGING_LOGGERF_HPP

#include <service/logging/log_domain.hpp>
#include <service/logging/log_level.hpp>
#include <service/logging/logger.hpp>

#include <string>

#define RELOG_SERVICE_LOGGING_LOGGER_STANDARD_BUFFER_SIZE 2048

namespace relog::service::logging
{

class Loggerf final : public Logger<std::string>
{
		using Logger<std::string>::Logger;

	public:
		void log(LogDomainProxy<std::string> domain, LogLevel level, const char* format...) __attribute__((format(printf, 4, 5)));

		void vlog(LogDomainProxy<std::string> domain, LogLevel level, const char* format, va_list argp) __attribute__((format(printf, 4, 0)));

		void log(LogLevel level, const char* format...) __attribute__((format(printf, 3, 4)));

		void vlog(LogLevel level, const char* format, va_list argp) __attribute__((format(printf, 3, 0)));

		void log(const char* format...) __attribute__((format(printf, 2, 3)));

		void vlog(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void flow(const char* format...) __attribute__((format(printf, 2, 3)));

		void vflow(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void debug(const char* format...) __attribute__((format(printf, 2, 3)));

		void vdebug(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void info(const char* format...) __attribute__((format(printf, 2, 3)));

		void vinfo(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void notice(const char* format...) __attribute__((format(printf, 2, 3)));

		void vnotice(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void warning(const char* format...) __attribute__((format(printf, 2, 3)));

		void vwarning(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void error(const char* format...) __attribute__((format(printf, 2, 3)));

		void verror(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));

		void critical(const char* format...) __attribute__((format(printf, 2, 3)));

		void vcritical(const char* format, va_list argp) __attribute__((format(printf, 2, 0)));
};

} // namespace relog::service::logging

#endif
