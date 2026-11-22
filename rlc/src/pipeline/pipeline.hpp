#ifndef RELOG_PIPELINE_PIPELINE_HPP
#define RELOG_PIPELINE_PIPELINE_HPP

#include <bootstrap/bootstrap.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>
#include <service/source/source_stack.hpp>

namespace relog::pipeline
{

class Pipeline final
{
	protected:
		Pipeline& self;

	public:
		relog::service::source::SourceStackController& ssc;

	public:
		static relog::service::logging::LogDomainOwner<std::string>&
		get_domain_owner()
		{
			return relog::bootstrap::logging::get_domain_owner();
		}

		static relog::service::logging::LogDomainProxy<std::string>&
		get_domain()
		{
			static relog::service::logging::LogDomainProxy<std::string> domain_ = get_domain_owner().get("pipeline");
			return domain_;
		}

		static relog::service::logging::Loggerf&
		get_logger()
		{
			static relog::service::logging::Loggerf _logger{get_domain(), relog::service::logging::LogLevel::INFO};
			return _logger;
		}

	protected:
		relog::service::logging::LogDomainOwner<std::string>& domain_owner = get_domain_owner();
		relog::service::logging::LogDomainProxy<std::string>& domain = get_domain();
		relog::service::logging::Loggerf& l = get_logger();

	public:
		Pipeline(relog::service::source::SourceStackController& ssc_);

		void init_logging();

		int entrypoint();
};

} // namespace relog::pipeline

#endif
