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

	#include <pipeline/phase/lex/lexer.hpp>
	#include <pipeline/phase/parse/parser.y.hpp>

	#define yyterminate() return relog::pipeline::parse::Parser::make_END();

	using relog::pipeline::parse::Parser;
%}

/* disable stdout printouts */
%option nodefault

/* disable yywrap while lexer integration is non-existent */
%option noyywrap

%option c++
%option yyclass="Lexer"
%option prefix="Lexer_"

WS      [ \t]

NL      (\r\n|\r|\n)

WSNL    " "|\r\n|\r|\n|\t

TS      [^_0-9a-zA-Z]

ID      [_a-zA-Z]+[_0-9a-zA-Z]*

%%

{ID} {
	l.flow("identifier: %s", yytext);
	return Parser::make_identifier(yytext);
}

wire|reg|logic|tri/{TS} {
	l.flow("net_type: %s", yytext);
	return Parser::make_net_type(yytext);
}

input|output|inout/{TS} {
	l.flow("net_direction: %s", yytext);
	return Parser::make_net_direction(yytext);
}

module/{TS} {
	l.flow("module");
	return Parser::make_module();
}

"<-" {
	l.flow("opLtDash");
	return Parser::make_opLtDash();
}

"=" {
	l.flow("opEq");
	return Parser::make_opEq();
}

"(" {
	l.flow("lparen");
	return Parser::make_lparen();
}

")" {
	l.flow("rparen");
	return Parser::make_rparen();
}

"{" {
	l.flow("lcurly");
	return Parser::make_lcurly();
}

"}" {
	l.flow("rcurly");
	return Parser::make_rcurly();
}

"," {
	l.flow("comma");
	return Parser::make_comma();
}

";" {
	l.flow("semicolon");
	return Parser::make_semicolon();
}

{WSNL} {}

. {
	l.warning("unknown character");
}

<<EOF>> {
	l.flow("eof");
	yyterminate();
}

%%
