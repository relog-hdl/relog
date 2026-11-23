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

%{
	#pragma GCC diagnostic ignored "-Wswitch-default"
	#pragma GCC diagnostic ignored "-Wunused-function"
	#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"

	#include <pipeline/phase/lex/lexer.hpp>
	#include <pipeline/phase/parse/parser.y.hpp>

	#define yyterminate() return relog::pipeline::parse::Parser::make_END();

	using relog::pipeline::parse::Parser;

	#define BEGINCONTEXT(ctx) {\
		yy_push_state(ctx);\
		l.flow("begin context");\
	}

	#define ENDCONTEXT() {\
		yy_pop_state();\
		l.flow("end context");\
	}
%}

/* disable stdout printouts */
%option nodefault

/* end of stream is handled by inliner, when eof is encountered
   that means lexer's job is finished and yyterminate should be called */
%option noyywrap

%option c++
%option yyclass="Lexer"
%option prefix="Lexer_"

/* whitespace */
WS      [ \t]

/* newline */
NL      (\r\n|\r|\n)

/* whitespace or newline */
WSNL    " "|\r\n|\r|\n|\t

/* token separator */
TS      [^_0-9a-zA-Z]

/* identifier */
ID      [_a-zA-Z]+[_0-9a-zA-Z]*

/* string literal */
SL      "\""([^"\\]|\\.)*\"

/* character literal */
CL      \'([^'\\]|\\.)*\'

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

">=" {
	l.flow("opGtEq");
	return Parser::make_opGtEq();
}

"=<" {
	l.flow("opEqLt");
	return Parser::make_opEqLt();
}

"==" {
	l.flow("opEqEq");
	return Parser::make_opEqEq();
}

"!=" {
	l.flow("opExclEq");
	return Parser::make_opExclEq();
}

"===" {
	l.flow("opEqEqEq");
	return Parser::make_opEqEqEq();
}

"!==" {
	l.flow("opExclEqEq");
	return Parser::make_opExclEqEq();
}

"&&" {
	l.flow("opAmpAmp");
	return Parser::make_opAmpAmp();
}

"||" {
	l.flow("opPipePipe");
	return Parser::make_opPipePipe();
}

"<-" {
	l.flow("opLtDash");
	return Parser::make_opLtDash();
}

"<->" {
	l.flow("opLtDashGt");
	return Parser::make_opLtDashGt();
}

"->" {
	l.flow("opDashGt");
	return Parser::make_opDashGt();
}

"<=" {
	l.flow("opLtEq");
	return Parser::make_opLtEq();
}

"<=>" {
	l.flow("opLtEqGt");
	return Parser::make_opLtEqGt();
}

"=>" {
	l.flow("opEqGt");
	return Parser::make_opEqGt();
}

"~&" {
	l.flow("opTildAmp");
	return Parser::make_opTildAmp();
}

"^|" {
	l.flow("opCaretPipe");
	return Parser::make_opCaretPipe();
}

"~^" {
	l.flow("opTildCaret");
	return Parser::make_opTildCaret();
}

"^~" {
	l.flow("opCaretTild");
	return Parser::make_opCaretTild();
}

\<|\> {
	switch(yytext[0]) {
		case '<':
			l.flow("lt");
			return '<';
		case '>':
			l.flow("gt");
			return '>';
		default:
			l.warning("lt/gt unknown token");
			return yytext[0];
	}
}

"<<" {
	l.flow("opShiftLeft");
	return Parser::make_opShiftLeft();
}

">>" {
	l.flow("opShiftRight");
	return Parser::make_opShiftRight();
}

\^|\&|\||\~|\!|\+|\-|\*|\/|\%|\= {
	l.flow("op: %c", yytext[0]);
	return yytext[0];
}

\?|\: {
	return yytext[0];
}

\(|\)|\{|\} {
	l.flow("brace: %c", yytext[0]);
	return yytext[0];
}

\,|\; {
	l.flow("separator: %c", yytext[0]);
	return yytext[0];
}

{WS}

{NL}

. {
	l.warning("unknown character");
}

<<EOF>> {
	l.flow("eof");
	yyterminate();
}

%%
