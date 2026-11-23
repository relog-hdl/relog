#include "source_file.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace relog::lib
{

SourceFile::SourceFile(char* filename_) : buffer(std::ios::in | std::ios::out), writestream(&buffer), readstream(&buffer), filestream(filename_, std::ios::in), filename(filename_)
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

std::ostream&
SourceFile::getWritestream()
{
	return writestream;
}

std::istream&
SourceFile::getReadstream()
{
	return readstream;
}

bool
SourceFile::bufferEmpty()
{
	return buffer.view().empty();
}

std::ifstream&
SourceFile::getFilestream()
{
	return filestream;
}

} // namespace relog::lib
