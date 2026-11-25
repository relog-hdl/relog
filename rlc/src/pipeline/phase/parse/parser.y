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

	namespace relog::pipeline::lex
	{
		class Lexer;
	}

	namespace relog::pipeline::parse
	{
		class Parser;
	}
}

%code top
{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wshadow"

	#include <pipeline/phase/lex/lexer.hpp>
	#include <pipeline/phase/parse/parser.y.hpp>
	#include <pipeline/phase/parse/parser.hpp>

	using relog::pipeline::lex::Lexer;
	using namespace relog::pipeline::parse;

	static Parser::symbol_type yylex(Lexer &lexer)
	{
		return lexer.get_next_token();
	}
}

%lex-param { relog::pipeline::lex::Lexer &lexer }
%parse-param { relog::pipeline::parse::Parser &self }
%parse-param { relog::pipeline::lex::Lexer &lexer }
%parse-param { relog::service::logging::Loggerf &l }

%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "eof"

%token module;

%token input;
%token output;
%token inout;

%token if;
%token else;
%token while;
%token do;
%token for;

%token init;
%token rise;
%token fall;
%token edge;

%token <std::string> identifier;
%token <std::string> net;
%token reg;
%token trireg;

%token <std::string> unsigned_number;
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


%token '?' ':' '@';
%token '[' ']' '(' ')' '{' '}';
%token ',' ';';

%nonassoc IF_STMT
%nonassoc else

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
//%type <std::string> type

%start description_text

%%

description_text			: top_level_statement_list
								{
									l.flow("description_text: top_level_statement_list");
								}
							| %empty
								{
									l.flow("description_text: %%empty");
								}
							;

top_level_statement_list	: top_level_statement
								{
									l.flow("top_level_statement_list: (start)");
								}
							| top_level_statement_list top_level_statement
								{
									l.flow("top_level_statement_list: (continuation)");
								}
							;

top_level_statement			: module_defn
								{
									l.flow("top_level_statement: module_defn");
								}
							| module_decl
								{
									l.flow("top_level_statement: module_decl");
								}
							| statement_decl
								{
									l.flow("top_level_statement: statement_decl");
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

module_port			: net_direction range_opt identifier
						{
							l.flow("module_port: (untyped) %s %s", $1.c_str(), $3.c_str());
						}
		   			| net_direction range_opt net identifier
						{
							l.flow("module_port: (net type) %s %s %s", $1.c_str(), $3.c_str(), $4.c_str());
						}
					| net_direction range_opt reg identifier
						{
							l.flow("module_port: (reg type) %s %s", $1.c_str(), $4.c_str());
						}
		   			;

module_body			: statement_list_opt
		   			;

statement_list_opt	: statement_list
						{
							l.flow("statement_list_opt: statement_list");
						}
					| %empty
						{
							l.flow("statement_list_opt: %%empty");
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

statement_loop_cond	: statement
					| '{' statement_list_opt '}'
					;

statement			: statement_empty
						{
							l.flow("statement: statement_empty");
						}
					| statement_decl
						{
							l.flow("statement: statement_decl");
						}
		 			| statement_assignment
						{
							l.flow("statement: statement_assignment");
						}
					| statement_if_cond
						{
							l.flow("statement: statement_if_cond");
						}
					| statement_while_loop
						{
							l.flow("statement: statement_while_loop");
						}
					| statement_do_while_loop
						{
							l.flow("statement: statement_do_while_loop");
						}
					| statement_for_loop
						{
							l.flow("statement: statement_for_loop");
						}
					| statement_at_init
						{
							l.flow("statement: statement_at_init");
						}
					| statement_at_event
						{
							l.flow("statement: statement_at_event");
						}
					| statement_at_every
						{
							l.flow("statement: statement_at_every");
						}
		 			;

statement_if_cond		: if '(' expr ')' statement_loop_cond %prec IF_STMT
						| if '(' expr ')' statement_loop_cond else statement_loop_cond
						;

statement_while_loop	: while '(' expr ')' statement_loop_cond
						;

statement_do_while_loop	: do statement_loop_cond while '(' expr ')' ';'
						;

statement_for_loop		: for '(' assignment_blocking ';' expr ';' assignment_blocking ')' statement_loop_cond
						;

statement_at_init		: '@' init '{' statement_list_opt '}'
						;

statement_at_event		: at_event_specifier_list ';'

statement_at_every		: at_event_specifier_list '{' statement_list_opt '}'

at_event_specifier_list		: at_event_specifier
							| at_event_specifier_list ',' at_event_specifier
							;

at_event_specifier			: '@' at_event_edge at_event_signal
							| '@' at_event_signal
							;

at_event_edge				: rise
							| fall
							;

at_event_signal				: expr
							;

statement_decl						: decl ';'
									;

statement_assignment				: assignment ';'
									;

/*
statement_assignment_transfer		: assignment_transfer ';'
									;

statement_assignment_continous		: assignment_continous ';'
									;

statement_assignment_blocking		: assignment_blocking ';'
									;

statement_assignment_nonblocking	: assignment_nonblocking ';'
									;
*/

statement_empty						: ';'
									;

decl								: net_decl
									| reg_decl
									;

net_decl							: net range_opt net_decl_list
									;

net_decl_list						: identifier
								 	| identifier "<-" expr
								 	| net_decl_list ',' identifier
								 	| net_decl_list ',' identifier "<-" expr
								 	;

reg_decl							: reg range_opt reg_decl_list
									;

reg_decl_list						: identifier
							 		| identifier '=' expr
							 		| reg_decl_list ',' identifier
							 		| reg_decl_list ',' identifier '=' expr
							 		;

assignment							: assignment_continous
									| assignment_blocking
									| assignment_nonblocking
									;

/*
assignment_transfer					: assignment_blocking
									| assignment_nonblocking
									;
*/

assignment_continous				: identifier "<-" expr
									;

assignment_blocking					: identifier '=' expr
									;

assignment_nonblocking				: identifier "<=" expr
									;

expr			: base
					{
						l.flow("expr: base");
					}
				| '(' expr ')'
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

number			: unsigned_number
				| number_base unsigned_number
				| unsigned_number number_base unsigned_number
				;

/*
type			: identifier
					{
						$$ = $1;
						l.flow("type: %s", $1.c_str());
					}
				;
*/

range_opt		: range
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

void Parser_BisonParser::error(const std::string &message)
{
	std::cout << "Error: " << message << std::endl;
}

#pragma GCC diagnostic pop
