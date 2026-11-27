#include "pipeline.hpp"

#include <pipeline/phase/inliner/inliner.hpp>
#include <pipeline/phase/lex/lexer.hpp>
#include <pipeline/phase/parse/parser.hpp>

#include <bootstrap/opts.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/logger_backend_terminal.hpp>
#include <service/logging/logger_formatter_terminal.hpp>
#include <service/logging/loggerf.hpp>

#include <iostream>
#include <sstream>

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
	using relog::pipeline::inliner::Decanter;
	using relog::pipeline::inliner::Inliner;
	using relog::pipeline::lex::Lexer;
	using relog::pipeline::parse::Parser;

	if(ssc.empty())
		return 0;

	std::stringbuf i2l_buf{std::ios::in | std::ios::out};

	std::ostream i2l_buf_in{&i2l_buf};
	std::istream i2l_buf_out{&i2l_buf};

	Decanter decanter{self};
	Inliner inliner{self, decanter, i2l_buf_in};

	inliner.run();

	if(relog::bootstrap::opts::preprocess)
	{
		if(i2l_buf_out.peek() != std::istream::traits_type::eof())
			std::cout << i2l_buf_out.rdbuf();
	}
	else
	{
		Lexer lexer{self, i2l_buf_out};
		Parser parser{self, lexer};

		parser.parse();
	}

	return 0;
}

} // namespace relog::pipeline
