#include "lexer.hpp"

#include <fstream>
#include <iostream>

namespace relog::pipeline::lex
{

Lexer::Lexer(Pipeline& pipeline) : yyFlexLexer(pipeline.ssc.top()->getFilestream(), std::cout), pl(pipeline) {}

int
Lexer::yywrap()
{
	pl.ssc.pop();

	if(pl.ssc.empty())
		return true;

	std::ifstream& new_yyin = pl.ssc.top()->getFilestream();

	switch_streams(new_yyin, std::cout);

	return false;
}

void
Lexer::LexerError(const char* msg)
{
	l.error("%s", msg);
	exit(2);
}

} // namespace relog::pipeline::lex
