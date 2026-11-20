/*
 * Structure based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

%{
	#pragma GCC diagnostic ignored "-Wswitch-default"
	#pragma GCC diagnostic ignored "-Wunused-function"
	#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"

	#include <pipeline/phase/lex/lexer.h>
	#include <pipeline/phase/parse/parser.y.hpp>

	#define yyterminate() return relog::pipeline::parse::Parser::make_END();
%}

/* disable stdout printouts */
%option nodefault

/* disable yywrap while lexer integration is non-existent */
%option noyywrap

%option c++
%option yyclass="Lexer"
%option prefix="Lexer_"

%%

%%
