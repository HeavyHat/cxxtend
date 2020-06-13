#include <hh/lru_cache.hpp>
#include <hh/helpers.hpp>
#include <benchmark/benchmark.h>

int add(int a, int b) {
    return a + b;
}


void benchmark_cache_creation(::benchmark::State& state) {
    for(const auto& _ : state) {
       auto cached_add = hh::functools::make_lrucache(add); 
    }
}

void benchmark_cache_usage_1ms(::benchmark::State& state) {
    auto delayed_add = hh::helpers::make_delayed<1>(add);
    auto cached_add = hh::functools::make_lrucache(delayed_add); 
    for(const auto& _ : state) {
        for(int i = 0; i < 128; ++i) {
            cached_add(i, i);
        }

        for(int i = 0; i < 128; ++i) {
            cached_add(i, i);
        }
    }
}

BENCHMARK(benchmark_cache_creation);
BENCHMARK(benchmark_cache_usage_1ms);
BENCHMARK_MAIN();

