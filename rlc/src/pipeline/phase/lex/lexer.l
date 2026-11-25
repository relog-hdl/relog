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

/* unbased number */
UN      [0-9]+[0-9_]*

/* based number */
BN      ([1-9]+[0-9_]*)?[ \t]*'[bBoOdDhH][ \t]*[0-9a-fA-FxXzZ?_]+

/* real number */
RN      [0-9]+[0-9_]*(\.[0-9][0-9_]*)?([eE][+-]?[0-9_]+)?

/* string literal */
SL      "\""([^"\\]|\\.)*\"

%%

if/{TS} {
	return Parser::make_if();
}

else/{TS} {
	return Parser::make_else();
}

switch/{TS} {
	return Parser::make_switch();
}

zswitch/{TS} {
	return Parser::make_zswitch();
}

xswitch/{TS} {
	return Parser::make_xswitch();
}

case/{TS} {
	return Parser::make_case();
}

default/{TS} {
	return Parser::make_default();
}

while/{TS} {
	return Parser::make_while();
}

do/{TS} {
	return Parser::make_do();
}

for/{TS} {
	return Parser::make_for();
}

generate/{TS} {
	return Parser::make_generate();
}

repeat/{TS} {
	return Parser::make_repeat();
}

init/{TS} {
	return Parser::make_init();
}

rise/{TS} {
	return Parser::make_rise();
}

fall/{TS} {
	return Parser::make_fall();
}

edge/{TS} {
	return Parser::make_edge();
}

wire|wand|wor|tri|triand|trior|tri0|tri1|supply0|supply1/{TS} {
	l.flow("net: %s", yytext);
	return Parser::make_net(yytext);
}

reg/{TS} {
	l.flow("reg");
	return Parser::make_reg();
}

trireg/{TS} {
	l.flow("trireg");
	return Parser::make_trireg();
}

input/{TS} {
	return Parser::make_input();
}

output/{TS} {
	return Parser::make_output();
}

inout/{TS} {
	return Parser::make_inout();
}

module/{TS} {
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

"<<" {
	l.flow("opLtLt");
	return Parser::make_opLtLt();
}

">>" {
	l.flow("opGtGt");
	return Parser::make_opGtGt();
}

"<<<" {
	l.flow("opLtLtLt");
	return Parser::make_opLtLtLt();
}

">>>" {
	l.flow("opGtGtGt");
	return Parser::make_opGtGtGt();
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

\^|\&|\||\~|\!|\+|\-|\*|\/|\%|\= {
	l.flow("op: %c", yytext[0]);
	return yytext[0];
}

\?|\:|\@ {
	//l.flow("control flow: %c", yytext[0]);
	return yytext[0];
}

\[|\]|\(|\)|\{|\} {
	//l.flow("brace: %c", yytext[0]);
	return yytext[0];
}

\.|\,|\; {
	//l.flow("separator: %c", yytext[0]);
	return yytext[0];
}

{ID} {
	l.flow("identifier: %s", yytext);
	return Parser::make_identifier(yytext);
}

{UN} {
	l.flow("unum: %s", yytext);
	return Parser::make_unsigned_number(yytext);
}

{BN} {
	l.flow("bnum: %s", yytext);
	return Parser::make_based_number(yytext);
}

{RN} {
	l.flow("rnum: %s", yytext);
	return Parser::make_real_number(yytext);
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
