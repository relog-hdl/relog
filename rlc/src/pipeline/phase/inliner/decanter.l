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

	#include <pipeline/phase/inliner/decanter.hpp>

	#define yyterminate() return DECANTER_END;

	#define DECANTER_WS " "

	#define BEGINCONTEXT(ctx) {\
		yy_push_state(ctx);\
		l.flow("begin context");\
	}

	#define ENDCONTEXT() {\
		yy_pop_state();\
		l.flow("end context");\
	}
%}

/* end of stream is handled by inliner, when eof is encountered
   that means decanter is finished with the provided file stream
   and yyterminate should be called */
%option noyywrap

%option c++
%option yyclass="Decanter"
%option prefix="Decanter_"

/* newline */
NL      (\r\n|\r|\n)

/* string literal, need to recognize because comments don't count
                   inside of string literals                      */
SL      "\""([^"\\]|\\.)*\"

%x ctxMultilineComment

%%

{SL} {
	yyout << yytext;
}

<*>"//"[^\n]* {
	if(YY_START == ctxMultilineComment)
		REJECT;
	yyout << DECANTER_WS;
	l.flow("comment: %s", yytext);
}

<*>"/*" {
	if(YY_START == ctxMultilineComment)
		REJECT;
	l.flow("begin multiline comment");
	BEGINCONTEXT(ctxMultilineComment);
}

<ctxMultilineComment>"*/" {
	l.flow("end multiline comment");
	yyout << DECANTER_WS;
	ENDCONTEXT();
}

<ctxMultilineComment>[^*/\n]* {
	l.flow("multi-line comment: %s", yytext);
}

<ctxMultilineComment>\*

<ctxMultilineComment>\/

<ctxMultilineComment>{NL}

\\{NL}

<<EOF>> {
	l.flow("eof");
	yyterminate();
}

%%
