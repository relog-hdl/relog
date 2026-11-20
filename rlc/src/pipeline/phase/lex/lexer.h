/*
 * Structure based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

#ifndef RELOG_PIPELINE_PHASE_LEX_LEXER_H
#define RELOG_PIPELINE_PHASE_LEX_LEXER_H

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

#include <pipeline/phase/parse/parser.y.hpp> // this is needed for symbol_type

namespace relog::pipeline::lex {
	
class Lexer : public yyFlexLexer {
	public:
		Lexer() {}
		
		virtual ~Lexer() {}

		virtual relog::pipeline::parse::Parser::symbol_type get_next_token();

		void LexerError(const char* msg) {
			std::cerr << msg << std::endl;
			exit(2);
		} // TODO override
};

}

#endif
