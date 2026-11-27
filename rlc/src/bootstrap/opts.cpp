#include "opts.hpp"

#include <bootstrap/bootstrap.hpp>

#include <service/logging/log_domain.hxx>
#include <service/logging/loggerf.hpp>

#include <getopt.h>

#include <vector>

namespace relog::bootstrap::opts
{

relog::service::logging::LogDomainProxy<std::string>& domain = logging::get_domain();
relog::service::logging::Loggerf& l = logging::get_logger();

bool preprocess = false;

// clang-format off
const_cstring optstring = "E";
struct option longopts[] = {
	{ RELOG_OPTS_PREPROCESS_NAME, no_argument, reinterpret_cast<int*>(&preprocess), true },
	{ 0, 0, 0, 0 }
};
// clang-format on

ParseOptsResult
parse_opts(std::vector<char*>& nonoptargs, int argc, char* argv[])
{
	int option, option_index;

	bool cont = false;
	bool arg_reparse = false;

	char** optarg_override = NULL;
	unsigned int optarg_override_length = 0;
	int last = 0;

	std::string long_option_name;

	while(true)
	{
		while(true)
		{
			option_index = 0;
			opterr = 0;

			option = getopt_long(argc, argv, optstring, longopts, &option_index);

			if(option == -1)
				break;

			l.info("Parsing option: %c (%d)", option, option);

			switch(option)
			{
				case 0:
					l.info("Parsing long option: %s", longopts[option_index].name);
					if(longopts[option_index].has_arg != no_argument)
						arg_reparse = true;
					if(longopts[option_index].flag != NULL)
					{
						cont = true;
						break;
					}

					long_option_name = longopts[option_index].name;

					break;
				case 'E':
					preprocess = true;
					cont = true;
					break;
				case '?':
					l.error("Argument error while parsing option: %d", option);
					return ARGUMENT_ERROR;
				default:
					l.error("Unknown error while parsing option: %d\n(Option functionality likely not implemented)", option);
					return UNKNOWN_ERROR;
				case 1:
					goto parse_non_option_args;
			}

			if(cont)
			{
				cont = false;
				continue;
			}

			if(arg_reparse)
			{
				if(optarg == NULL)
				{
					last = optind - 1;
					for(int i = optind; i < argc; i++)
						if(argv[i][0] == '-')
							break;
						else
							last = i;
					optarg_override_length = last - optind + 1;
					optarg_override = reinterpret_cast<char**>(malloc(sizeof(char*) * optarg_override_length));
					for(int i = optind; i <= last; i++)
						optarg_override[i - optind] = argv[i];
				}
				else
				{
					last = optind - 1;
					for(int i = optind; i < argc; i++)
						if(argv[i][0] == '-')
							break;
						else
							last = i;
					optarg_override_length = last - optind + 2;
					optarg_override = reinterpret_cast<char**>(malloc(sizeof(char*) * optarg_override_length));
					optarg_override[0] = optarg;
					for(int i = optind; i <= last; i++)
						optarg_override[i - optind + 1] = argv[i];
				}

				for(unsigned int i = 0; i < optarg_override_length; i++)
					l.info("Parsed argument %u: '%s'", i + 1, optarg_override[i]);
			}

			free(optarg_override);
			optarg_override = NULL;
			optarg_override_length = 0;

			arg_reparse = false;
		}

parse_non_option_args:
		if(optind >= argc)
			return OK;

		nonoptargs.push_back(argv[optind]);

		argc -= optind;
		argv += optind;
	}
}

} // namespace relog::bootstrap::opts
