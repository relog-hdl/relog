#ifndef RELOG_OPTS_HPP
#define RELOG_OPTS_HPP

#include <aliases.hpp>

#include <getopt.h>

namespace relog::conf
{

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

} // namespace relog::conf

#endif
