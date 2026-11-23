#include "lexer.hpp"

#include <iostream>

namespace relog::pipeline::lex
{

Lexer::Lexer(Pipeline& pipeline, std::istream& in) : yyFlexLexer(in, std::cout), pl(pipeline), is(in) {}

void
Lexer::LexerError(const char* msg)
{
	l.error("%s", msg);
	exit(2);
}

} // namespace relog::pipeline::lex
