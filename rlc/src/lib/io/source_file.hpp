#ifndef RELOG_LIB_IO_SOURCE_FILE_HPP
#define RELOG_LIB_IO_SOURCE_FILE_HPP

#include <fstream>
#include <iostream>
#include <sstream>

namespace relog::lib
{

class SourceFile
{
	protected:
		std::stringbuf buffer;
		std::ostream writestream;
		std::istream readstream;
		std::ifstream filestream;
		std::string filename;

	public:
		SourceFile(char* filename_);

		virtual ~SourceFile() = default;

		std::string getFilename();

		std::ostream& getWritestream();

		std::istream& getReadstream();

		bool bufferEmpty();

		std::ifstream& getFilestream();
};

} // namespace relog::lib

#endif
