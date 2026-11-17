#ifndef RELOG_SERVICE_CODETREE_CODETREE_HPP
#define RELOG_SERVICE_CODETREE_CODETREE_HPP

namespace relog::service::codetree
{

class CodeTree
{
		virtual ~CodeTree();
};

class CodeTreeNode
{
		virtual ~CodeTreeNode();

	protected:
		enum Type
		{
			DIRECTORY,
			FILE
		};
};

} // namespace relog::service::codetree

#endif
