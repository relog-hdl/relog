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
%token switch;
%token zswitch;
%token xswitch;
%token case;
%token default;

%token while;
%token do;
%token for;

%token generate;
%token repeat;

%token init;
%token rise;
%token fall;
%token edge;

%token <std::string> identifier;
%token <std::string> net;
%token reg;
%token trireg;

%token <std::string> edge_descriptor;
%token <std::string> unsigned_number;
%token <std::string> based_number;
%token <std::string> real_number;
%token <std::string> string;


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
%token '.' ',' ';';

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
%type <std::string> module_name
%type <std::string> name
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

module_defn			: module name '(' ')' '{' module_body '}' ';'
						{
							l.flow("module_defn: (empty port list) %s", $2.c_str());
						}
		   			| module name '(' module_port_list ')' '{' module_body '}' ';'
						{
							l.flow("module_defn: (non-empty port list) %s", $2.c_str());
						}
		   			;

module_decl			: module name '(' ')' ';'
						{
							l.flow("module_decl: (empty port list) %s", $2.c_str());
						}
					| module name '(' module_port_list ')' ';'
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

module_port			: net_direction range_opt name
						{
							l.flow("module_port: (untyped) %s %s", $1.c_str(), $3.c_str());
						}
		   			| net_direction range_opt net name
						{
							l.flow("module_port: (net type) %s %s %s", $1.c_str(), $3.c_str(), $4.c_str());
						}
					| net_direction range_opt reg name
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
					| statement_switch_cond
						{
							l.flow("statement: statement_switch_cond");
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
					| statement_module_inst
						{
							l.flow("statement: statement_module_inst");
						}
					| statement_repeat
						{
							l.flow("statement: statement_repeat");
						}
					| statement_generate
						{
							l.flow("statement: statement_generate");
						}
		 			;

statement_if_cond		: if_head statement_loop_cond %prec IF_STMT
						| if_head statement_loop_cond else statement_loop_cond
						;

if_head					: if '(' expr ')'
						;

statement_switch_cond	: switch_head '{' switch_cond_case_list '}'

switch_head				: switch_type_keyword '(' expr ')'
						;

switch_type_keyword		: switch
						| zswitch
						| xswitch
						;

switch_cond_case_list	: switch_cond_case
						| switch_cond_case_list switch_cond_case
						;

switch_cond_case		: case_cond ':' statement_list_opt
						;

case_cond				: case case_cond_expr_list
						| default
						;

case_cond_expr_list		: expr
						| case_cond_expr_list ',' expr
						;

statement_while_loop	: while_head statement_loop_cond
						;

statement_do_while_loop	: do statement_loop_cond while_head ';'
						;

while_head				: while '(' expr ')'
						;

statement_for_loop		: for_head statement_loop_cond
						;

for_head				: for '(' assignment_blocking ';' expr ';' assignment_blocking ')'
						;

statement_at_init		: at_init_head '{' statement_list_opt '}'
						;

at_init_head			: '@' init
						;

statement_at_event		: at_event_specifier_list_or_auto ';'

statement_at_every		: at_event_specifier_list_or_auto '{' statement_list_opt '}'

at_event_specifier_list_or_auto	: at_event_specifier_list
								| '@' '*'
								;

at_event_specifier_list			: at_event_specifier
								| at_event_specifier_list ',' at_event_specifier
								;

at_event_specifier				: '@' at_event_edge at_event_signal
								| '@' at_event_signal
								;

at_event_edge					: rise
								| fall
								| edge edge_descriptor
								;

at_event_signal					: expr
								;

statement_repeat			: repeat_head statement_loop_cond

repeat_head					: repeat '(' expr ')'

statement_generate			: generate_head generate_trailer

generate_head				: generate '(' generate_variable_name ')'

generate_variable_name		: identifier
							;

generate_trailer			: statement_if_cond
							| statement_for_loop
							| statement_switch_cond
							;

statement_module_inst		: module_name name '{' '}' ';'
							| module_name name '{' module_port_connection_list '}' ';'
							| module_name name '(' ')' ';'
							| module_name name '(' module_port_connection_list ')' ';'
							;

module_port_connection		: '.' name '(' expr ')'
							;

module_port_connection_list	: module_port_connection
							| module_port_connection_list ',' module_port_connection
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

net_decl_list						: name
								 	| name "<-" expr
								 	| net_decl_list ',' name
								 	| net_decl_list ',' name "<-" expr
								 	;

reg_decl							: reg range_opt reg_decl_list
									;

reg_decl_list						: name
							 		| name '=' expr
							 		| reg_decl_list ',' name
							 		| reg_decl_list ',' name '=' expr
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
									| identifier "<=" expr at_event_specifier_list_or_auto
									| identifier "<=" expr repeat_head at_event_specifier_list_or_auto
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

base			: name
					{
						l.flow("base: %s", $1.c_str());
					}
				| number
				| string
				;

number			: unsigned_number
				| based_number
				| real_number
				;

module_name		: identifier
					{
						$$ = $1;
						l.flow("module_name: %s", $1.c_str());
					}
				;

name			: identifier
					{
						$$ = $1;
						l.flow("name: %s", $1.c_str());
					}
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
