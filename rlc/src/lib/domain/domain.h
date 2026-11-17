#ifndef RELOG_LIB_DOMAIN_DOMAIN_H
#define RELOG_LIB_DOMAIN_DOMAIN_H

namespace relog::lib
{

template<typename TSelf>
class Domain;

template<typename TSelf, typename TDomain>
class DomainProxy;

template<typename TDomainProxy, typename TDomain>
class DomainOwner;

} // namespace relog::lib

#endif
