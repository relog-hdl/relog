#ifndef RELOG_BOOTSTRAP_BOOTSTRAP_HPP
#define RELOG_BOOTSTRAP_BOOTSTRAP_HPP

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

#include <string>

namespace relog::bootstrap
{

extern relog::service::logging::LogDomainOwner<std::string>& domain_owner;
extern relog::service::logging::LogDomainProxy<std::string>& domain;
extern relog::service::logging::Loggerf& l;

struct logging
{
		static relog::service::logging::LogDomainOwner<std::string>&
		get_domain_owner()
		{
			static relog::service::logging::LogDomainOwner<std::string> domain_owner_{};
			return domain_owner_;
		}

		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = get_domain_owner().get("bootstrap");
			return domain_;
		}

		static relog::service::logging::Loggerf&
		get_logger()
		{
			static relog::service::logging::Loggerf _logger{get_domain(), relog::service::logging::LogLevel::INFO};
			return _logger;
		}
};

void
init_logging();

} // namespace relog::bootstrap

#endif
