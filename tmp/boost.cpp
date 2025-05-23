#include <boost/pool/pool.hpp>
#include <chrono>
#include <iostream>

const int NUM_ALLOCATIONS = 1000000;
const int ALLOCATION_SIZE = 32;

void default_allocation() {
    for (int i = 0; i < NUM_ALLOCATIONS; ++i) {
        char* p = new char[ALLOCATION_SIZE];
        delete[] p;
    }
}

void boost_pool_allocation() {
    boost::pool<> p(ALLOCATION_SIZE);
    for (int i = 0; i < NUM_ALLOCATIONS; ++i) {
        char* ptr = static_cast<char*>(p.malloc());
        p.free(ptr);
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    default_allocation();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> default_duration = end - start;
    std::cout << "Default allocation time: " << default_duration.count()
              << " seconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    boost_pool_allocation();
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> boost_duration = end - start;
    std::cout << "Boost pool allocation time: " << boost_duration.count()
              << " seconds" << std::endl;

    return 0;
}