/*
 * Cpp structure & integration based on Flex & Bison LALR C++ Example
 * used under The MIT License (MIT)
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@ezaquarii.com>
 * (https://github.com/ezaquarii/bison-flex-cpp-example)
 */

#ifndef RELOG_PIPELINE_INLINER_INLINER_HPP
#define RELOG_PIPELINE_INLINER_INLINER_HPP

#include <pipeline/phase/inliner/decanter.hpp>
#include <pipeline/pipeline.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

#include <iostream>
#include <sstream>

#include <unordered_map>
#include <vector>

#include <memory>

// clang-format off
/**
 * Generated Flex class name is yyFlexLexer by default. If we want to use more flex-generated
 * classes we should name them differently. See lexer.l prefix option.
 *
 * Unfortunately the implementation relies on this trick with redefining class name
 * with a preprocessor macro. See GNU Flex manual, "Generating C++ Scanners" section
 */
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer Inliner_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL int relog::pipeline::inliner::Inliner::get_next_token()

#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif
// clang-format on

#define RELOG_INLINER_MAX_BUFFER_STACK_SIZE 1024

#define RELOG_INLINER_BUFFER_STACK_SIZE_GUARD_INTERVAL 10

#define RELOG_INLINER_WARN_BUFFER_STACK_SIZE (RELOG_INLINER_MAX_BUFFER_STACK_SIZE - RELOG_INLINER_BUFFER_STACK_SIZE_GUARD_INTERVAL)

#define INLINER_END 0

namespace relog::pipeline::inliner
{

class Inliner final : public yyFlexLexer
{
	public:
		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = relog::pipeline::Pipeline::get_domain() / "inliner";
			return domain_;
		}

		static relog::service::logging::Loggerf&
		get_logger()
		{
			static relog::service::logging::Loggerf _logger{get_domain(), relog::service::logging::LogLevel::INFO};
			return _logger;
		}

	protected:
		relog::service::logging::LogDomainProxy<std::string>& domain = get_domain();
		relog::service::logging::Loggerf& l = get_logger();

	protected:
		relog::pipeline::Pipeline& pl;
		relog::pipeline::inliner::Decanter& dctr;
		std::ostream& os;

	public:
		Inliner(relog::pipeline::Pipeline& pipeline, relog::pipeline::inliner::Decanter& decanter, std::ostream& out);

		virtual int get_next_token();

		void run();

		int yywrap();

		[[noreturn]] void LexerError(const char* msg);

	protected:
		void runDecanter();

	protected:
		void handleInclude(char* inc_str);

		void include(bool external, std::string& path);

	protected:
		struct FunctionLikeMacro final
		{
				std::vector<std::string> arg_names;
				std::string repl;

			public:
				FunctionLikeMacro(std::vector<std::string> arg_names_, std::string repl_);
		};

		std::unordered_map<std::string, std::string> object_like_macros;
		std::unordered_map<std::string, std::unique_ptr<FunctionLikeMacro>> function_like_macros;

		void checkMacroExists(std::string& name);

		bool macroExists(std::string& name);

	protected:
		struct ConstructMacroStatus final
		{
				std::string name, repl;
				std::vector<std::string> args;
		};

		std::unique_ptr<ConstructMacroStatus> construct_macro = nullptr;

		void handleDefineObjectLike();

		void handleDefineFunctionLike();

		void define(std::string& name, std::string& replacement);

		void define(std::string& name, std::vector<std::string>& arguments, std::string& replacement);

	protected:
		struct IExpandMacroArgVisitor;

		struct ExpandMacroArg
		{
				std::string value;
				std::string replacement;

			public:
				ExpandMacroArg(std::string& value_);

				virtual ~ExpandMacroArg() = default;

				virtual void admit(IExpandMacroArgVisitor& visitor) = 0;
		};

		struct ExpandMacroArgPlain final : public ExpandMacroArg
		{
			public:
				using ExpandMacroArg::ExpandMacroArg;

				void admit(IExpandMacroArgVisitor& visitor);
		};

		struct ExpandMacroArgCall final : public ExpandMacroArg
		{
				std::vector<std::unique_ptr<ExpandMacroArg>> args;

			public:
				using ExpandMacroArg::ExpandMacroArg;

				void admit(IExpandMacroArgVisitor& visitor);
		};

		struct IExpandMacroArgVisitor
		{
			public:
				virtual ~IExpandMacroArgVisitor() = default;

				virtual void visit(ExpandMacroArgPlain& arg) = 0;

				virtual void visit(ExpandMacroArgCall& arg) = 0;
		};

		struct ExpandMacroArgPrintVisitor final : public IExpandMacroArgVisitor
		{
				relog::service::logging::Loggerf& l;

			public:
				ExpandMacroArgPrintVisitor(relog::service::logging::Loggerf& l_);

				void visit(ExpandMacroArgPlain& arg);

				void visit(ExpandMacroArgCall& arg);
		};

		struct ExpandMacroArgExpandVisitor final : public IExpandMacroArgVisitor
		{
				relog::pipeline::inliner::Inliner& i;
				relog::service::logging::Loggerf& l;

			public:
				ExpandMacroArgExpandVisitor(relog::pipeline::inliner::Inliner& i_, relog::service::logging::Loggerf& l_);

				void visit(ExpandMacroArgPlain& arg);

				void visit(ExpandMacroArgCall& arg);
		};

	protected:
		struct ExpandMacroStatus final
		{
				std::string name;
				std::stack<std::unique_ptr<ExpandMacroArg>> arg_stack;
				std::unique_ptr<ExpandMacroArgCall> arg_root = nullptr;

			public:
				void push_arg_plain(const char* val);

				void push_arg_call(const char* val);

				void pop_push_back();
		};

		std::unique_ptr<ExpandMacroStatus> expand_macro = nullptr;

		void handleExpandObjectLike();

		void handleExpandFunctionLike();

	public:
		std::string expand(std::string& name);

		std::string expand(std::string& name, std::unique_ptr<ExpandMacroArgCall>& arg_root);

		bool expand_object_like_once(std::string& str, std::unordered_map<std::string, int>& expanded);

		inline bool replace_all_id_occurences(std::string& in, std::string& of, std::string& with);

	protected:
		std::stack<std::unique_ptr<std::istringstream>> streams;

		void push_string(std::string& str);
};

} // namespace relog::pipeline::inliner

#endif
