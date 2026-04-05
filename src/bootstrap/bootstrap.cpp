#include "bootstrap.hpp"

#include <bootstrap/opts.cpp>

#include <service/logging/log_domain.cpp>
#include <service/logging/logger.cpp>
#include <service/logging/logger_backend_terminal.cpp>
#include <service/logging/logger_formatter_terminal.cpp>
#include <service/logging/loggerf.cpp>

#include <lib/domain/domain.cpp>

#include <memory>

namespace relog::bootstrap
{

relog::service::logging::LogDomainOwner<std::string> d{};
relog::service::logging::LogDomainProxy<std::string> r = d.get("bootstrap");
relog::service::logging::Loggerf l{r, relog::service::logging::LogLevel::INFO};

void
init_logging()
{
	auto backend = std::make_shared<relog::service::logging::LoggerBackendTerminal>();
	auto formatter = std::make_shared<relog::service::logging::LoggerFormatterTerminalPlain>();

	r.changePipeline(backend, formatter);
	r.setThreshold(relog::service::logging::LogLevel::INFO);
}

} // namespace relog::bootstrap

int
main(int argc, char* argv[])
{
	using relog::bootstrap::l;

	relog::bootstrap::init_logging();
	relog::bootstrap::parse_opts(argc, argv);

	return 0;
}
