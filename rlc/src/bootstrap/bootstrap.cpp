#include "bootstrap.hpp"

#include <bootstrap/opts.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/logger.hpp>
#include <service/logging/logger_backend_terminal.hpp>
#include <service/logging/logger_formatter_terminal.hpp>
#include <service/logging/loggerf.hpp>

#include <lib/domain/domain.hpp>
#include <rlc/version.h>

#include <librelog/version.h>

#include <memory>

namespace relog::bootstrap
{

using relog::service::logging::LogDomainOwner;
using relog::service::logging::LogDomainProxy;
using relog::service::logging::Loggerf;
using relog::service::logging::LogLevel;

LogDomainOwner<std::string>& domain_owner = logging::get_domain_owner();
LogDomainProxy<std::string>& domain = logging::get_domain();
Loggerf& l = logging::get_logger();

void
init_logging()
{
	using relog::service::logging::LoggerBackendTerminal;
	using relog::service::logging::LoggerFormatterTerminalPlain;

	auto backend = std::make_shared<LoggerBackendTerminal>();
	auto formatter = std::make_shared<LoggerFormatterTerminalPlain>();

	domain.changePipeline(backend, formatter);
	domain.setThreshold(LogLevel::FLOW);
}

} // namespace relog::bootstrap

int
main(int argc, char* argv[])
{
	using relog::bootstrap::l;

	using relog::bootstrap::init_logging;

	init_logging();
	l.info("%s", RELOG_PROJECT_SUMMARY);
	l.info("%s", LIBRELOG_PROJECT_SUMMARY);

	using relog::bootstrap::opts::parse_opts;
	using relog::bootstrap::opts::ParseOptsResult;

	int ind;
	ParseOptsResult ret = parse_opts(&ind, argc, argv);

	switch(ret)
	{
		case ParseOptsResult::ARGUMENT_ERROR:
			return 1;
		case ParseOptsResult::UNKNOWN_ERROR:
			return 1;
		default:
			l.flow("Parsed options.");
			break;
	}

	return 0;
}
