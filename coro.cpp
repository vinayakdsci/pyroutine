#include "coro.h"
#include <chrono>

using sclock = std::chrono::high_resolution_clock;
using time_point = std::chrono::time_point<sclock>;
using ms = std::chrono::milliseconds;

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

Sleep sleep_(double time) {
  const auto start = sclock::now();
  for (unsigned i = 0;; ++i) {
    auto now = decltype(start)::clock::now();
    // auto duration = ms{time};
    struct args arguments = {
        .now = now,
        .start = start,
        .duration = time,
    };

    co_yield arguments;
  }
}

int main() {
  auto generator = getGenericGenerator();
  auto sleeper = sleep_(0.09);

  for (int32_t i = 0; i < 200; ++i) {
    std::cout << "Counter: " << generator() << "\n";
    if (sleeper()) {
      std::cout << "Lapsed time\n";
      break;
    }
  }
  return 0;
}