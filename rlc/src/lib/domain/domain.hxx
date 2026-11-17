#ifndef RELOG_LIB_DOMAIN_DOMAIN_HXX
#define RELOG_LIB_DOMAIN_DOMAIN_HXX

#include "domain.hpp"

#include <list>
#include <map>
#include <memory>
#include <string>

namespace relog::lib
{

template<typename TSelf>
Domain<TSelf>::Domain(std::weak_ptr<TSelf> parent, std::string name) : m_parent(parent), m_name(name) {}

template<typename TSelf>
std::shared_ptr<TSelf>
Domain<TSelf>::createRoot(std::string name)
{
	TSelf* domain = new TSelf(std::weak_ptr<TSelf>(), name);
	domain->m_is_root = true;

	std::shared_ptr<TSelf> domain_ptr = std::shared_ptr<TSelf>(domain);
	domain->m_self = domain_ptr;
	domain->m_root = domain_ptr;

	domain->onCreatedImpl_();

	return domain_ptr;
}

template<typename TSelf>
std::shared_ptr<TSelf>
Domain<TSelf>::getOrCreateChild(std::string name)
{
	if(m_children.contains(name))
		return m_children[name];

	TSelf* domain = new TSelf(m_self, name);
	domain->m_root = m_root;

	std::shared_ptr<TSelf> domain_ptr = std::shared_ptr<TSelf>(domain);
	domain->m_self = domain_ptr;

	m_children[name] = domain_ptr;

	domain->onCreatedImpl_();

	return domain_ptr;
}

template<typename TSelf>
template<typename... strings>
std::shared_ptr<TSelf>
Domain<TSelf>::getOrCreateChildren(std::string name, strings... names)
{
	return getOrCreateChildrenImpl_(getOrCreateChild(name), names...);
}

template<typename TSelf>
template<typename... strings>
std::shared_ptr<TSelf>
Domain<TSelf>::getOrCreateChildrenImpl_(std::shared_ptr<TSelf> current, std::string name, strings... names)
{
	return getOrCreateChildrenImpl_(current->getOrCreateChild(name), names...);
}

template<typename TSelf>
std::shared_ptr<TSelf>
Domain<TSelf>::getOrCreateChildrenImpl_(std::shared_ptr<TSelf> current)
{
	return current;
}

template<typename TSelf>
void
Domain<TSelf>::onCreated()
{
}

template<typename TSelf>
void
Domain<TSelf>::onCreatedImpl_()
{
	this->onCreated();
}

template<typename TSelf>
bool
Domain<TSelf>::isRoot()
{
	return m_is_root;
}

template<typename TSelf>
std::weak_ptr<TSelf>
Domain<TSelf>::getRoot()
{
	return m_root;
}

template<typename TSelf>
std::weak_ptr<TSelf>
Domain<TSelf>::getParent()
{
	return m_parent;
}

template<typename TSelf>
std::list<std::weak_ptr<TSelf>>
Domain<TSelf>::getChain()
{
	std::list<std::weak_ptr<TSelf>> chain;

	chain.push_front(m_self);

	std::shared_ptr<TSelf> parent = m_parent.lock();
	while(!parent->isRoot())
	{
		chain.push_front(std::weak_ptr(parent));
		parent = parent->getParent().lock();
	}

	chain.push_front(std::weak_ptr(parent));

	return chain;
}

template<typename TSelf>
std::string
Domain<TSelf>::getName()
{
	return m_name;
}

template<typename TSelf>
std::string
Domain<TSelf>::getFullyQualifiedName(const char separator, bool reverse)
{
	if(this->isRoot())
		return m_name;

	std::shared_ptr<TSelf> parent = m_parent.lock();
	std::string parent_name = parent->getFullyQualifiedName(separator, reverse);

	if(reverse)
		return m_name + separator + parent_name;
	else
		return parent_name + separator + m_name;
}

template<typename TSelf>
std::partial_ordering
Domain<TSelf>::compare(TSelf* other)
{
	if(this == other)
		return std::partial_ordering::equivalent;

	if(this->m_name == other->m_name)
	{
		if(this->m_is_root && other->m_is_root)
			return std::partial_ordering::equivalent;

		std::shared_ptr<TSelf> this_parent = m_parent.lock();
		std::shared_ptr<TSelf> other_parent = other->m_parent.lock();

		if(this_parent == other_parent)
			return std::partial_ordering::equivalent;
	}

	std::list<std::weak_ptr<TSelf>> this_chain = this->getChain();
	std::list<std::weak_ptr<TSelf>> other_chain = other->getChain();

	std::shared_ptr<TSelf> this_chain_current;
	std::shared_ptr<TSelf> other_chain_current;

	auto this_chain_it = this_chain.begin();
	auto other_chain_it = other_chain.begin();

	while(this_chain_it != this_chain.end() && other_chain_it != other_chain.end())
	{
		this_chain_current = this_chain_it->lock();
		other_chain_current = other_chain_it->lock();

		if(!((this_chain_current.get() == other_chain_current.get()) || (this_chain_current->getName() == other_chain_current->getName())))
			return std::partial_ordering::unordered;

		++this_chain_it;
		++other_chain_it;
	}

	if((this_chain_it == this_chain.end()) && (other_chain_it != other_chain.end()))
		return std::partial_ordering::greater;
	else if((this_chain_it != this_chain.end()) && (other_chain_it == other_chain.end()))
		return std::partial_ordering::less;
	else
		return std::partial_ordering::equivalent;
}

template<typename TSelf>
std::partial_ordering
Domain<TSelf>::compare(TSelf& other)
{
	return this->compare(&other);
}

template<typename TSelf>
std::partial_ordering
Domain<TSelf>::compare(std::shared_ptr<TSelf> other)
{
	return this->compare(other.get());
}

template<typename TSelf>
std::partial_ordering
Domain<TSelf>::operator<=>(const TSelf& other) const
{
	return const_cast<TSelf*>(this)->compare(const_cast<TSelf&>(other));
}

template<typename TSelf, typename TDomain>
DomainProxy<TSelf, TDomain>::DomainProxy(std::shared_ptr<TDomain> domain) : m_domain(domain) {}

template<typename TSelf, typename TDomain>
template<typename... strings>
TSelf
DomainProxy<TSelf, TDomain>::relative(strings... names)
{
	return TSelf(m_domain->getOrCreateChildren(names...));
}

template<typename TSelf, typename TDomain>
TSelf
DomainProxy<TSelf, TDomain>::operator/(std::string name)
{
	return relative(name);
}

template<typename TSelf, typename TDomain>
bool
DomainProxy<TSelf, TDomain>::isRoot()
{
	return m_domain->isRoot();
}

template<typename TSelf, typename TDomain>
TSelf
DomainProxy<TSelf, TDomain>::getRoot()
{
	return TSelf(m_domain->getRoot().lock());
}

template<typename TSelf, typename TDomain>
TSelf
DomainProxy<TSelf, TDomain>::getParent()
{
	return TSelf(m_domain->getParent().lock());
}

template<typename TSelf, typename TDomain>
std::list<TSelf>
DomainProxy<TSelf, TDomain>::getChain()
{
	std::list<std::weak_ptr<TDomain>> src = m_domain->getChain();

	std::list<TSelf> ret;
	for(auto it = src.begin(); it != src.end(); ++it)
		ret.push_back(TSelf(it->lock()));

	return ret;
}

template<typename TSelf, typename TDomain>
std::string
DomainProxy<TSelf, TDomain>::getName()
{
	return m_domain->getName();
}

template<typename TSelf, typename TDomain>
std::string
DomainProxy<TSelf, TDomain>::getFullyQualifiedName(const char separator, bool reverse)
{
	return m_domain->getFullyQualifiedName(separator, reverse);
}

template<typename TSelf, typename TDomain>
std::partial_ordering
DomainProxy<TSelf, TDomain>::compare(TSelf& other)
{
	return m_domain->compare(other.m_domain);
}

template<typename TSelf, typename TDomain>
std::partial_ordering
DomainProxy<TSelf, TDomain>::operator<=>(const TSelf& other) const
{
	return m_domain->compare(other.m_domain);
}

template<typename TSelf, typename TDomain>
std::shared_ptr<TDomain>
DomainProxy<TSelf, TDomain>::unwrap()
{
	return m_domain;
}

template<typename TDomainProxy, typename TDomain>
DomainOwner<TDomainProxy, TDomain>::DomainOwner() {}

template<typename TDomainProxy, typename TDomain>
template<typename... strings>
TDomainProxy
DomainOwner<TDomainProxy, TDomain>::get(std::string name, strings... names)
{
	if(!m_domains.contains(name))
		m_domains[name] = TDomain::createRoot(name);

	std::shared_ptr<TDomain> root = m_domains[name];

	return TDomainProxy(getImpl_(root, names...));
}

template<typename TDomainProxy, typename TDomain>
template<typename... strings>
std::shared_ptr<TDomain>
DomainOwner<TDomainProxy, TDomain>::getImpl_(std::shared_ptr<TDomain> current, std::string name, strings... names)
{
	return getImpl_(current->getOrCreateChild(name), names...);
}

template<typename TDomainProxy, typename TDomain>
std::shared_ptr<TDomain>
DomainOwner<TDomainProxy, TDomain>::getImpl_(std::shared_ptr<TDomain> current)
{
	return current;
}

} // namespace relog::lib

#endif
