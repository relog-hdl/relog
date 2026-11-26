/*
 * Cpp structure & integration based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

#ifndef RELOG_PIPELINE_INLINER_INLINER_HPP
#define RELOG_PIPELINE_INLINER_INLINER_HPP

#include <pipeline/phase/inliner/decanter.hpp>
#include <pipeline/pipeline.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

#include <iostream>
#include <sstream>

// clang-format off
/**
 * Generated Flex class name is yyFlexLexer by default. If we want to use more flex-generated
 * classes we should name them differently. See lexer.l prefix option.
 *
 * Unfortunately the implementation relies on this trick with redefining class name
 * with a preprocessor macro. See GNU Flex manual, "Generating C++ Scanners" section
 */
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer Inliner_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL int relog::pipeline::inliner::Inliner::get_next_token()
// clang-format on

#define INLINER_END 0

namespace relog::pipeline::inliner
{

class Inliner final : public yyFlexLexer
{
	public:
		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = relog::pipeline::Pipeline::get_domain() / "inliner";
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
		relog::pipeline::Pipeline& pl;
		relog::pipeline::inliner::Decanter& dctr;
		std::ostream& os;

	public:
		Inliner(relog::pipeline::Pipeline& pipeline, relog::pipeline::inliner::Decanter& decanter, std::ostream& out);

		virtual int get_next_token();

		void run();

		int yywrap();

		[[noreturn]] void LexerError(const char* msg);

	protected:
		void runDecanter();

		void handleInclude(char* inc_str);

		void include(bool external, std::string& path);
};

} // namespace relog::pipeline::inliner

#endif
