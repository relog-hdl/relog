/*
 * Cpp structure & integration based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

#ifndef RELOG_PIPELINE_PHASE_LEX_LEXER_HPP
#define RELOG_PIPELINE_PHASE_LEX_LEXER_HPP

#include <pipeline/pipeline.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

// clang-format off
#include <pipeline/phase/parse/parser.y.hpp> // this is needed for symbol_type
#include <pipeline/phase/parse/parser.hpp>

#include <iostream>

/**
 * Generated Flex class name is yyFlexLexer by default. If we want to use more flex-generated
 * classes we should name them differently. See lexer.l prefix option.
 *
 * Unfortunately the implementation relies on this trick with redefining class name
 * with a preprocessor macro. See GNU Flex manual, "Generating C++ Scanners" section
 */
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer Lexer_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

// Scanner method signature is defined by this macro. Original yylex() returns int.
// Sinice Bison 3 uses symbol_type, we must change returned type. We also rename it
// to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL relog::pipeline::parse::Parser::symbol_type relog::pipeline::lex::Lexer::get_next_token()
// clang-format on

namespace relog::pipeline::lex
{

class Lexer final : public yyFlexLexer
{
	public:
		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = relog::pipeline::Pipeline::get_domain() / "lex";
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
		std::istream& is;

	public:
		Lexer(relog::pipeline::Pipeline& pipeline, std::istream& in);

		virtual relog::pipeline::parse::Parser::symbol_type get_next_token();

		[[noreturn]] void LexerError(const char* msg);
};

} // namespace relog::pipeline::lex

#endif
