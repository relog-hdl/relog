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

%x ctxPreprocessor

%x ctxPreprocessorUnrecognized

%x ctxInclude

%x ctxDefine

%x ctxDefineObjectLikeMacro

%x ctxDefineFunctionLikeMacro

%x ctxDefineFunctionLikeMacroReplacement

%x ctxMacroArgumentsPre

%x ctxMacroArguments

%x ctxMacroArgumentsBracedInside

%%

{SL} {
	// ignore preprocessor directives inside of string literals
	yyout << yytext;
}

^{WS}*` {
	l.flow("begin preprocessor directive");
	BEGINCONTEXT(ctxPreprocessor);
}

{ID} {
	std::string id{yytext};
	if(object_like_macros.count(id) > 0)
	{
		push_string(object_like_macros[id]);
		l.flow("replacing object-like macro: %s -> '%s'", id.c_str(), object_like_macros[id].c_str());
	}
	else if(function_like_macros.count(id) > 0)
	{
		l.flow("collecting arguments for function-like macro");
		expand_macro = std::make_unique<ExpandMacroStatus>();
		expand_macro->name = id;
		BEGINCONTEXT(ctxMacroArgumentsPre);
	}
	else
		yyout << yytext;
}

<ctxMacroArgumentsPre>\( {
	BEGINCONTEXT(ctxMacroArguments);
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>[^\(\)\,\r\n]* {
	expand_macro->current_arg.append(yytext);
}

<ctxMacroArguments,ctxMacroArgumentsBracedInside>\( {
	expand_macro->current_arg.append("(");
	BEGINCONTEXT(ctxMacroArgumentsBracedInside);
}

<ctxMacroArgumentsBracedInside>\) {
	ENDCONTEXT(); // pops either to ctxMacroArgumentsBracedInside or to ctxMacroArguments
	              // in which case next brace will end macro arguments
	expand_macro->current_arg.append(")");
}

<ctxMacroArgumentsBracedInside>\,{WS}* {
	expand_macro->current_arg.append(",");
}

<ctxMacroArguments>\,{WS}* {
	expand_macro->args.push_back(expand_macro->current_arg);
	expand_macro->current_arg = std::string{};
}

<ctxMacroArguments>\) {
	expand_macro->args.push_back(expand_macro->current_arg);
	expand_macro->current_arg = std::string{};
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxMacroArgumentsPre context
	l.flow("handling function-like macro: %s", expand_macro->name.c_str());
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
	l.flow("defining object-like macro: %s -> '%s'", construct_macro->name.c_str(), construct_macro->repl.c_str());
	handleDefineObjectLike();
}

<ctxDefineObjectLikeMacro>[^ \t\r\n][^\r\n]*/{NL} {
	construct_macro->repl = yytext;
}

<ctxDefineFunctionLikeMacro>\([ \t0-9a-zA-Z_\,]*\) {
	std::string str = yytext;
	construct_macro->args = str.substr(1,str.size()-2);
	BEGINCONTEXT(ctxDefineFunctionLikeMacroReplacement);
}

<ctxDefineFunctionLikeMacroReplacement>{WS}

<ctxDefineFunctionLikeMacroReplacement>{NL} {
	ENDCONTEXT();
	ENDCONTEXT(); // also pop ctxDefineFunctionLikeMacro context
	ENDCONTEXT(); // also pop ctxDefine context
	ENDCONTEXT(); // also pop ctxPreporcessor context
	l.flow("defining function-like macro: %s(%s) -> '%s'", construct_macro->name.c_str(), construct_macro->args.c_str(), construct_macro->repl.c_str());
	handleDefineFunctionLike();
}

<ctxDefineFunctionLikeMacroReplacement>[^ \t\r\n][^\r\n]*/{NL} {
	construct_macro->repl = yytext;
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
