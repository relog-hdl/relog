#ifndef RELOG_LIB_IO_SOURCE_FILE_HPP
#define RELOG_LIB_IO_SOURCE_FILE_HPP

#include <fstream>

namespace relog::lib
{

class SourceFile
{
	protected:
		std::ifstream filestream;
		std::string filename;

	public:
		SourceFile(char* filename_);

		virtual ~SourceFile() = default;

		std::string getFilename();

		std::ifstream& getFilestream();
};

} // namespace relog::lib

#endif
