#include "pipeline.hpp"

#include <pipeline/phase/lex/lexer.hpp>
#include <pipeline/phase/parse/parser.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/logger_backend_terminal.hpp>
#include <service/logging/logger_formatter_terminal.hpp>
#include <service/logging/loggerf.hpp>

#include <fstream>
#include <iostream>
#include <vector>

#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace relog::pipeline
{

using relog::service::logging::LogDomainOwner;
using relog::service::logging::LogDomainProxy;
using relog::service::logging::Loggerf;
using relog::service::logging::LogLevel;

void
Pipeline::init_logging()
{
	using relog::service::logging::LoggerBackendTerminal;
	using relog::service::logging::LoggerFormatterTerminalPlain;

	auto backend = std::make_shared<LoggerBackendTerminal>();
	auto formatter = std::make_shared<LoggerFormatterTerminalPlain>();

	domain.changePipeline(backend, formatter);
	domain.setThreshold(LogLevel::FLOW);
}

int
Pipeline::entrypoint(Pipeline& pl, std::vector<std::ifstream>& files)
{
	using relog::pipeline::lex::Lexer;
	using relog::pipeline::parse::Parser;

	Lexer lexer{pl};
	Parser parser{pl, lexer};
	parser.parse();

	return 0;
}

} // namespace relog::pipeline
