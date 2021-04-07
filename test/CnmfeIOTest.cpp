#include "isxCnmfeIO.h"
#include "isxTest.h"
#include "catch.hpp"
#include <iostream>

TEST_CASE("SaveCNMFeOutputToH5", "[cnmfe-io]")
{
    const std::string outputFilename = "acakm180a1naubn179nspqic98palanqy.h5";
    const std::string footprintsKey = "footprints";
    const std::string tracesKey = "traces";

    SECTION("3 cells, 5 time points, 4x6 pixels FOV")
    {
        isx::CubeFloat_t expectedFootprints(4, 6, 3);
        expectedFootprints.slice(0) = {
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f},
            {19.8f, 10.1f, 1.0f, 0.0f, 1.0f, 2.0f},
            {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
            {0.1f, 78.9f, 10.9f, 18.17f, 0.34f, 9.1f}
        };
        expectedFootprints.slice(1) = {
            {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
        };
        expectedFootprints.slice(2) = {
            {1.9f, 2.9f, 3.9f, 4.9f, 5.9f, 6.9f},
            {1.01f, 1.02f, 1.03f, 1.04f, -0.1f, 0.5f},
            {-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
            {9.1f, 8.5f, 6.7f, 8.3f, 4.1f, 5.1f}
        };

        const isx::MatrixFloat_t expectedTraces = {
            {0.74f, 0.9f, 1.2f, 10.12f, 5.6f},
            {100.8f, 187.13f, 200.01f, 150.88f, 99.99f},
            {5.901f, 1.43f, 3.72f, 1.11f, 2.22f}
        };

        isx::saveOutputToH5File(expectedFootprints, expectedTraces, outputFilename, footprintsKey, tracesKey);

        isx::CubeFloat_t actualFootprints;
        isx::MatrixFloat_t actualTraces;
        actualFootprints.load(arma::hdf5_name(outputFilename, footprintsKey, arma::hdf5_opts::trans));
        actualTraces.load(arma::hdf5_name(outputFilename, tracesKey, arma::hdf5_opts::trans));

        REQUIRE(arma::approx_equal(actualFootprints, expectedFootprints, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(actualTraces, expectedTraces, "reldiff", 1e-5f));
    }

    std::remove(outputFilename.c_str());
}

TEST_CASE("SaveCNMFeTracesToCSV", "[cnmfe-io]")
{
    const std::string outputFilename = "coanciunw.18qsnma2.auna.csv";

    SECTION("2 traces, 3 time points")
    {
        const isx::MatrixFloat_t expectedTraces = {
            {0.74f, 0.9f, 1.2f},
            {100.8f, 187.13f, 200.01f},
        };

        isx::saveTracesToCSVFile(expectedTraces, outputFilename);

        isx::MatrixFloat_t actualTraces;
        std::ifstream csvFileRead(outputFilename);
        REQUIRE(csvFileRead.is_open());

        std::string line;
        getline(csvFileRead, line);
        REQUIRE(line == "Frame,C0,C1");
        getline(csvFileRead, line);
        REQUIRE(line == "0,0.74,100.8");
        getline(csvFileRead, line);
        REQUIRE(line == "1,0.9,187.13");
        getline(csvFileRead, line);
        REQUIRE(line == "2,1.2,200.01");

        csvFileRead.close();
    }

    std::remove(outputFilename.c_str());
}

TEST_CASE("SaveCNMFeFootprintsToTiff", "[cnmfe-io]")
{
    const std::string outputFilename = "cnaaiucnaw279aiodnuin8q1.tiff";

    SECTION("3 cells, 4x6 pixels FOV")
    {
        isx::CubeFloat_t expectedFootprints(4, 6, 3);
        expectedFootprints.slice(0) = {
            {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f},
            {19.8f, 10.1f, 1.0f, 0.0f, 1.0f, 2.0f},
            {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
            {0.1f, 78.9f, 10.9f, 18.17f, 0.34f, 9.1f}
        };
        expectedFootprints.slice(1) = {
            {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
        };
        expectedFootprints.slice(2) = {
            {1.9f, 2.9f, 3.9f, 4.9f, 5.9f, 6.9f},
            {1.01f, 1.02f, 1.03f, 1.04f, -0.1f, 0.5f},
            {-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
            {9.1f, 8.5f, 6.7f, 8.3f, 4.1f, 5.1f}
        };

        isx::saveFootprintsToTiffFile(expectedFootprints, outputFilename);

        // TODO: validate output tiff file
    }

    std::remove(outputFilename.c_str());
}
