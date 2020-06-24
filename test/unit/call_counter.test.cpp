/*
 * =====================================================================================
 *
 *       Filename:  call_counter.test.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/24/2020 11:40:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include <hh/call_counter.hpp>

namespace hh::functools {

  namespace {
    int add(int a, int b) { return a + b; }
  }  // namespace

  TEST(call_counter, empty_on_initial_construction) {
    call_counter counter{add};
    EXPECT_EQ(counter.total_calls(), 0) << "Call counter instantiated incorrectly.";
  }

  TEST(call_counter, no_unique_entries_when_first_created) {
    call_counter counter{add};
    EXPECT_EQ(counter.unique_entries(), 0) << "Call counter instantiated incorrectly.";
  }

  TEST(call_counter, entires_which_have_not_been_called_return_zero) {
    call_counter counter{add};
    EXPECT_EQ(counter.call_count(1, 2), 0)
        << "Call counter has non zero value for entry which has not been called.";
  }

  TEST(call_counter, entires_which_have_been_called_return_non_zero) {
    call_counter counter{add};
    counter(1, 2);
    EXPECT_EQ(counter.call_count(1, 2), 1) << "Call counter did not increment call count.";
  }

  TEST(call_counter, entires_which_have_been_called_increments_total_call) {
    call_counter counter{add};
    counter(1, 2);
    EXPECT_EQ(counter.total_calls(), 1) << "Call counter did not increment call count.";
  }

  TEST(call_counter, entires_which_have_been_called_increments_unqiue_entires) {
    call_counter counter{add};
    counter(1, 2);
    EXPECT_EQ(counter.unique_entries(), 1) << "Call counter did not increment call count.";
  }
}  // namespace hh::functools
