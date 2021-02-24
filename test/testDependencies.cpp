#include "catch.hpp"
#include "ThreadPool.h"

void add(int a, int b, int & sum)
{
    sum = a + b;
}

TEST_CASE("ThreadPoolDependency", "[dependencies]")
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
