#include <iostream>
#include <thread>
#include <vector>

#include "MemoryPool.h"
#include "utils.h"

class P1 {
    int id_;
};

class P2 {
    int id_[5];
};

class P3 {
    int id_[10];
};

class P4 {
    int id_[20];
};

void BenchmarkMemoryPool(size_t nTimes, size_t nWorks, size_t rounds) {
    std::vector<std::thread> vThread(nWorks);

    for (size_t k = 0; k < nWorks; ++k) {
        vThread[k] = std::thread([&]() {
            for (size_t j = 0; j < rounds; ++j) {
                for (size_t i = 0; i < nTimes; i++) {
                    P1* p1 = newElement<P1>();
                    deleteElement<P1>(p1);
                    P2* p2 = newElement<P2>();
                    deleteElement<P2>(p2);
                    P3* p3 = newElement<P3>();
                    deleteElement<P3>(p3);
                    P4* p4 = newElement<P4>();
                    deleteElement<P4>(p4);
                }
            }
        });
    }
    for (auto& t : vThread) {
        t.join();
    }
}

void BenchmarkNew(size_t nTimes, size_t nWorks, size_t rounds) {
    std::vector<std::thread> vThread(nWorks);

    for (size_t k = 0; k < nWorks; ++k) {
        vThread[k] = std::thread([&]() {
            for (size_t j = 0; j < rounds; ++j) {
                for (size_t i = 0; i < nTimes; i++) {
                    P1* p1 = new P1;
                    delete p1;
                    P2* p2 = new P2;
                    delete p2;
                    P3* p3 = new P3;
                    delete p3;
                    P4* p4 = new P4;
                    delete p4;
                }
            }
        });
    }
    for (auto& t : vThread) {
        t.join();
    }
}

int main() {
    myUtils::measure_time(BenchmarkMemoryPool, 100, 1, 10);
    std::cout << "============================================================="
                 "=============="
              << std::endl;
    myUtils::measure_time(BenchmarkNew, 100, 1, 10);

    return 0;
}