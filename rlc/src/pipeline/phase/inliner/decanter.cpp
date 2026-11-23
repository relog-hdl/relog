#include "decanter.hpp"

#include <iostream>

namespace relog::pipeline::inliner
{

Decanter::Decanter(Pipeline& pipeline) : pl(pipeline) {}

void
Decanter::run()
{
	int ret;

	while(true)
	{
		ret = get_next_token();
		if(ret == DECANTER_END)
			break;
	}
}

void
Decanter::LexerError(const char* msg)
{
	l.error("%s", msg);
	exit(2);
}

} // namespace relog::pipeline::inliner
