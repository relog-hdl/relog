/*
 * Structure based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

%require "3.2"
%skeleton "lalr1.cc"
%defines
%define parse.assert

%define api.parser.class { Parser_BisonParser }
%define api.namespace { relog::pipeline::parse }

%define api.token.constructor
%define api.value.type variant

%code requires
{
	#include <service/logging/loggerf.hpp>

	#include <stdio.h>
	#include <stdlib.h>

	namespace relog::pipeline::lex {
		class Lexer;
	}

	namespace relog::pipeline::parse {
		class Parser;
	}
}

%code top
{
	#include <pipeline/phase/lex/lexer.h>
	#include <pipeline/phase/parse/parser.y.hpp>
	#include <pipeline/phase/parse/parser.h>

	using relog::pipeline::lex::Lexer;
	using namespace relog::pipeline::parse;

	static Parser::symbol_type yylex(Lexer &lexer) {
        return lexer.get_next_token();
    }
}

%lex-param { relog::pipeline::lex::Lexer &lexer }
%parse-param { relog::pipeline::parse::Parser &self }
%parse-param { relog::pipeline::lex::Lexer &lexer }

/* An extremely dirty hack to get logger injected into token handlers */

%{
#define CLOSE_SCOPE }
#define OPEN_SCOPE  {
%}

%initial-action {
	CLOSE_SCOPE
	relog::service::logging::Loggerf& l = self.l;
	OPEN_SCOPE
}

%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "eof"

%token <std::string> identifier;
%token <std::string> net_type;
%token <std::string> net_direction;

%token module;

%token opLtDash opEq;
%token lparen rparen lcurly rcurly;

%token comma semicolon;

%start program

%%

program: module_decl_list
       | %empty
	   ;

module_decl_list: module_decl
					{
						l.flow("module_decl_list start");
					}
                | module_decl_list module_decl
					{
						l.flow("module_decl_list continuation");
					}
				;

module_decl: module identifier lparen rparen lcurly module_body rcurly
           | module identifier lparen module_port_list rparen lcurly module_body rcurly
		   ;

module_port_list: module_port
                | module_port_list comma module_port
				;

module_port: net_direction identifier
           | net_direction net_type identifier
           | net_direction type identifier
		   ;

module_body: statement_list
           | %empty
		   ;

statement_list: statement
              | statement_list statement
			  ;

statement: statement_named_ref_net_decl
         | statement_named_ref_decl
		 | statement_assignment
		 ;

statement_named_ref_net_decl: net_type statement_named_ref_net_decl_list semicolon;

statement_named_ref_net_decl_list: identifier
                                 | identifier opLtDash expr
                                 | statement_named_ref_net_decl_list comma identifier
								 | statement_named_ref_net_decl_list comma identifier opLtDash expr
                                 ;

statement_named_ref_decl: type statement_named_ref_decl_list semicolon;

statement_named_ref_decl_list: identifier
                             | identifier opEq expr
                             | statement_named_ref_decl_list comma identifier
							 | statement_named_ref_decl_list comma identifier opEq expr
							 ;

statement_assignment: identifier opLtDash expr semicolon;

expr: expr_named_ref;

expr_named_ref: identifier;

type: identifier;

%%

void Parser_BisonParser::error(const std::string &message) {
	std::cout << "Error: " << message << std::endl;
}
