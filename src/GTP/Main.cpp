#include <iostream>
#include <string>

#include "GTPEngine.h"

int main(int argc, char* argv[]) {
	double thinkingTime = 0.0;
	for (int i = 1; i < argc; ++i) {
		if (std::string(argv[1]) == "-t") {
			thinkingTime = std::stod(argv[2]);
		}
	}
	if (thinkingTime <= 0.0) {
		GTPEngine e;
		e.Start();
	} else {
		GTPEngine e(thinkingTime);
		e.Start();
	}

	return 0;
}

/*
 * Inputs can be in one of four formats:
 * id command_name arguments
 * id command_name
 * command_name arguments
 * command_name
 *
 * Required inputs are:
 * protocol_version
 * name
 *
 *
 * The program may output anything, but only lines starting with an = or a ? should be interpreted.
 * If the response was successful, start with =, otherwise (such as bad input) start with ? and
 * print some error message.
 */