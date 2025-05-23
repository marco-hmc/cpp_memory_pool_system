#include <benchmark/benchmark.h>

#include <thread>
#include <vector>

#include "MemoryPool.h"

namespace {
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
}  // namespace

static void BenchmarkMemoryPool(benchmark::State& state) {
    size_t nTimes = state.range(0);
    size_t nWorks = state.range(1);
    size_t rounds = state.range(2);

    for (auto _ : state) {
        std::vector<std::thread> vThread(nWorks);

        for (size_t k = 0; k < nWorks; ++k) {
            vThread[k] = std::thread([&]() {
                for (size_t j = 0; j < rounds; ++j) {
                    for (size_t i = 0; i < nTimes; i++) {
                        P1* p1 = Mp1::newElement<P1>();
                        Mp1::deleteElement<P1>(p1);
                        P2* p2 = Mp1::newElement<P2>();
                        Mp1::deleteElement<P2>(p2);
                        P3* p3 = Mp1::newElement<P3>();
                        Mp1::deleteElement<P3>(p3);
                        P4* p4 = Mp1::newElement<P4>();
                        Mp1::deleteElement<P4>(p4);
                    }
                }
            });
        }
        for (auto& t : vThread) {
            t.join();
        }
    }
}

static void BenchmarkNew(benchmark::State& state) {
    size_t nTimes = state.range(0);
    size_t nWorks = state.range(1);
    size_t rounds = state.range(2);

    for (auto _ : state) {
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
}

BENCHMARK(BenchmarkMemoryPool)->Args({100, 1, 10});
BENCHMARK(BenchmarkNew)->Args({100, 1, 10});

BENCHMARK_MAIN();
