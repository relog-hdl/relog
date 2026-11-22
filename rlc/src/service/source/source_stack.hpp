#ifndef RELOG_SERVICE_SOURCE_SOURCE_STACK_HPP
#define RELOG_SERVICE_SOURCE_SOURCE_STACK_HPP

#include <lib/io/source_file.hpp>

#include <memory>
#include <stack>
#include <vector>

namespace relog::service::source
{

class SourceStackController
{
	protected:
		std::stack<std::shared_ptr<relog::lib::SourceFile>> stack;

	public:
		SourceStackController(std::vector<char*>& filenames);

		virtual ~SourceStackController() = default;

		std::shared_ptr<relog::lib::SourceFile> openPush(char* filename);

		std::shared_ptr<relog::lib::SourceFile> top();

		void pop();

		bool empty();
};

} // namespace relog::service::source

#endif
