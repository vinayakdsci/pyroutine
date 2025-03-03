#include <coroutine>
#include <exception>
#include <iostream>
#include <thread>

template<typename PromiseType>
struct GetPromise {
    /// Implements  `promise` type that will be used by all the coroutine_handles.
    PromiseType *p_;
    // All the functions below are recognised/required by the compiler.
    // Await ready returning false means that the function is immediately ready to suspend.
    // This will make the first await_suspend after calling await_ready return true;
    bool await_ready () { return false; }
    // Await suspend returning false means that after all the tasks in the scope of the function
    // are done, do _not_ suspend the coroutine. Here it accepts a coro handle and sets an internal
    // pointer to point to its address. It is legal for await_suspend to return the coroutine handle from p_.
    bool await_suspend(std::coroutine_handle<PromiseType> handle) {
        p_ = &handle;
        return false;
    }
    // Since await_suspend did not suspend execution and does not return anything in particular,
    // we define an await_ready method to actually be able to access the promise_type from the struct.
    PromiseType *await_resume() { return p_; }
};

/// Generator is a simple generator that makes use of coroutines.
/// Note here that the name Generator is not required for coro magic.
/// We could have named it anything.
struct Generator {
    struct promise_type {
        uint64_t value_;
        Generator get_return_object () {
            return {
                .handle_ = std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
        // std::suspend_never tells the compiler that this promise_type requires eager execution.
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        // unhandled_exception is what the compiler will call to handle unhandled exceptions in the coro.
        void unhandled_exception() {};
        // yield_value is special. When we call co_yield on an awaiter (Generator in this case), the compiler
        // will call the awaiter's yield_value method.
        std::suspend_always yield_value(uint64_t value) {
            value_ = value;
            return {};
        }
    };
    std::coroutine_handle<promise_type> handle_;
};

/// A generic generator class.
template <typename T> struct GenericGenerator {
  // Forward-declare promise type to typedef handle_type.
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  struct promise_type {
    T value_;
    // The generic generator will be able to catch and rethrow exceptions.
    std::exception_ptr exception_;
    GenericGenerator get_return_object() {
      return GenericGenerator(handle_type::from_promise(*this));
    }
    // Declaring initial_suspend with type std::susend_always means that we will
    // have to call awaiter.resume before we can get the initial value out of
    // the awaiter.
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    // Set the excpetion ptr to point to the current exception. We will create a
    // method that will rethrow this excpetion.
    void unhandled_exception() { exception_ = std::current_exception(); }
    // Use a C++20 concept to assign a constraint on the template to choose the
    // correct overload.
    template <std::convertible_to<T> From>
    std::suspend_always yield_value(From &&from) {
      // Forward to the correct overload.
      value_ = std::forward<From>(from);
      return {};
    }
    // Indicates that the promise returns void.
    void return_void() {}
  };

  handle_type handle_;
  // Allow move-only construction from another Generator.
  GenericGenerator(const GenericGenerator &) = delete;
  GenericGenerator(handle_type h) : handle_(h) {}

  // Destroy the coro handle inside the Generator's destructor,
  // as it will no longer be needed once the Generator is destroyed.
  ~GenericGenerator() {
    handle_.destroy();
    std::cout << "Destroyed Generator and coro handle\n";
  }

  explicit operator bool() {
    fill();
    return !handle_.done();
  }

  T operator()() {
    fill();
    full_ = false;
    return std::move(handle_.promise().value_);
  }

private:
  bool full_ = false;
  void fill() {
    if (!full_) {
      handle_();
      // Rethrow any exceptions here.
      if (handle_.promise().exception_) {
        std::rethrow_exception(handle_.promise().exception_);
      }
      full_ = true;
    }
  }
};