#include "bootstrap.hpp"

#include <bootstrap/opts.cpp>

int
main(int argc, char* argv[])
{
	relog::bootstrap::parse_opts(argc, argv);
	return 0;
}
