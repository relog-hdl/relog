/*
 * Structure based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

%require "3.2"
%skeleton "lalr1.cc"
%defines
%define api.parser.class { Parser }
%define api.namespace { relog::pipeline::parse }

%define api.token.constructor
%define api.value.type variant

%code requires
{
	#include <stdio.h>
	#include <stdlib.h>

	namespace relog::pipeline::lex {
		class Lexer;
	}

	//extern int lexer_lex(void);

	//int parser_lex(void);

	//void parser_error(const char *s);
}

%code top
{
	#include <pipeline/phase/lex/lexer.h>
	#include <pipeline/phase/parse/parser.y.hpp>

	static relog::pipeline::parse::Parser::symbol_type yylex(relog::pipeline::lex::Lexer &lexer) {
        return lexer.get_next_token();
    }

	using namespace relog::pipeline::parse;
}

%lex-param { relog::pipeline::lex::Lexer &lexer }
%parse-param { relog::pipeline::lex::Lexer &lexer }

%define parse.trace
%define parse.error verbose

%token END 0 "eof"

%%

placeholder:;

%%

void relog::pipeline::parse::Parser::error(const std::string &message) {
	std::cout << "Error: " << message << std::endl;
}
