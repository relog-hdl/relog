#include "inliner.hpp"

#include <iostream>
#include <sstream>

namespace relog::pipeline::inliner
{

Inliner::Inliner(Pipeline& pipeline, Decanter& decanter, std::ostream& out) : yyFlexLexer(std::cin, out), pl(pipeline), dctr(decanter), os(out), d2i_buf(std::ios::in | std::ios::out), d2i_buf_in(&d2i_buf), d2i_buf_out(&d2i_buf)
{
	runDecanter();
}

int
Inliner::yywrap()
{
	pl.ssc.pop();

	if(pl.ssc.empty())
		return true;

	runDecanter();
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

void
Inliner::runDecanter()
{
	dctr.switch_streams(pl.ssc.top()->getFilestream(), d2i_buf_in);
	switch_streams(d2i_buf_out, os);

	dctr.run();
}

} // namespace relog::pipeline::inliner
