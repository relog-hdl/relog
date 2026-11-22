#include "source_stack.hpp"

#include <lib/io/source_file.hpp>

#include <fstream>
#include <memory>

namespace relog::service::source
{

SourceStackController::SourceStackController(std::vector<char*>& filenames)
{
	using relog::lib::SourceFile;

	std::shared_ptr<SourceFile> file;
	for(char* filename : filenames)
	{
		file = std::make_shared<SourceFile>(filename);
		stack.push(std::move(file));
	}
}

std::shared_ptr<relog::lib::SourceFile>
SourceStackController::openPush(char* filename)
{
	using relog::lib::SourceFile;

	std::shared_ptr<SourceFile> file = std::make_shared<SourceFile>(filename);
	stack.push(file);

	return file;
}

std::shared_ptr<relog::lib::SourceFile>
SourceStackController::top()
{
	using relog::lib::SourceFile;

	std::shared_ptr<SourceFile> file = stack.top();

	return file;
}

void
SourceStackController::pop()
{
	stack.pop();
}

bool
SourceStackController::empty()
{
	return stack.empty();
}

} // namespace relog::service::source
