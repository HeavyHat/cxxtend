#ifndef INCLUDED_HH_CALL_COUNTER_H
#define INCLUDED_HH_CALL_COUNTER_H

#include <functional>
#include <hh/hash.hpp>
#include <tuple>

namespace hh {
  namespace functools {
    template <typename RETURN_TYPE, typename... ARGUMENTS> class call_counter {
    public:
      using key_type = std::tuple<ARGUMENTS...>;
      using function_signture = std::function<RETURN_TYPE(ARGUMENTS...)>;

    private:
      mutable std::unordered_map<key_type, unsigned long long> d_counts;
      mutable unsigned long long d_total_calls = 0ul;
      function_signture d_func;

    public:
      call_counter() = delete;
      constexpr call_counter(RETURN_TYPE (*func)(ARGUMENTS...)) : d_func{func} {};
      constexpr call_counter(function_signture func) : d_func{func} {};

      template <typename... INPUT_ARGUMENTS>
      unsigned long long call_count(INPUT_ARGUMENTS&&... args) {
        key_type key{std::forward<INPUT_ARGUMENTS>(args)...};
        return d_counts[std::move(key)];
      }

      template <typename... INPUT_ARGUMENTS>
      RETURN_TYPE operator()(INPUT_ARGUMENTS&&... args) const {
        key_type key = std::make_tuple(std::forward<INPUT_ARGUMENTS>(args)...);
        ++d_counts[std::move(key)];
        ++d_total_calls;
        return d_func(std::forward<INPUT_ARGUMENTS>(args)...);
      }

      unsigned int unique_entries() const { return d_counts.size(); }
      unsigned long long total_calls() const { return d_total_calls; }
    };
  }  // namespace functools
}  // namespace hh

#endif
