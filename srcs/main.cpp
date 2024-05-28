#include "config_parser.hpp"

int main ()
{
	Parser parse("config/default.conf");

	parse.run();
}