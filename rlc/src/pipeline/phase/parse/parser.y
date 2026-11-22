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

%token opLtDash opLtDashGt opDashGt;
%token '&' '|' '~' '!' '+' '-' '=';
%token '(' ')' '{' '}';

%token ',' ';';

%type <std::string> type

%start design

%%

design				: design_statement_list
						{
							l.flow("design: design_statement_list");
						}
	   				| %empty
						{
							l.flow("design: %%empty");
						}
	   				;

design_statement_list	: design_statement
							{
								l.flow("design_statement_list: (start)");
							}
						| design_statement_list design_statement
							{
								l.flow("design_statement_list: (continuation)");
							}
						;

design_statement	: module_defn
						{
							l.flow("design_statement: module_defn");
						}
					| module_decl
						{
							l.flow("design_statement: module_decl");
						}
					| statement_named_ref_net_decl
						{
							l.flow("design_statement: statement_named_ref_net_decl");
						}
					| statement_named_ref_decl
						{
							l.flow("design_statement: statement_named_ref_decl");
						}
					;

module_defn			: module identifier '(' ')' '{' module_body '}' ';'
						{
							l.flow("module_defn: (empty port list) %s", $2.c_str());
						}
		   			| module identifier '(' module_port_list ')' '{' module_body '}' ';'
						{
							l.flow("module_defn: (non-empty port list) %s", $2.c_str());
						}
		   			;

module_decl			: module identifier '(' ')' ';'
						{
							l.flow("module_decl: (empty port list) %s", $2.c_str());
						}
					| module identifier '(' module_port_list ')' ';'
						{
							l.flow("module_decl: (non-empty port list) %s", $2.c_str());
						}
					;

module_port_list	: module_port
						{
							l.flow("module_port_list: (start)");
						}
					| module_port_list ',' module_port
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

statement_named_ref_net_decl		: net_type statement_named_ref_net_decl_list ';'
									;

statement_named_ref_net_decl_list	: identifier
								 	| identifier opLtDash expr
								 	| statement_named_ref_net_decl_list ',' identifier
								 	| statement_named_ref_net_decl_list ',' identifier opLtDash expr
								 	;

statement_named_ref_decl			: type statement_named_ref_decl_list ';'
									;

statement_named_ref_decl_list		: identifier
							 		| identifier '=' expr
							 		| statement_named_ref_decl_list ',' identifier
							 		| statement_named_ref_decl_list ',' identifier '=' expr
							 		;

statement_assignment				: identifier opLtDash expr ';'
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
