#include "domain.hpp"

#include <list>
#include <map>
#include <memory>
#include <string>

namespace relog::lib
{

Domain::Domain(std::weak_ptr<Domain> parent, std::string name)
{
	m_is_root = false;

	m_parent = parent;
	m_name = name;
}

Domain::~Domain() {}

std::shared_ptr<Domain>
Domain::createRoot(std::string name)
{
	Domain* domain = new Domain(std::weak_ptr<Domain>(), name);
	domain->m_is_root = true;

	std::shared_ptr<Domain> domain_ptr = std::shared_ptr<Domain>(domain);
	domain->m_self = domain_ptr;
	domain->m_root = domain_ptr;

	return domain_ptr;
}

std::shared_ptr<Domain>
Domain::getOrCreateChild(std::string name)
{
	if(m_children.contains(name))
		return m_children[name];

	Domain* domain = new Domain(m_self, name);
	domain->m_root = m_root;

	std::shared_ptr<Domain> domain_ptr = std::shared_ptr<Domain>(domain);
	domain->m_self = domain_ptr;

	m_children[name] = domain_ptr;

	return domain_ptr;
}

bool
Domain::isRoot()
{
	return m_is_root;
}

std::weak_ptr<Domain>
Domain::getRoot()
{
	return m_root;
}

std::weak_ptr<Domain>
Domain::getParent()
{
	return m_parent;
}

std::list<std::weak_ptr<Domain>>
Domain::getChain()
{
	std::list<std::weak_ptr<Domain>> chain;

	chain.push_front(m_self);

	std::shared_ptr<Domain> parent = m_parent.lock();
	while(!parent->isRoot())
	{
		chain.push_front(std::weak_ptr(parent));
		parent = parent->getParent().lock();
	}

	chain.push_front(std::weak_ptr(parent));

	return chain;
}

std::string
Domain::getName()
{
	return m_name;
}

std::string
Domain::getFullyQualifiedName(const char separator, bool reverse)
{
	if(this->isRoot())
		return m_name;

	std::shared_ptr<Domain> parent = m_parent.lock();
	std::string parent_name = parent->getFullyQualifiedName(separator, reverse);

	if(reverse)
		return m_name + separator + parent_name;
	else
		return parent_name + separator + m_name;
}

std::partial_ordering
Domain::compare(Domain* other)
{
	if(this == other)
		return std::partial_ordering::equivalent;

	if(this->m_name == other->m_name)
	{
		if(this->m_is_root && other->m_is_root)
			return std::partial_ordering::equivalent;

		std::shared_ptr<Domain> this_parent = m_parent.lock();
		std::shared_ptr<Domain> other_parent = other->m_parent.lock();

		if(this_parent == other_parent)
			return std::partial_ordering::equivalent;
	}

	std::list<std::weak_ptr<Domain>> this_chain = this->getChain();
	std::list<std::weak_ptr<Domain>> other_chain = other->getChain();

	std::shared_ptr<Domain> this_chain_current;
	std::shared_ptr<Domain> other_chain_current;

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

std::partial_ordering
Domain::compare(Domain& other)
{
	return this->compare(&other);
}

std::partial_ordering
Domain::compare(std::shared_ptr<Domain> other)
{
	return this->compare(other.get());
}

std::partial_ordering
Domain::operator<=>(const Domain& other) const
{
	return const_cast<Domain*>(this)->compare(const_cast<Domain&>(other));
}

DomainOwner::DomainOwner() {}

DomainOwner::~DomainOwner() {}

template<typename... strings>
std::shared_ptr<Domain>
DomainOwner::get(std::string name, strings... names)
{
	if(!m_domains.contains(name))
		m_domains[name] = Domain::createRoot(name);

	std::shared_ptr<Domain> root = m_domains[name];

	return getImpl_(root, names...);
}

template<typename... strings>
std::shared_ptr<Domain>
DomainOwner::getImpl_(std::shared_ptr<Domain> current, std::string name, strings... names)
{
	return getImpl_(current->getOrCreateChild(name), names...);
}

std::shared_ptr<Domain>
DomainOwner::getImpl_(std::shared_ptr<Domain> current)
{
	return current;
}

} // namespace relog::lib
