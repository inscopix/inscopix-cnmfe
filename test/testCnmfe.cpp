#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "cnmfe/cnmfe.h"

TEST_CASE("CNMFe completes successfully", "[cnmfe]") {
	Cnmfe cnmfe;
	std::string actualStatus = cnmfe.runCnmfe();
	const std::string expectedStatus = "CNMFe ran successfully";
	REQUIRE(actualStatus == expectedStatus);
}
