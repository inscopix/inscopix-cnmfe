#include <iostream>
#include "isx/cnmfe.h"

int main(int argc, char **argv) {
	Cnmfe cnmfe;
	std::string status = cnmfe.runCnmfe();
	std::cout << "CNMFe status: " << status << '\n';
	return 0;
}
