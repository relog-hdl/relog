#include "parser.hpp"

namespace relog::pipeline::parse
{

void
Parser::error(const std::string& message)
{
	l.error("%s", message.c_str());
	exit(2);
}

} // namespace relog::pipeline::parse
