#pragma once

#include <utility>

class MemoryPool {
  public:
    MemoryPool() {}

    void myFunction() {}
};

template <typename T, typename... Args>
T* newElement(Args&&... args) {
    T* element = new T(std::forward<Args>(args)...);
    return element;
}

template <typename T>
void deleteElement(T* element) {
    delete element;
}