#include "coro.h"

/// Produces an infinite stream of numbers.
Generator getGenerator() {
    for (uint64_t i = 0;; ++i)
        // Equivalent to `co_await promise.yield_value();`
        co_yield i;
}

GenericGenerator<int32_t> getGenericGenerator() {
  for (int32_t i = 0;;) {
    co_yield i++;
  }
}

int main() {
  auto generator = getGenericGenerator();
  for (int32_t i = 0; i < 20; ++i) {
    std::cout << "Counter: " << generator() << "\n";
  }
  return 0;
}