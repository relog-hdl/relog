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
Inliner::checkMacroExists(std::string& name)
{
	if(macroExists(name))
	{
		std::string err = "attempted to redefine existing macro: ";
		err.append(name);
		LexerError(err.c_str());
	}
}

bool
Inliner::macroExists(std::string& name)
{
	return object_like_macros.count(name) > 0 || function_like_macros.count(name) > 0;
}

void
Inliner::handleDefineObjectLike()
{
	checkMacroExists(construct_macro->name);

	define(construct_macro->name, construct_macro->repl);

	construct_macro = nullptr;
}

void
Inliner::handleDefineFunctionLike()
{
	checkMacroExists(construct_macro->name);

	define(construct_macro->name, construct_macro->args, construct_macro->repl);

	construct_macro = nullptr;
}

void
Inliner::define(std::string& name, std::string& replacement)
{
	l.flow("defining object-like macro: %s -> '%s'", construct_macro->name.c_str(), construct_macro->repl.c_str());

	object_like_macros[std::move(name)] = std::move(replacement);
}

void
Inliner::define(std::string& name, std::vector<std::string>& arguments, std::string& replacement)
{
	l.flow("defining function-like macro: %s -> '%s'", name.c_str(), replacement.c_str());
	for(std::string arg : arguments)
		l.flow("function-like macro argument: %s", arg.c_str());

	std::unique_ptr<FunctionLikeMacro> flmacro = std::make_unique<FunctionLikeMacro>(std::move(arguments), std::move(replacement));
	function_like_macros[std::move(name)] = std::move(flmacro);
}

Inliner::ExpandMacroArg::ExpandMacroArg(std::string& value_) : value(value_) {}

void
Inliner::ExpandMacroArgPlain::admit(IExpandMacroArgVisitor& visitor)
{
	visitor.visit(*this);
}

void
Inliner::ExpandMacroArgCall::admit(IExpandMacroArgVisitor& visitor)
{
	visitor.visit(*this);
}

Inliner::ExpandMacroArgPrintVisitor::ExpandMacroArgPrintVisitor(relog::service::logging::Loggerf& l_) : l(l_) {}

void
Inliner::ExpandMacroArgPrintVisitor::visit(ExpandMacroArgPlain& arg)
{
	l.flow("plain arg: %s", arg.value.c_str());
}

void
Inliner::ExpandMacroArgPrintVisitor::visit(ExpandMacroArgCall& arg)
{
	l.flow("call arg: %s", arg.value.c_str());

	for(auto&& child_arg : arg.args)
		child_arg->admit(*this);

	l.flow("end call arg: %s", arg.value.c_str());
}

Inliner::ExpandMacroArgExpandVisitor::ExpandMacroArgExpandVisitor(Inliner& i_, relog::service::logging::Loggerf& l_) : i(i_), l(l_) {}

void
Inliner::ExpandMacroArgExpandVisitor::visit(ExpandMacroArgPlain& arg)
{
	arg.replacement = arg.value;

	std::unordered_map<std::string, int> expanded;
	while(true)
	{
		if(!i.expand_object_like_once(arg.replacement, expanded))
			break;
	}
}

void
Inliner::ExpandMacroArgExpandVisitor::visit(ExpandMacroArgCall& arg)
{
	// does not need to visit children
	// because it will be done by Inliner::expand

	ExpandMacroArgCall* p = &arg;
	auto ptr = std::unique_ptr<ExpandMacroArgCall>{p};
	i.expand(arg.value, ptr);
	ptr.release();
}

void
Inliner::ExpandMacroStatus::push_arg_call(const char* val)
{
	std::string val_str = val;
	ExpandMacroArgCall* arg = new ExpandMacroArgCall{val_str};
	std::unique_ptr<ExpandMacroArg> arg_ptr = std::unique_ptr<ExpandMacroArg>{reinterpret_cast<ExpandMacroArg*>(arg)};
	arg_stack.push(std::move(arg_ptr));
}

void
Inliner::ExpandMacroStatus::push_arg_plain(const char* val)
{
	std::string val_str = val;
	ExpandMacroArgPlain* arg = new ExpandMacroArgPlain{val_str};
	std::unique_ptr<ExpandMacroArg> arg_ptr = std::unique_ptr<ExpandMacroArg>{reinterpret_cast<ExpandMacroArg*>(arg)};
	arg_stack.push(std::move(arg_ptr));
}

void
Inliner::ExpandMacroStatus::pop_push_back()
{
	std::unique_ptr<ExpandMacroArg> arg = std::move(arg_stack.top());
	arg_stack.pop();

	if(arg_stack.empty())
		arg_root->args.push_back(std::move(arg));
	else
	{
		// all arguments on the stack except for the last one are always calls by definition
		ExpandMacroArgCall* parent = reinterpret_cast<ExpandMacroArgCall*>(arg_stack.top().get());
		parent->args.push_back(std::move(arg));
	}
}

void
Inliner::handleExpandObjectLike()
{
	std::string expansion = expand(expand_macro->name);

	push_string(expansion);

	expand_macro = nullptr;
}

void
Inliner::handleExpandFunctionLike()
{
	std::string expansion = expand(expand_macro->name, expand_macro->arg_root);

	push_string(expansion);

	expand_macro = nullptr;
}

std::string
Inliner::expand(std::string& name)
{
	l.flow("expanding object-like macro: %s", name.c_str());
	return object_like_macros[name];
}

std::string
Inliner::expand(std::string& name, std::unique_ptr<ExpandMacroArgCall>& arg_root)
{
	std::unique_ptr<FunctionLikeMacro>& macro = function_like_macros[name];

	l.flow("expanding function-like macro: %s", name.c_str());
	ExpandMacroArgPrintVisitor vp{l};
	arg_root->admit(vp);

	int required_arg_count = macro->arg_names.size();
	int arg_count = arg_root->args.size();

	if(arg_count != required_arg_count)
	{
		l.error("error expanding macro '%s': requires %d arguments, provided %d", name.c_str(), required_arg_count, arg_count);
		return "";
	}

	// TODO(Gleb Salmanov): Stringification and prescan

	ExpandMacroArgExpandVisitor ve{*this, l};
	for(auto&& arg : arg_root->args)
		arg->admit(ve);

	std::string& repl = arg_root->replacement;
	repl = macro->repl;

	for(int arg = 0; arg < arg_count; arg++)
	{
		std::string& arg_name = macro->arg_names[arg];
		std::string& arg_replacement = arg_root->args[arg]->replacement;

		replace_all_id_occurences(repl, arg_name, arg_replacement);
	}

	return repl;
}

bool
Inliner::expand_object_like_once(std::string& str, std::unordered_map<std::string, int>& expanded)
{
	for(auto& name_ref : std::views::keys(object_like_macros))
	{
		std::string& name = const_cast<std::string&>(name_ref);

		if(replace_all_id_occurences(str, name, object_like_macros[name]))
		{
			if(expanded.count(name) > 0)
				expanded[name] += 1;
			else
				expanded[name] = 1;

			if(expanded[name] >= RELOG_INLINER_WARN_BUFFER_STACK_SIZE)
			{
				l.warning("max stack size approaching: %d", RELOG_INLINER_MAX_BUFFER_STACK_SIZE - expanded[name]);
				if(expanded[name] >= RELOG_INLINER_MAX_BUFFER_STACK_SIZE)
					LexerError("max stack size reached");
			}

			return true;
		}
	}

	return false;
}

inline bool
Inliner::replace_all_id_occurences(std::string& in, std::string& of, std::string& with)
{
	bool replaced = false;

	char validator;
	bool valid;

	size_t pos = 0;
	while((pos = in.find(of, pos)) != std::string::npos)
	{
		valid = true;
		validator = in[pos + of.length()];

		if(validator > 'a' && validator < 'z')
			valid = false;
		else if(validator > 'A' && validator < 'Z')
			valid = false;
		else if(validator > '0' && validator < '9')
			valid = false;
		else if(validator == '_')
			valid = false;

		if(valid)
		{
			in.replace(pos, of.length(), with);
			pos += with.length();
			replaced = true;
		}
		else
		{
			pos += of.length();
			continue;
		}
	}

	return replaced;
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
