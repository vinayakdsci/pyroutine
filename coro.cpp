#include <iostream>

#include "coro.h"


/// Produces an infinite stream of numbers.
Generator getGenerator() {
    for (uint64_t i = 0;; ++i)
        // Equivalent to `co_await promise.yield_value();`
        co_yield i;
}


int main() {
    auto handle = getGenerator().handle_;
    auto &promise = handle.promise();
    
    for(int i = 0; i < 10; ++i) {
        std::cout << "Counter: " << promise.value_ << "\n";
        handle();
    }
    
    // Cleans up the handle's allocated memory.
    handle.destroy();
    return 0;
}