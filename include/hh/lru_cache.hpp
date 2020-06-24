#ifndef INCLUDED_HH_LRU_CACHE_HPP
#define INCLUDED_HH_LRU_CACHE_HPP
#include <experimental/iterator>
#include <functional>
#include <hh/hash.hpp>
#include <hh/optional.hpp>
#include <list>
#include <mutex>
#include <tl/expected.hpp>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace hh {
  namespace functools {

    /**
     * Functional-style lru-cache which either calls the function from cache or returns a previouly
     * attained value.
     *
     * The cache is initialised with a specific size and only the most-recently used N argument
     * tuples will be recorded. After the size paramter exceeds the maximum allowed size previous
     * tuples return values will be dropped. Providing a max size of zero makes the cache size
     * infinite, and can retain cached values as long as memory is available. The underlying
     * implementation is based on hastaples to look up the hashed parmaeter tuples which have been
     * used to call the function, for this reason all types in the argument list for the underlying
     * function must implement the std::hash<T> specialisation and all for equality comparison.
     *
     * @see std::hash()
     * @see make_lrucache
     * @tparam RETURN_TYPE The return type of the function used to instantiate this cache.
     * @tparams ARGUMENTS... Paramter Pack of types which this function declares in its signature.
     */
    template <typename RETURN_TYPE, typename... ARGUMENTS> class lru_cache {
    public:
      static constexpr std::size_t key_size = sizeof...(ARGUMENTS);
      using return_type = RETURN_TYPE; /** The return type of this cache.*/
      using decayed_return_type
          = std::decay_t<RETURN_TYPE>; /** The decayed returned type of this cache. */
      using cache_key
          = std::tuple<std::decay_t<ARGUMENTS>...>; /** The tuple type used to key return values.
                                                       Two equal cache keys should return equal
                                                       return values. */
      using cache_entry
          = std::pair<cache_key,
                      decayed_return_type>; /** The entry in the cache implementation. The decayed
                                               type is used to prevent naught reference tricks. */
      using function_signature = std::function<RETURN_TYPE(ARGUMENTS...)>;

      function_signature d_func;
      unsigned int d_max_size;
      mutable std::list<cache_entry> d_cache;
      mutable std::unordered_map<cache_key, typename decltype(d_cache)::iterator> d_cache_map;

      lru_cache() = delete;
      /**
       * Constructs a cache with a given function and size.
       *
       * @see make_lrucache()
       * @param func The underlying function which will be used to populate the cache on call.
       * @param cache_size The maximum size of the underlying cache.
       */
      constexpr lru_cache(RETURN_TYPE (*func)(ARGUMENTS...), unsigned int cache_size)
          : d_func{func}, d_max_size{cache_size}, d_cache{}, d_cache_map{cache_size} {}
      /**
       * Constructs a cache with a given function and size.
       *
       * @see make_lrucache()
       * @param func The underlying function which will be used to populate the cache on call.
       * @param cache_size The maximum size of the underlying cache.
       */
      constexpr lru_cache(function_signature func, unsigned int cache_size)
          : d_func{func}, d_max_size{cache_size}, d_cache{}, d_cache_map{cache_size} {}
      virtual ~lru_cache() = default;

      /**
       * Calls the underlying function or returns historic value.
       *
       * Will first check to see if the tuple has been calculated previously, to do this is requires
       * us to hash the arguments and call an equality of cache keys. This can have performance
       * implications for functions which have very long argument lists where this operation is more
       * expensive than calling the underlying function.
       *
       * @see lru_cache()
       * @tparam INPUT_ARGUMENTS A collection of arguments which should be convertibale to the
       * cachekey of the underlying. A static assertion checks this property.
       * @params args The arguments to call the function with, arguments are perfectly forwarded to
       * the underlying signature of the function this cache was initialised with. Implicitly
       * converting arguments as required.
       */
      template <typename... INPUT_ARGUMENTS> RETURN_TYPE operator()(INPUT_ARGUMENTS&&... args) {
        cache_key key{std::forward<INPUT_ARGUMENTS>(args)...};
        get_entry_from_cache(key)
            .map([this](const auto& entry_it) { d_cache.splice(d_cache.end(), d_cache, entry_it); })
            .map_error(
                [this, &key](...) { put_in_cache(std::move(key), std::apply(d_func, key)); });
        return d_cache.back().second;
      }

      /** The maximum number of entries this cache can store. If 0 an infinite number of values can
       * be stored. */
      constexpr auto max_size() const { return d_max_size; }

      /** The current number of retained historic values */
      auto size() const { return d_cache_map.size(); }

    private:
      struct unexpected_tag {};

      hh::optional<typename decltype(d_cache)::iterator> get_entry_from_cache(
          const cache_key& key) const {
        auto found_it = d_cache_map.find(key);
        if (found_it == d_cache_map.end()) {
          return hh::nullopt;
        }
        return found_it->second;
      }

      template <typename... ARGS>
      tl::expected<void, unexpected_tag> put_in_cache(cache_key&& key,
                                                      decayed_return_type&& return_value) const {
        d_cache.emplace_back(std::move(key), std::move(return_value));
        d_cache_map[key] = std::prev(d_cache.end());
        if (max_size() != 0 && size() > max_size()) {
          d_cache_map.erase(d_cache.front().first);
          d_cache.pop_front();
        }
        return {};
      }
    };

    constexpr static auto DEFAULT_SIZE = 128u;

    template <typename RETURN_TYPE, typename... ARGUMENTS>
    constexpr auto make_lrucache(std::function<RETURN_TYPE(ARGUMENTS...)> func,
                                 unsigned int size = DEFAULT_SIZE) {
      return lru_cache<RETURN_TYPE, ARGUMENTS...>(func, size);
    }
    template <typename RETURN_TYPE, typename... ARGUMENTS>
    constexpr auto make_lrucache(RETURN_TYPE (*func)(ARGUMENTS...),
                                 unsigned int size = DEFAULT_SIZE) {
      return lru_cache<RETURN_TYPE, ARGUMENTS...>(func, size);
    }

    template <class Ch, class Tr, class Tuple, std::size_t... Is> void print_tuple_impl(
        std::basic_ostream<Ch, Tr>& os, const Tuple& t,
        std::index_sequence<Is...> = std::make_index_sequence<std::tuple_size<Tuple>::value>()) {
      ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
    }

    template <typename RETURN_TYPE, typename... ARGUMENTS> inline std::ostream& operator<<(
        std::ostream& os, const typename lru_cache<RETURN_TYPE, ARGUMENTS...>::cache_key& key) {
      os << "(";
      print_tuple_impl(os, key);
      os << ")";
      return os;
    }

    template <typename RETURN_TYPE, typename... ARGUMENTS> inline std::ostream& operator<<(
        std::ostream& os, const typename lru_cache<RETURN_TYPE, ARGUMENTS...>::cache_entry& entry) {
      os << entry.first << " -> " << entry.second;
      return os;
    }

    /**
     * Helper function to print the contents of the cache, used for debugging purposes. This
     * operation can be expensive so should not be used in production code.
     *
     * @see make_lrucache()
     * @param os The output strem to stream the cache into.
     * @param cache The cache to output to the given stream.
     * @tparam RETURN_TYPE The return type of the cache.
     * @tparam ARGUMENTS... The arguments that are used to construct the cache keys.
     */
    template <typename RETURN_TYPE, typename... ARGUMENTS>
    inline std::ostream& operator<<(std::ostream& os,
                                    const lru_cache<RETURN_TYPE, ARGUMENTS...>& cache) {
      os << "lru_cache<" << cache.size() << "/" << cache.max_size() << ">[ ";
      std::copy(cache.d_cache.begin(), cache.d_cache.end(),
                std::experimental::make_ostream_joiner(os, ", "));
      os << " ]";
      return os;
    }
  }  // namespace functools

}  // namespace hh
#endif
