#include "inliner.hpp"

#include <iostream>

namespace relog::pipeline::inliner
{

Inliner::Inliner(Pipeline& pipeline, std::ostream& out) : yyFlexLexer(pipeline.ssc.top()->getFilestream(), out), pl(pipeline), os(out) {}

int
Inliner::yywrap()
{
	pl.ssc.pop();

	if(pl.ssc.empty())
		return true;

	std::ifstream& new_yyin = pl.ssc.top()->getFilestream();

	switch_streams(new_yyin, os);

	return false;
}

void
Inliner::run()
{
	int ret;

	while(true)
	{
		ret = get_next_token();
		if(ret == INLINER_END)
			break;
	}
}

void
Inliner::LexerError(const char* msg)
{
	l.error("%s", msg);
	exit(2);
}

} // namespace relog::pipeline::inliner
