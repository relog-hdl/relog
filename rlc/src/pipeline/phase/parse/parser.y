%{
#include <stdio.h>
#include <stdlib.h>

extern int lexer_lex(void);

int parser_lex(void);

void parser_error(const char *s);
%}

%define api.prefix {parser_}

%%

placeholder:;

%%

int parser_lex(void) {
	return lexer_lex();
}

/* temporary yyerror */
void parser_error(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
