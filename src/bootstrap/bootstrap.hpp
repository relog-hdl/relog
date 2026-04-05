#ifndef RELOG_BOOTSTRAP_BOOTSTRAP_HPP
#define RELOG_BOOTSTRAP_BOOTSTRAP_HPP

#include <service/logging/log_domain.hpp>
#include <service/logging/loggerf.hpp>

#include <string>

namespace relog::bootstrap
{

extern relog::service::logging::LogDomainOwner<std::string> d;
extern relog::service::logging::LogDomainProxy<std::string> r;
extern relog::service::logging::Loggerf l;

void
init_logging();

} // namespace relog::bootstrap

#endif
