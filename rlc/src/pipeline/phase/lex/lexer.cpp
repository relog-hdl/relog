#include "lexer.hpp"

namespace relog::pipeline::lex
{

void
Lexer::LexerError(const char* msg)
{
	l.error("%s", msg);
	exit(2);
}

} // namespace relog::pipeline::lex
