#include "catch.hpp"
#include "ThreadPool.h"
#include "H5Cpp.h"
#include <armadillo>
#include "opencv2/core.hpp"
#include <QFile>
namespace libtiff {
    // placed in its own namespace to avoid
    // type redefinition conflict with OpenCV
    #include "tiffio.h"
}

void add(int a, int b, int & sum)
{
    sum = a + b;
}

TEST_CASE("ThreadPoolDependency", "[cnmfe-dependencies]")
{
    std::vector<std::future<void>> results(3);
    std::vector<int> sums {-1, -1, -1};

    ThreadPool pool(2);
    results[0] = pool.enqueue(add, 0, 1, std::ref(sums[0]));
    results[1] = pool.enqueue(add, -100, 100, std::ref(sums[1]));
    results[2] = pool.enqueue(add, 9, 5, std::ref(sums[2]));

    results[0].get();
    results[1].get();
    results[2].get();

    REQUIRE(sums[0] == 1);
    REQUIRE(sums[1] == 0);
    REQUIRE(sums[2] == 14);
}

TEST_CASE("HDF5Dependency", "[cnmfe-dependencies]")
{
    const std::string expectedFilename = "acmqn170a1naubn179nspqicoanqy.h5";
    H5::H5File h5file(expectedFilename, H5F_ACC_TRUNC);
    const std::string actualFilename = h5file.getFileName();
    h5file.close();
    REQUIRE(actualFilename == expectedFilename);
    std::remove(expectedFilename.c_str());
}

TEST_CASE("ArmadilloDependency", "[cnmfe-dependencies]")
{
    const arma::Col<float> a = {41.85f, 98.17f, -68.76f};
    const arma::Col<float> b = {0.01f, 14.5f, 1.098f};
    const arma::Col<float> expectedSum = {41.86f, 112.67f, -67.662f};

    arma::Col<float> actualSum = a + b;

    REQUIRE(arma::approx_equal(actualSum, expectedSum, "reldiff", 1e-5f));
}

TEST_CASE("OpenCVDependency", "[cnmfe-dependencies]")
{
    float data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    cv::Mat dataMat = cv::Mat(2, 5, CV_32F, data);

    const double expectedSum = 55;
    double actualSum = cv::sum(dataMat)[0];

    REQUIRE(actualSum == expectedSum);
}

TEST_CASE("LibTiffDependency", "[cnmfe-dependencies]")
{
    void * buf = libtiff::_TIFFmalloc(size_t(10));
    libtiff::_TIFFfree(buf);
    REQUIRE(TIFFTAG_IMAGEWIDTH == 256);

    const char * mode = "w";
    const std::string filename = "myfile_X9Fa1nahda8had.tiff";
    libtiff::TIFF * tiffFile = libtiff::TIFFOpen(filename.c_str(), mode);
    REQUIRE(tiffFile != nullptr);
    TIFFClose(tiffFile);
    std::remove(filename.c_str());
}

TEST_CASE("QtDependency", "[cnmfe-dependencies]")
{
    const std::string filename = "dsahkafafbjcvha.bin";
    const char * msg = "hello";

    QFile file(QString::fromStdString(filename));
    bool success = file.open(QIODevice::ReadWrite);
    REQUIRE(success);

    auto numBytes = qstrlen(msg);
    auto bytesWritten = file.write(msg, numBytes);
    REQUIRE(bytesWritten == numBytes);

    file.close();
    std::remove(filename.c_str());
}
