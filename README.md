### Pyroutine

This repository contains experimental code to implement and test native C++20 coroutines with Nanobind and use them in Python.
The primary intent is to replace the need to use `asyncio` when awaitable functions have to be used in Python.

#### Build instructions

For now, nanobind/pybind bindings have not been implemented. To test out the C++ coroutine code, build with:
```sh
make coro
```

This will produce a binary name `coro.out` in the top level directory. Run with
```sh
./coro.out
```
