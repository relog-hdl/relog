#include "inliner.hpp"

#include <lib/io/source_file.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <vector>

#include <memory>

namespace relog::pipeline::inliner
{

Inliner::Inliner(Pipeline& pipeline, Decanter& decanter, std::ostream& out) : yyFlexLexer(pipeline.ssc.top()->getReadstream(), out), pl(pipeline), dctr(decanter), os(out)
{
	runDecanter();
}

int
Inliner::yywrap()
{
	if(!streams.empty())
	{
		l.flow("popping buffer");

		yypop_buffer_state();
		streams.pop();

		if(!streams.empty())
		{
			l.flow("streams not empty, not wrapping");
			return false;
		}
	}

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
Inliner::include(bool external, std::string& path)
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

Inliner::FunctionLikeMacro::FunctionLikeMacro(std::vector<std::string> arg_names_, std::string repl_) : arg_names(std::move(arg_names_)), repl(repl_) {}

void
Inliner::handleDefineObjectLike()
{
	checkMacroExists();

	define(construct_macro->name, construct_macro->repl);

	construct_macro = nullptr;
}

void
Inliner::handleDefineFunctionLike()
{
	checkMacroExists();

	std::vector<std::string> args;

	std::stringstream sstream{construct_macro->args};
	for(std::string arg; std::getline(sstream, arg, ','); args.push_back(arg))
		;

	define(construct_macro->name, args, construct_macro->repl);

	construct_macro = nullptr;
}

void
Inliner::checkMacroExists()
{
	if(object_like_macros.count(construct_macro->name) > 0 || function_like_macros.count(construct_macro->name) > 0)
	{
		std::string err = "attempted to redefine existing macro: ";
		err.append(construct_macro->name);
		LexerError(err.c_str());
	}
}

void
Inliner::define(std::string& name, std::string& replacement)
{
	object_like_macros[std::move(name)] = std::move(replacement);
}

void
Inliner::define(std::string& name, std::vector<std::string>& arguments, std::string& replacement)
{
	std::unique_ptr<FunctionLikeMacro> flmacro = std::make_unique<FunctionLikeMacro>(std::move(arguments), std::move(replacement));
	function_like_macros[std::move(name)] = std::move(flmacro);
}

void
Inliner::handleExpandFunctionLike()
{
	for(std::string arg : expand_macro->args)
		l.flow("funciton-like macro arg: %s", arg.c_str());

	// TODO(Gleb Salmanov): Expand

	expand_macro = nullptr;
}

void
Inliner::push_string(std::string& str)
{
	std::unique_ptr<std::istringstream> str_stream = std::make_unique<std::istringstream>(str);
	yy_buffer_state* buffer = yy_create_buffer(str_stream.get(), YY_BUF_SIZE);

	streams.push(std::move(str_stream));
	yypush_buffer_state(buffer);

	if(streams.size() >= RELOG_INLINER_WARN_BUFFER_STACK_SIZE)
	{
		l.warning("max stack size approaching: %lu", RELOG_INLINER_MAX_BUFFER_STACK_SIZE - streams.size());
		if(streams.size() >= RELOG_INLINER_MAX_BUFFER_STACK_SIZE)
			LexerError("max stack size reached");
	}

	l.flow("pushed string to streams: %s", str.c_str());
}

} // namespace relog::pipeline::inliner
