#include "inliner.hpp"

#include <lib/io/source_file.hpp>

#include <iostream>
#include <sstream>
#include <string>

namespace relog::pipeline::inliner
{

Inliner::Inliner(Pipeline& pipeline, Decanter& decanter, std::ostream& out) : yyFlexLexer(pipeline.ssc.top()->getReadstream(), out), pl(pipeline), dctr(decanter), os(out)
{
	runDecanter();
}

int
Inliner::yywrap()
{
	pl.ssc.pop();

	if(pl.ssc.empty())
		return true;

	switch_streams(pl.ssc.top()->getReadstream(), os);
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
	if(pl.ssc.top()->bufferEmpty())
	{
		dctr.switch_streams(pl.ssc.top()->getFilestream(), pl.ssc.top()->getWritestream());
		dctr.run();
	}
}

void
Inliner::handleInclude(char* inc_str)
{
	std::string str{inc_str};
	std::string path = str.substr(1, str.rfind(inc_str[0] == '"' ? '"' : '>') - 1);
	include(inc_str[0] == '<', path);
}

void
Inliner::include(bool external, std::string path)
{
	l.flow("including: %d %s", external, path.c_str());

	try
	{
		pl.ssc.openPush(const_cast<char*>(path.c_str()));
		switch_streams(pl.ssc.top()->getReadstream(), os);
		runDecanter();
	}
	catch(const std::exception& e)
	{
		l.critical("error including %s: %s", path.c_str(), e.what());
		exit(2);
	}
}

} // namespace relog::pipeline::inliner
