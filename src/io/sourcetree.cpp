#include <fstream>

#include "sourcetree.hpp"

namespace relog
{

class SourceTree
{
};

class SourceTreeNode
{
protected:
	enum Type
	{
		DIRECTORY,
		FILE
	};
};

} // namespace relog
