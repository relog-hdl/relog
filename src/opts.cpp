#include "opts.hpp"

#include <getopt.h>

#include <cstdio>
#include <cstdlib>

namespace relog::conf
{

// clang-format off
const_cstring optstring = "d";
struct option longopts[] = {
	{ "debug", no_argument, NULL, 0 },
	{ 0, 0, 0, 0 }
};
// clang-format on

ParseOptsResult
parse_opts(int argc, char* argv[])
{
	int option, option_index;

	bool cont = false;
	bool arg_reparse = false;

	char** optarg_override = NULL;
	unsigned int optarg_override_length = 0;
	int last = 0;

	while(true)
	{
		option_index = 0;
		opterr = 0;

		option = getopt_long(argc, argv, optstring, longopts, &option_index);

		if(option == -1)
			break;

		printf("Parsing option: %c (%d)\n", option, option);

		switch(option)
		{
			case 0:
				printf("Parsing long option: %s\n", longopts[option_index].name);
				if(longopts[option_index].has_arg != no_argument)
					arg_reparse = true;
				if(longopts[option_index].flag != NULL)
				{
					cont = true;
					break;
				}

				break;
			case 'd':
				cont = true;
				break;
			case '?':
				printf("Argument error while parsing option: %d\n", option);
				return ARGUMENT_ERROR;
			default:
				printf("Unknown error while parsing option: %d\n(Option functionality likely not implemented)\n", option);
				return UNKNOWN_ERROR;
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
				printf("Parsed argument %u: '%s'\n", i + 1, optarg_override[i]);
		}

		free(optarg_override);
		optarg_override = NULL;
		optarg_override_length = 0;

		arg_reparse = false;
	}

	return OK;
}

} // namespace relog::conf
