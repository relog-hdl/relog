%{
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wunused-function"
%}

/* disable yywrap while lexer integration is non-existent */
%option noyywrap

%option prefix="lexer_"

%%
