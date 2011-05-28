#include <string>
#include <iostream>
#include <ostream>
#include <istream>
#include <sstream>

#include "om.hpp"

#include "proc.hpp"

using namespace TeamWobble::Icfpc::Om;

int main(int argc, char **argv) {
	std::cout << "om v2" << std::endl;

	if (argc != 2) {
		std::cout << std::endl <<
				"incorrect number of command line arguments" <<
				" -- please specify scenario id" << std::endl;
	}
	else {
		std::istringstream argStream1(argv[1]);
		Word scenarioId;
		argStream1 >> scenarioId;
	
		Proc prc(scenarioId, true);
	
		prc.init();
		prc.run();
	}

	std::cout << std::endl << "all done." << std::endl;
}
