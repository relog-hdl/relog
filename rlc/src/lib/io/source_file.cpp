#include "source_file.hpp"

#include <fstream>
#include <stdexcept>

namespace relog::lib
{

SourceFile::SourceFile(char* filename_) : filestream(filename_, std::ios::in), filename(filename_)
{
	if(!filestream)
	{
		std::string str = "error opening file: ";
		str.append(filename);
		throw new std::runtime_error{str.c_str()};
	}
}

std::string
SourceFile::getFilename()
{
	return filename;
}

std::ifstream&
SourceFile::getFilestream()
{
	return filestream;
}

} // namespace relog::lib
