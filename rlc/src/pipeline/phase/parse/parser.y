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
	#include <pipeline/phase/lex/lexer.hpp>
	#include <pipeline/phase/parse/parser.y.hpp>
	#include <pipeline/phase/parse/parser.hpp>

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

%type <std::string> type

%start design

%%

design				: module_decl_list
						{
							l.flow("design: module_decl_list");
						}
	   				| %empty
						{
							l.flow("design: %%empty");
						}
	   				;

module_decl_list	: module_decl
						{
							l.flow("module_decl_list: (start)");
						}
					| module_decl_list module_decl
						{
							l.flow("module_decl_list: (continuation)");
						}
					;

module_decl			: module identifier lparen rparen lcurly module_body rcurly
						{
							l.flow("module_decl: (empty port list) %s", $2.c_str());
						}
		   			| module identifier lparen module_port_list rparen lcurly module_body rcurly
						{
							l.flow("module_decl: (non-empty port list) %s", $2.c_str());
						}
		   			;

module_port_list	: module_port
						{
							l.flow("module_port_list: (start)");
						}
					| module_port_list comma module_port
						{
							l.flow("module_port_list: (continuation)");
						}
					;

module_port			: net_direction identifier
						{
							l.flow("module_port: (untyped) %s %s", $1.c_str(), $2.c_str());
						}
		   			| net_direction net_type identifier
						{
							l.flow("module_port: (net type) %s %s %s", $1.c_str(), $2.c_str(), $3.c_str());
						}
					| net_direction type identifier
						{
							l.flow("module_port: (non-net type) %s %s %s", $1.c_str(), $2.c_str(), $3.c_str());
						}
		   			;

module_body			: statement_list
						{
							l.flow("module_body: statement_list");
						}
		   			| %empty
						{
							l.flow("module_body: %%empty");
						}
		   			;

statement_list		: statement
						{
							l.flow("statement_list: (start)");
						}
			  		| statement_list statement
						{
							l.flow("statement_list: (continuation)");
						}
			  		;

statement			: statement_named_ref_net_decl
						{
							l.flow("statement: statement_named_ref_net_decl");
						}
		 			| statement_named_ref_decl
						{
							l.flow("statement: statement_named_ref_decl");
						}
		 			| statement_assignment
						{
							l.flow("statement: statement_assignment");
						}
		 			;

statement_named_ref_net_decl		: net_type statement_named_ref_net_decl_list semicolon
									;

statement_named_ref_net_decl_list	: identifier
								 	| identifier opLtDash expr
								 	| statement_named_ref_net_decl_list comma identifier
								 	| statement_named_ref_net_decl_list comma identifier opLtDash expr
								 	;

statement_named_ref_decl			: type statement_named_ref_decl_list semicolon;

statement_named_ref_decl_list		: identifier
							 		| identifier opEq expr
							 		| statement_named_ref_decl_list comma identifier
							 		| statement_named_ref_decl_list comma identifier opEq expr
							 		;

statement_assignment				: identifier opLtDash expr semicolon
									;

expr			: expr_named_ref
					{
						l.flow("expr: expr_named_ref");
					}
				;

expr_named_ref	: identifier
					{
						l.flow("expr_named_ref: %s", $1.c_str());
					}
				;

type			: identifier
					{
						$$ = $1;
						l.flow("type: %s", $1.c_str());
					}
				;

%%

void Parser_BisonParser::error(const std::string &message) {
	std::cout << "Error: " << message << std::endl;
}
