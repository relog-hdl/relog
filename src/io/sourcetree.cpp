#include <fstream>

#include <util/namespace.hpp>

#include "sourcetree.hpp"
using namespace relog;

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
