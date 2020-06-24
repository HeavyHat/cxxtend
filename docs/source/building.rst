Building
--------

This repo uses CMake as its primary build system, the targets are also accessible via the root level Makefile. Additionally to aid with the resolution of dependencies on other machines there is an included dockerfile which comes pckaged with the instructions necessary to build the task as well as run the task in a minimal environment. Building the tests can be done using the makefile `build-test` and `exec-test` targets or via the `test` target. This makefile is only used as an access and documentation of cmake targets and additional targets may be available within the Cmake configuration.

This repo also comes packaged with a benchmarking suite that leverages GoogleBenchmark as well as a google test suite for testing any new functionality. These should be treated as living documentation of the usage of the packaged utilities as well as an indication of the supported functionality. Any additions to the provided functionality should document this functionality in the provided tests and benchmarks. These will be reported on the readme of the repo.

The provided docker image also ships with the necessary utilities to build the documentation and the included test harnesses.
