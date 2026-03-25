#ifndef RELOG_LIB_DOMAIN_HPP
#define RELOG_LIB_DOMAIN_HPP

#include <list>
#include <map>
#include <memory>
#include <string>

#define RELOG_LIB_DOMAIN_HPP_DEFAULT_SEPARATOR '/'

namespace relog::lib
{

/**
 * @class Domain
 * @brief Represents a domain tree node
 * @author Gleb Salmanov
 *
 * Represents a node of a domain tree, meaning a single subdomain within
 * a domain hierarchy.
 *
 * An instance must not be manually deallocated, unless it is a root node
 * which was not allocated by `DomainOwner`.
 * Deallocating the root node deallocates all of the children.
 *
 * Intended for usecases where it does not make sense to keep children
 * around after the parent has been deallocated, and children are not
 * deallocated until the entire tree is.
 *
 * Should be used with `DomainOwner`.
 */
class Domain
{
		bool m_is_root;
		std::weak_ptr<Domain> m_self;
		std::weak_ptr<Domain> m_root;
		std::weak_ptr<Domain> m_parent;
		std::map<std::string, std::shared_ptr<Domain>> m_children;
		std::string m_name;

	protected:
		Domain(std::weak_ptr<Domain> parent, std::string name);

		template<typename... strings>
		std::shared_ptr<Domain> getOrCreateChildrenImpl_(std::shared_ptr<Domain> current, std::string name, strings... names);

		std::shared_ptr<Domain> getOrCreateChildrenImpl_(std::shared_ptr<Domain> current);

	public:
		virtual ~Domain();

		static std::shared_ptr<Domain> createRoot(std::string name);

		std::shared_ptr<Domain> getOrCreateChild(std::string name);

		template<typename... strings>
		std::shared_ptr<Domain> getOrCreateChildren(std::string name, strings... names);

		bool isRoot();

		std::weak_ptr<Domain> getRoot();

		std::weak_ptr<Domain> getParent();

		std::list<std::weak_ptr<Domain>> getChain();

		std::string getName();

		std::string getFullyQualifiedName(const char separator = RELOG_LIB_DOMAIN_HPP_DEFAULT_SEPARATOR, bool reverse = false);

		std::partial_ordering compare(Domain* other);

		std::partial_ordering compare(Domain& other);

		std::partial_ordering compare(std::shared_ptr<Domain> other);

		std::partial_ordering operator<=>(const Domain&) const;
};

class DomainProxy
{
		std::shared_ptr<Domain> m_domain;

	public:
		DomainProxy(std::shared_ptr<Domain> domain);

		virtual ~DomainProxy();

		template<typename... strings>
		DomainProxy relative(strings... names);

		DomainProxy operator/(std::string name);

		bool isRoot();

		DomainProxy getRoot();

		DomainProxy getParent();

		std::list<DomainProxy> getChain();

		std::string getName();

		std::string getFullyQualifiedName(const char separator = RELOG_LIB_DOMAIN_HPP_DEFAULT_SEPARATOR, bool reverse = false);

		std::partial_ordering compare(DomainProxy& other);

		std::partial_ordering operator<=>(const DomainProxy& other) const;

		std::shared_ptr<Domain> unwrap();
};

/**
 * @class DomainOwner
 * @brief Convenience wrapper for `Domain` class functions
 * @author Gleb Salmanov
 *
 * A wrapper for the functions of the `Domain` class made for convenience,
 * which takes ownership of the root domain nodes.
 *
 * Can create and hold multiple root domain nodes, which are deallocated
 * with their respective trees when the `DomainOwner` is.
 *
 * Features a single `get` function which gets the nodes from the trees
 * or creates them as needed.
 */
class DomainOwner
{
		std::map<std::string, std::shared_ptr<Domain>> m_domains;

	public:
		DomainOwner();

		virtual ~DomainOwner();

		template<typename... strings>
		DomainProxy get(std::string name, strings... names);

	protected:
		template<typename... strings>
		std::shared_ptr<Domain> getImpl_(std::shared_ptr<Domain> current, std::string name, strings... names);

		std::shared_ptr<Domain> getImpl_(std::shared_ptr<Domain> current);
};

} // namespace relog::lib

#endif
