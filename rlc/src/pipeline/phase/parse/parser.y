/*
 * Cpp structure & integration based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 *
 * Rules based on (in large but limited part) cppparser
 * used under The MIT License (MIT)
 * Copyright (c) 2022 Satya Das <SatyaRanjan.Das@tomtom.com>
 * (https://github.com/satya-das/cppparser)
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
%token <std::string> net;
%token reg;
%token trireg;

%token input;
%token output;
%token inout;

%token module;

%token <std::string> unsigned_number;
%token <std::string> signed_number;
%token <char> number_base;


%token opGtEq ">=";
%token opEqLt "=<";
%token opEqEq "==";
%token opExclEq "!=";
%token opEqEqEq "===";
%token opExclEqEq "!==";

%token opAmpAmp "&&";
%token opPipePipe "||";

%token opLtDash "<-";
%token opLtDashGt "<->";
%token opDashGt "->";

%token opLtEq "<=";
%token opLtEqGt "<=>";
%token opEqGt "=>";

%token lt "<";
%token gt ">";

%token opLtLt "<<";
%token opGtGt ">>";
%token opLtLtLt "<<<";
%token opGtGtGt ">>>";

%token opTildAmp "~&";
%token opCaretPipe "^|";
%token opTildCaret "~^";
%token opCaretTild "^~";

%token '^' '&' '|' '~' '!' '+' '-' '*' '/' '%' '=';


%token '?' ':';
%token '[' ']' '(' ')' '{' '}';
%token ',' ';';

%right '?' TERNARYCOND
%left "||"
%left "&&"
%left '|'
%left '^' "^~"
%left '&'

%left "==" "!=" "===" "!=="
%left '<' '>' "=<" ">="
%left "<<" ">>" "<<<" ">>>"
%left '+' '-'
%left '*' '/' '%'
%right UNARYPLUS UNARYMINUS '!' '~' UNARYAMP "~&" UNARYPIPE "^|" UNARYCARET "~^"

%type <std::string> net_direction
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
					| statement_named_ref_reg_decl
						{
							l.flow("design_statement: statement_named_ref_reg_decl");
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

module_port			: net_direction range_optional identifier
						{
							l.flow("module_port: (untyped) %s %s", $1.c_str(), $3.c_str());
						}
		   			| net_direction range_optional net identifier
						{
							l.flow("module_port: (net type) %s %s %s", $1.c_str(), $3.c_str(), $4.c_str());
						}
					| net_direction range_optional reg identifier
						{
							l.flow("module_port: (reg type) %s %s", $1.c_str(), $4.c_str());
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
		 			| statement_named_ref_reg_decl
						{
							l.flow("statement: statement_named_ref_decl");
						}
		 			| statement_assignment
						{
							l.flow("statement: statement_assignment");
						}
		 			;

statement_named_ref_net_decl		: net range_optional statement_named_ref_net_decl_list ';'
									;

statement_named_ref_net_decl_list	: identifier
								 	| identifier "<-" expr
								 	| statement_named_ref_net_decl_list ',' identifier
								 	| statement_named_ref_net_decl_list ',' identifier "<-" expr
								 	;

statement_named_ref_reg_decl		: reg range_optional statement_named_ref_reg_decl_list ';'
									;

statement_named_ref_reg_decl_list	: identifier
							 		| identifier '=' expr
							 		| statement_named_ref_reg_decl_list ',' identifier
							 		| statement_named_ref_reg_decl_list ',' identifier '=' expr
							 		;

statement_assignment				: identifier "<-" expr ';'
									;

expr			: base
					{
						l.flow("expr: base");
					}
				| '+' base %prec UNARYPLUS
				| '-' base %prec UNARYMINUS
				| '!' base
				| '~' base
				| '&' base %prec UNARYAMP
				| "~&" base
				| '|' base %prec UNARYPIPE
				| "^|" base
				| '^' base %prec UNARYCARET
				| "~^" base
				| expr '+' expr
				| expr '-' expr
				| expr '*' expr
				| expr '/' expr
				| expr '%' expr
				| expr "==" expr
				| expr "!=" expr
				| expr "===" expr
				| expr "!==" expr
				| expr "&&" expr
				| expr "||" expr
				| expr '<' expr
				| expr "=<" expr
				| expr '>' expr
				| expr ">=" expr
				| expr '&' expr
				| expr '|' expr
				| expr '^' expr
				| expr "^~" expr
				| expr ">>" expr
				| expr "<<" expr
				| expr ">>>" expr
				| expr "<<<" expr
				| expr '?' expr ':' expr %prec TERNARYCOND
					{
						l.flow("expr: (ternary)");
					}
				;

base			: identifier
					{
						l.flow("base: %s", $1.c_str());
					}
				| const_base
				;

const_base		: number
				;

number			: signed_number
				| unsigned_number
				| number_base unsigned_number
				| unsigned_number number_base unsigned_number
				;

type			: identifier
					{
						$$ = $1;
						l.flow("type: %s", $1.c_str());
					}
				;

range_optional	: range
				| %empty
				;

range			: '[' expr ':' expr ']'
				;

net_direction	: input
					{
						$$ = "input";
						l.flow("net_direction: %s", "input");
					}
				| output
					{
						$$ = "output";
						l.flow("net_direction: %s", "output");
					}
				| inout
					{
						$$ = "inout";
						l.flow("net_direction: %s", "inout");
					}
				;

%%

void Parser_BisonParser::error(const std::string &message) {
	std::cout << "Error: " << message << std::endl;
}
