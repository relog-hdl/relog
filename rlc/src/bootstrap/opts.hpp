#ifndef RELOG_BOOTSTRAP_OPTS_HPP
#define RELOG_BOOTSTRAP_OPTS_HPP

#include <bootstrap/bootstrap.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

#include <util/aliases.hpp>

#include <getopt.h>

namespace relog::bootstrap::opts
{

extern relog::service::logging::LogDomainProxy<std::string>& domain;
extern relog::service::logging::Loggerf& l;

struct logging
{
		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = relog::bootstrap::logging::get_domain_owner().get("bootstrap", "opts");
			return domain_;
		}

		static relog::service::logging::Loggerf&
		get_logger()
		{
			static relog::service::logging::Loggerf _logger{get_domain(), relog::service::logging::LogLevel::INFO};
			return _logger;
		}
};

typedef enum parse_opts_result
{
	OK,
	ARGUMENT_ERROR,
	UNKNOWN_ERROR
} ParseOptsResult;

extern const_cstring optstring;
extern struct option longopts[];

ParseOptsResult
parse_opts(int argc, char* argv[]);

} // namespace relog::bootstrap::opts

#endif
