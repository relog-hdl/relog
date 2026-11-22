#include "pipeline.hpp"

#include <pipeline/phase/lex/lexer.hpp>
#include <pipeline/phase/parse/parser.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/logger_backend_terminal.hpp>
#include <service/logging/logger_formatter_terminal.hpp>
#include <service/logging/loggerf.hpp>

#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace relog::pipeline
{

using relog::service::logging::LogDomainOwner;
using relog::service::logging::LogDomainProxy;
using relog::service::logging::Loggerf;
using relog::service::logging::LogLevel;
using relog::service::source::SourceStackController;

Pipeline::Pipeline(SourceStackController& ssc_) : self(*this), ssc(ssc_) {}

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
Pipeline::entrypoint()
{
	using relog::pipeline::lex::Lexer;
	using relog::pipeline::parse::Parser;

	if(ssc.empty())
		return 0;

	Lexer lexer{self};
	Parser parser{self, lexer};

	parser.parse();

	return 0;
}

} // namespace relog::pipeline
