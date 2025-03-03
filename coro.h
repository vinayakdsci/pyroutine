#include <coroutine>
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
