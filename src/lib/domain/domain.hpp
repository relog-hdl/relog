#ifndef RELOG_LIB_DOMAIN_DOMAIN_HPP
#define RELOG_LIB_DOMAIN_DOMAIN_HPP

#include <list>
#include <map>
#include <memory>
#include <string>

#include <util/derived.hpp>

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
template<typename TSelf>
class Domain
{
	protected:
		bool m_is_root = false;
		std::weak_ptr<TSelf> m_self;
		std::weak_ptr<TSelf> m_root;
		std::weak_ptr<TSelf> m_parent;
		std::map<std::string, std::shared_ptr<TSelf>> m_children;
		std::string m_name;

	public:
		virtual ~Domain();

		static std::shared_ptr<TSelf> createRoot(std::string name);

		std::shared_ptr<TSelf> getOrCreateChild(std::string name);

		template<typename... strings>
		std::shared_ptr<TSelf> getOrCreateChildren(std::string name, strings... names);

		bool isRoot();

		std::weak_ptr<TSelf> getRoot();

		std::weak_ptr<TSelf> getParent();

		std::list<std::weak_ptr<TSelf>> getChain();

		std::string getName();

		std::string getFullyQualifiedName(const char separator = RELOG_LIB_DOMAIN_HPP_DEFAULT_SEPARATOR, bool reverse = false);

		std::partial_ordering compare(TSelf* other);

		std::partial_ordering compare(TSelf& other);

		std::partial_ordering compare(std::shared_ptr<TSelf> other);

		std::partial_ordering operator<=>(const TSelf&) const;

	protected:
		Domain(std::weak_ptr<TSelf> parent, std::string name);

		template<typename... strings>
		std::shared_ptr<TSelf> getOrCreateChildrenImpl_(std::shared_ptr<TSelf> current, std::string name, strings... names);

		std::shared_ptr<TSelf> getOrCreateChildrenImpl_(std::shared_ptr<TSelf> current);

		virtual void onCreated();

		void onCreatedImpl_();
};

template<typename TSelf, typename TDomain>
class DomainProxy
{
	protected:
		std::shared_ptr<TDomain> m_domain;

	public:
		DomainProxy(std::shared_ptr<TDomain> domain);

		virtual ~DomainProxy();

		template<typename... strings>
		TSelf relative(strings... names);

		TSelf operator/(std::string name);

		bool isRoot();

		TSelf getRoot();

		TSelf getParent();

		std::list<TSelf> getChain();

		std::string getName();

		std::string getFullyQualifiedName(const char separator = RELOG_LIB_DOMAIN_HPP_DEFAULT_SEPARATOR, bool reverse = false);

		std::partial_ordering compare(TSelf& other);

		std::partial_ordering operator<=>(const TSelf& other) const;

		std::shared_ptr<TDomain> unwrap();
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
template<typename TDomainProxy, typename TDomain>
class DomainOwner
{
	protected:
		std::map<std::string, std::shared_ptr<TDomain>> m_domains;

	public:
		DomainOwner();

		virtual ~DomainOwner();

		template<typename... strings>
		TDomainProxy get(std::string name, strings... names);

	protected:
		template<typename... strings>
		std::shared_ptr<TDomain> getImpl_(std::shared_ptr<TDomain> current, std::string name, strings... names);

		std::shared_ptr<TDomain> getImpl_(std::shared_ptr<TDomain> current);
};

class SimpleDomain final : public Domain<SimpleDomain>
{
		using Domain<SimpleDomain>::Domain;
};

class SimpleDomainProxy final : public DomainProxy<SimpleDomainProxy, SimpleDomain>
{
		using DomainProxy<SimpleDomainProxy, SimpleDomain>::DomainProxy;
};

class SimpleDomainOwner final : public DomainOwner<SimpleDomainProxy, SimpleDomain>
{
		using DomainOwner<SimpleDomainProxy, SimpleDomain>::DomainOwner;
};

} // namespace relog::lib

#endif
