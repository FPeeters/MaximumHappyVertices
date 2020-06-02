#include <cstdlib>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>
#include "linear_int_distribution.h"

int main(int argc, char **argv) {
    double alpha = strtod(argv[1], nullptr);
    int seed = (int) strtol(argv[2], nullptr, 10);

    std::vector<unsigned int> buckets(50, 0);

    std::default_random_engine engine(seed);

    for (int i = 0; i < 10000; ++i) {
        linear_int_distribution<unsigned int> distribution(alpha, 0, 50);
        ++buckets[distribution(engine)];
    }

    unsigned int max = *std::max_element(buckets.begin(), buckets.end());

    for (unsigned int item: buckets) {
        for (unsigned int i = 0; i < item * 50 / max; ++i)
            std::cout << "#";
        std::cout << std::endl;
    }
}
