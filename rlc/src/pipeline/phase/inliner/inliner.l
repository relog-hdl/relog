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

/* identifier */
ID      [_a-zA-Z]+[_0-9a-zA-Z]*

/* string literal */
SL      \"([^"\\]|\\.)*\"

%x ctxMacroArgumentsPre

%x ctxMacroArguments

%x ctxMacroArgumentsBracedInside

%x ctxPreprocessor

%x ctxPreprocessorUnrecognized

%x ctxInclude

%x ctxDefine

%x ctxDefineObjectLikeMacro

%x ctxDefineFunctionLikeMacro

%x ctxDefineFunctionLikeMacroArgs

%x ctxDefineFunctionLikeMacroReplacement

%x ctxUndef

%%

{SL} {
	// ignore preprocessor directives inside of string literals
	yyout << yytext;
}

^{WS}*` {
	l.flow("begin preprocessor directive");
	BEGINCONTEXT(ctxPreprocessor);
}

{ID}/{TS} {
	std::string id{yytext};
	if(object_like_macros.count(id) > 0)
	{
		expand_macro = std::make_unique<ExpandMacroStatus>();
		expand_macro->name = id;
		handleExpandObjectLike();
	}
	else if(function_like_macros.count(id) > 0)
	{
		expand_macro = std::make_unique<ExpandMacroStatus>();
		expand_macro->name = id;
		expand_macro->arg_root = std::make_unique<ExpandMacroArgCall>(id);
		l.flow("collecting arguments for function-like macro");
		BEGINCONTEXT(ctxMacroArgumentsPre);
	}
	else
		yyout << yytext;
}

<ctxMacroArgumentsPre>\({WS}*\)

<ctxMacroArgumentsPre>\(/{WS}*[^\,\)] {
	BEGINCONTEXT(ctxMacroArguments);
}

<ctxMacroArgumentsPre>\(/{WS}*\, {
	BEGINCONTEXT(ctxMacroArguments);
	expand_macro->push_arg_plain("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>{WS}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>[^ \t\(\)\,\r\n]+/{WS}*\( {
	expand_macro->push_arg_call(yytext);
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>[^ \t\(\)\,\r\n]+/{WS}*[\)\,] {
	expand_macro->push_arg_plain(yytext);
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\({WS}*/\, {
	BEGINCONTEXT(ctxMacroArgumentsBracedInside);
	expand_macro->push_arg_plain("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\({WS}*/\( {
	BEGINCONTEXT(ctxMacroArgumentsBracedInside);
	expand_macro->push_arg_call("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\({WS}*/\) {
	BEGINCONTEXT(ctxMacroArgumentsBracedInside);
	expand_macro->push_arg_plain("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\({WS}* {
	BEGINCONTEXT(ctxMacroArgumentsBracedInside);
}

<ctxMacroArgumentsBracedInside>\) {
	ENDCONTEXT(); // pops either to ctxMacroArgumentsBracedInside or to ctxMacroArguments
	              // in which case next brace will end macro arguments
	expand_macro->pop_push_back();
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\,{WS}*/\, {
	expand_macro->pop_push_back();
	expand_macro->push_arg_plain("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\,{WS}*/\( {
	expand_macro->pop_push_back();
	expand_macro->push_arg_call("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\,{WS}*/\) {
	expand_macro->pop_push_back();
	expand_macro->push_arg_plain("");
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\,{WS}* {
	expand_macro->pop_push_back();
}

<ctxMacroArguments>\) {
	expand_macro->pop_push_back();
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxMacroArgumentsPre context
	handleExpandFunctionLike();
}

<<EOF>> {
	l.flow("eof");
	yyterminate();
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

<ctxDefine>{WS}

<ctxDefine>{ID}/\( {
	construct_macro->name = yytext;
	BEGINCONTEXT(ctxDefineFunctionLikeMacro);
}

<ctxDefine>{ID}/[^\(] {
	construct_macro->name = yytext;
	BEGINCONTEXT(ctxDefineObjectLikeMacro);
}

<ctxDefineObjectLikeMacro>{WS}

<ctxDefineObjectLikeMacro>{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxDefine context
	ENDCONTEXT(); // also pop ctxPreporcessor context
	handleDefineObjectLike();
}

<ctxDefineObjectLikeMacro>[^ \t\r\n][^\r\n]*/{NL} {
	construct_macro->repl = yytext;
}

<ctxDefineFunctionLikeMacro>\(/{WS}*[^\,] {
	BEGINCONTEXT(ctxDefineFunctionLikeMacroArgs);
}

<ctxDefineFunctionLikeMacro>\({WS}*[\,][^\r\n]*{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxDefine context
	ENDCONTEXT(); // also pop ctxPreporcessor context
	l.error("argument name cannot be empty");
	construct_macro = nullptr;
}

<ctxDefineFunctionLikeMacroArgs>{WS}

<ctxDefineFunctionLikeMacroArgs>[^\,\(\) \t\r\n][^\,\(\)\r\n]*/[\,\(\)] {
	construct_macro->args.push_back(yytext);
}

<ctxDefineFunctionLikeMacroArgs>\,/{WS}*[^\)\,]

<ctxDefineFunctionLikeMacroArgs>\,{WS}*[\)\,][^\r\n]*{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxDefineFunctionLikeMacro context
	ENDCONTEXT(); // also pop ctxDefine context
	ENDCONTEXT(); // also pop ctxPreporcessor context
	l.error("argument name cannot be empty");
	construct_macro = nullptr;
}

<ctxDefineFunctionLikeMacroArgs>\([^\r\n]*{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxDefineFunctionLikeMacro context
	ENDCONTEXT(); // also pop ctxDefine context
	ENDCONTEXT(); // also pop ctxPreporcessor context
	l.error("unexpected opening brace in macro definition argument list context");
	construct_macro = nullptr;
}

<ctxDefineFunctionLikeMacroArgs>\)/{TS} {
	ENDCONTEXT();
	BEGINCONTEXT(ctxDefineFunctionLikeMacroReplacement);
}

<ctxDefineFunctionLikeMacroReplacement>{WS}

<ctxDefineFunctionLikeMacroReplacement>{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxDefineFunctionLikeMacro context
	ENDCONTEXT(); // also pop ctxDefine context
	ENDCONTEXT(); // also pop ctxPreporcessor context
	handleDefineFunctionLike();
}

<ctxDefineFunctionLikeMacroReplacement>[^ \t\r\n][^\r\n]*/{NL} {
	construct_macro->repl = yytext;
}

<ctxUndef>{WS}

<ctxUndef>{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxPreporcessor context
}

<ctxUndef>{ID}/{WS}*{NL} {
	handleUndefine(yytext);
}

<ctxUndef>[^\r\n]*[^ \t0-9a-zA-Z_\r\n]+[^\r\n]*/{NL} {
	l.error("invalid undefine directive trailer: %s", yytext);
}

<ctxPreprocessor>{NL} {
	l.flow("end preprocessor directive");
	ENDCONTEXT();
}

<ctxPreprocessor>include/{TS} {
	BEGINCONTEXT(ctxInclude);
}

<ctxPreprocessor>define/{TS} {
	BEGINCONTEXT(ctxDefine);
	construct_macro = std::make_unique<ConstructMacroStatus>();
}

<ctxPreprocessor>undef/{TS} {
	BEGINCONTEXT(ctxUndef);
}

<ctxPreprocessor>[^ \t\r\n]+/{TS} {
	l.warning("unrecognized preprocessor directive: %s", yytext);
	BEGINCONTEXT(ctxPreprocessorUnrecognized);
}

<ctxPreprocessorUnrecognized>{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxPreprocessor context
}

<ctxPreprocessorUnrecognized>[^\r\n]* {
	l.warning("ignoring unrecognized preprocessor directive trailer: %s", yytext);
}

%%

int Inliner_FlexLexer::yywrap() {
	return 1;
}
