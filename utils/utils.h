#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <utility>

enum class Status { kSuccess, kFailure };

#define DISABLE_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName&) = delete;  \
    ClassName& operator=(const ClassName&) = delete;

#define DISABLE_CONSTRUCTION(ClassName)   \
    ClassName() = delete;                 \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define DISABLE_MOVE(ClassName)      \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

namespace myUtils {
    template <typename Func, typename... Args>
    inline void measure_time(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        std::forward<Func>(func)(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        double milliseconds = duration.count() / 1e6;
        if (milliseconds < 1000) {
            std::cout << std::scientific << std::setprecision(2) << milliseconds
                      << " ms" << std::endl;
        } else {
            std::cout << std::scientific << std::setprecision(2)
                      << milliseconds / 1000 << " s" << std::endl;
        }
    }

}  // namespace myUtils