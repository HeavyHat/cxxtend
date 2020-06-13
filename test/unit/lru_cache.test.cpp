#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hh/lru_cache.hpp>
#include <hh/helpers.hpp>
#include <chrono>
#include <thread>

namespace hh::functools{


int add(int a, int b) {
    return a + b;
}



TEST(lru_cache, initiliased_with_add_with_a_size_of_128) {
    auto cache_add = hh::functools::make_lrucache(add);
}

TEST(lru_cache, invocation_of_cached_method_is_equivialant_to_non_cached_variant) {
    auto cached_add = hh::functools::make_lrucache(add);
    EXPECT_EQ(cached_add(1,1), 1+1) << "Cached evaluation and non cached evaluation is not equivilant with no resuse! It is likely that the correct underlying implementation is not being called.";
}

TEST(lru_cache, invocation_of_cached_method_is_equivialant_to_non_cached_variant_when_returning_precalculated_value) {
    auto cached_add = hh::functools::make_lrucache(add);
    cached_add(1,1);
    EXPECT_EQ(cached_add(1,1), 1+1) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
}

TEST(lru_cache, size_increases_as_unique_parameterised_calls_are_made) {
    auto cached_add = hh::functools::make_lrucache(add);
    cached_add(1,1);
    EXPECT_EQ(cached_add.size(), 1) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
    cached_add(1,2);
    EXPECT_EQ(cached_add.size(), 2) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
    cached_add(2,1);
    EXPECT_EQ(cached_add.size(), 3) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
    cached_add(2,2);
    EXPECT_EQ(cached_add.size(), 4) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
}
TEST(lru_cache, size_increases_as_unique_parameterised_calls_are_made_but_bot_when_input_is_equal) {
    auto cached_add = hh::functools::make_lrucache(add);
    cached_add(1,1);
    EXPECT_EQ(cached_add.size(), 1) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
    cached_add(1,2);
    EXPECT_EQ(cached_add.size(), 2) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
    cached_add(2,1);
    EXPECT_EQ(cached_add.size(), 3) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
    cached_add(2,1);
    EXPECT_EQ(cached_add.size(), 3) << "Cached evaluation and non cached evaluation is not equivilant with reuse! It is likely that the cache is malformed.";
}

TEST(lru_cache, returns_correct_result_for_smaller_cache_sizes) {
    constexpr static auto ITERATIONS = 128;
    std::function<int(int,int)> delayed_add = hh::helpers::make_delayed<1>(add);
    auto cached_add = hh::functools::make_lrucache(delayed_add);
    std::vector<int> expected{};
    std::vector<int> actual{};
    for(int i = 1; i < ITERATIONS; ++i) {
        actual.emplace_back(cached_add(i,i));
        expected.emplace_back(i+i);
    }
    EXPECT_THAT(actual, ::testing::ElementsAreArray(expected)) << "Calling the uncached variant of the function yielded some different results for large sizes!";
}

TEST(lru_cache, retriving_cached_results_is_faster_than_calling_slow_method) {
    constexpr static auto ITERATIONS = 128;
    std::function<int(int,int)> delayed_add = hh::helpers::make_delayed<1>(add);
    auto cached_add = hh::functools::make_lrucache(delayed_add, ITERATIONS);
    std::vector<int> expected{};
    std::vector<int> actual{};
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 1; i < ITERATIONS; ++i) {
        cached_add(i,i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto normal_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    start = std::chrono::high_resolution_clock::now();
    for(int i = 1; i < ITERATIONS; ++i) {
        cached_add(i,i);
    }
    end = std::chrono::high_resolution_clock::now();
    auto cached_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    EXPECT_LT(cached_time, normal_time) << "Retrieving data from cache seems to be extremely slow!";
}
}
