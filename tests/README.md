Automated Tests
===============

We use Google's [GTest framework](http://code.google.com/p/googletest/) for the tests. If CMake cannot find the GTest library, it will disable all tests and issue a warning. Additionally, if [Valgrind](http://valgrind.org/) is available, it will be used to run some tests for a second time in order to look for memory leaks.

The test suite is organized as follows:

* `tests/core` contains unit tests for the Coral core library.
* `tests/lua` tests the Lua integration provided by the 'lua' module.
* `tests/moduleA` defines a Coral module whose purpose is to try and test features (in terms of type declarations and implementations) that were not exercised nor tested by the Coral core. It also provides the dependencies for moduleB, in order to test dependency handling.
* `tests/moduleB` defines a second Coral module for testing module dependency handling and inter-module interactions.
* `tests/regen` re-builds the core library using freshly-generated code, to test whether the latest code generation rules would break the core.
