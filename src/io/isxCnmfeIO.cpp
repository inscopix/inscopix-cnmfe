#include "isxCnmfeIO.h"
#include "isxCnmfeParams.h"
#include "isxExportTiff.h"
#include "isxUtilities.h"
#include "isxLog.h"
#include "json.hpp"
#include <fstream>
#include <iomanip>

namespace isx
{
    void saveOutputToH5File(
        const CubeFloat_t & footprints,
        const MatrixFloat_t & traces,
        const std::string & outputFilename,
        const std::string & footprintsKey,
        const std::string & tracesKey)
    {
        ISX_LOG_INFO("Saving components to h5 file (file: ", outputFilename, ")");
        // data is transposed since armadillo elements are stored in column-major ordering
        footprints.save(arma::hdf5_name(outputFilename, footprintsKey, arma::hdf5_opts::trans + arma::hdf5_opts::append));
        traces.save(arma::hdf5_name(outputFilename, tracesKey, arma::hdf5_opts::trans + arma::hdf5_opts::append));
    }

    std::string convertNumberToPaddedString(const size_t inNumber, const size_t inWidth)
    {
        std::stringstream ss;
        ss.width(inWidth);
        ss.fill('0');
        ss << inNumber;
        return ss.str();
    }

    void writeHeaders(
        std::ofstream & inStream,
        const std::string & cellNamePrefix,
        const size_t numCells)
    {
        size_t width = (numCells > 10) ? (size_t(std::floor(std::log10(numCells - 1)) + 1)) : (1);
        inStream << "Frame";
        for (size_t cellId(0); cellId < numCells; cellId++)
        {
            inStream << "," << cellNamePrefix << convertNumberToPaddedString(cellId, width);
        }
        inStream << std::endl;
    }

    void writeTraces(
        std::ofstream & inStream,
        const MatrixFloat_t & inTraces)
    {
        const int32_t maxDecimalsForFloat = std::numeric_limits<float>::digits10 + 1;
        for (size_t timepoint(0); timepoint < inTraces.n_cols; timepoint++)
        {
            inStream << timepoint;
            for (size_t cellId(0); cellId < inTraces.n_rows; cellId++)
            {
                inStream << "," << std::setprecision(maxDecimalsForFloat) << inTraces(cellId, timepoint);
            }
            inStream << std::endl;
        }
    }

    void saveTracesToCSVFile(
        const MatrixFloat_t & traces,
        const std::string & outputFilename,
        const std::string & cellNamePrefix)
    {
        if (traces.n_rows > 0)
        {
            ISX_LOG_INFO("Saving temporal traces to csv file (file: ", outputFilename, ")");
            std::ofstream outFile(outputFilename);
            writeHeaders(outFile, cellNamePrefix, traces.n_rows);
            writeTraces(outFile, traces);
            outFile.close();
        }
    }

    void saveFootprintsToTiffFile(
        const CubeFloat_t & footprints,
        const std::string & outputFilename)
    {
        ISX_LOG_INFO("Saving footprints to tiff file (file: ", outputFilename, ")");
        std::unique_ptr<TiffExporter> out(new TiffExporter(outputFilename, true));
        for (size_t i = 0; i < footprints.n_slices; ++i)
        {
            out->toTiffOut(footprints.slice(i));
            out->nextTiffDir();
        }
    }

    std::string getMemoryMapDirPath(
        const std::string & inputMoviePath,
        const std::string & outputDir,
        const int processingMode,
        const int patchSize,
        const int patchOverlap)
    {
        nlohmann::json params;
        params["absoluteFilepath"] = inputMoviePath;
        if (static_cast<isx::CnmfeMode_t>(processingMode) != CnmfeMode_t::ALL_IN_MEMORY)
        {
            // patch size and patch overlap don't matter for all-in-memory mode
            params["patchSize"] = patchSize;
            params["patchOverlap"] = patchOverlap;
        }

        // construct unique hash based on input parameters to uniquely identify memory map files
        std::stringstream id;
        id << std::hex << std::hash<std::string>{}(params.dump());
        return outputDir + "/" + getBaseName(inputMoviePath) + "-" + id.str();
    }

    std::string getH5OutputFilename(const std::string & inputMoviePath, const std::string & outputDir)
    {
        return outputDir + "/" + getBaseName(inputMoviePath) + "_output.h5";
    }

    std::string getFootprintsOutputFilename(const std::string & inputMoviePath, const std::string & outputDir)
    {
        return outputDir + "/" + getBaseName(inputMoviePath) + "_footprints.tiff";
    }

    std::string getTracesOutputFilename(const std::string & inputMoviePath, const std::string & outputDir)
    {
        return outputDir + "/" + getBaseName(inputMoviePath) + "_traces.csv";
    }
}
