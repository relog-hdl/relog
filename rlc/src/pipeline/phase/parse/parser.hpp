#ifndef RELOG_PIPELINE_PHASE_PARSE_PARSER_HPP
#define RELOG_PIPELINE_PHASE_PARSE_PARSER_HPP

#include <pipeline/phase/parse/parser.y.hpp>

#include <pipeline/pipeline.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

namespace relog::pipeline::parse
{

class Parser final : public Parser_BisonParser
{
	public:
		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = relog::pipeline::Pipeline::get_domain() / "parse";
			return domain_;
		}

		static relog::service::logging::Loggerf&
		get_logger()
		{
			static relog::service::logging::Loggerf _logger{get_domain(), relog::service::logging::LogLevel::INFO};
			return _logger;
		}

	protected:
		relog::service::logging::LogDomainProxy<std::string>& domain = get_domain();
		relog::service::logging::Loggerf& l = get_logger();

	protected:
		Pipeline& pl;

	public:
		Parser(Pipeline& pipeline_, relog::pipeline::lex::Lexer& lexer_) : Parser_BisonParser(*this, lexer_), pl(pipeline_) {}

		[[noreturn]] void error(const std::string& message);

		friend Parser_BisonParser;
};

} // namespace relog::pipeline::parse

#endif
