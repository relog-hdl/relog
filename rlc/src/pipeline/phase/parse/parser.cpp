#include "parser.hpp"

namespace relog::pipeline::parse
{

Parser::Parser(Pipeline& pipeline_, relog::pipeline::lex::Lexer& lexer_) : Parser_BisonParser(*this, lexer_, get_logger()), pl(pipeline_) {}

void
Parser::error(const std::string& message)
{
	l.error("%s", message.c_str());
	exit(2);
}

} // namespace relog::pipeline::parse
