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

	#include <pipeline/phase/inliner/inliner.hpp>

	#define yyterminate() return INLINER_END;

	#define BEGINCONTEXT(ctx) {\
		yy_push_state(ctx);\
		l.flow("begin context");\
	}

	#define ENDCONTEXT() {\
		yy_pop_state();\
		l.flow("end context");\
	}
%}

%option c++
%option yyclass="Inliner"
%option prefix="Inliner_"

/* whitespace */
WS      [ \t]

/* newline */
NL      (\r\n|\r|\n)

/* token separator */
TS      [^_0-9a-zA-Z]

/* string literal */
SL      \"([^"\\]|\\.)*\"

%x ctxPreprocessor

%x ctxInclude

%%

{SL} {
	// ignore preprocessor directives inside of string literals
	yyout << yytext;
}

^{WS}*` {
	l.flow("begin preprocessor directive");
	BEGINCONTEXT(ctxPreprocessor);
}

<ctxPreprocessor>{NL} {
	l.flow("end preprocessor directive");
	ENDCONTEXT();
}

<ctxPreprocessor>include/{TS} {
	BEGINCONTEXT(ctxInclude);
}

<ctxInclude>{WS}

<ctxInclude>\"([^\"\r\n]|\\\")*\"{WS}*{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxPreprocessor context
	handleInclude(yytext);
}

<ctxInclude>\<([^\<\>\r\n]|\\\<|\\\>)*\>{WS}*{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxPreprocessor context
	handleInclude(yytext);
}

<<EOF>> {
	l.flow("eof");
	yyterminate();
}

%%

int Inliner_FlexLexer::yywrap() {
	return 1;
}
