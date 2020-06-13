#ifndef INCLUDED_HH_LRU_CACHE_HPP
#define INCLUDED_HH_LRU_CACHE_HPP
#include <utility>
#include <list>
#include <tuple>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <hh/optional.hpp>
#include <hh/hash.hpp>
#include <tl/expected.hpp>

#include <experimental/iterator>


namespace hh {
    namespace functools {


    template <typename RETURN_TYPE, typename... ARGUMENTS>
    class lru_cache {
        public:
            static constexpr std::size_t key_size = sizeof...(ARGUMENTS);
            using return_type = RETURN_TYPE;
            using decayed_return_type = std::decay_t<RETURN_TYPE>;
            using cache_key = std::tuple<std::decay_t<ARGUMENTS>...>;
            using cache_entry = std::pair<cache_key,decayed_return_type>;
            using function_signature = std::function<RETURN_TYPE (ARGUMENTS...)>;


            function_signature d_func;
            unsigned int d_max_size;
            mutable std::list<cache_entry> d_cache;
            mutable std::unordered_map<cache_key,typename decltype(d_cache)::iterator> d_cache_map;

            lru_cache() = delete;
            constexpr lru_cache(RETURN_TYPE (*func)(ARGUMENTS...),unsigned int cache_size) : d_func{func}, d_max_size{cache_size}, d_cache{}, d_cache_map{cache_size} {}
            constexpr lru_cache(function_signature func,unsigned int cache_size) : d_func{func}, d_max_size{cache_size}, d_cache{}, d_cache_map{cache_size} {}
            virtual ~lru_cache() = default;

            template <typename... INPUT_ARGUMENTS>
            RETURN_TYPE operator()(INPUT_ARGUMENTS&& ...args) {
                cache_key key{std::forward<INPUT_ARGUMENTS>(args)...};
                get_entry_from_cache(key)
                    .map([this](const auto& entry_it){
                             d_cache.splice(d_cache.end(), d_cache, entry_it);
                        })
                    .map_error([this, &key](...){
                            put_in_cache(std::move(key), std::apply(d_func, key));
                        });
                return d_cache.back().second;
            }

            constexpr auto max_size() const {
                return d_max_size;
            }

            auto size() const {
                return d_cache_map.size();
            }

        private:
            struct unexpected_tag {};

            hh::optional<typename decltype(d_cache)::iterator> get_entry_from_cache(const cache_key& key)const {
                auto found_it = d_cache_map.find(key);
                if(found_it == d_cache_map.end()) {
                    return hh::nullopt;
                }
                return found_it->second;
            }

            template <typename... ARGS>
            tl::expected<void,unexpected_tag> put_in_cache(cache_key&& key, decayed_return_type&& return_value) const {
                d_cache.emplace_back(std::move(key), std::move(return_value));
                d_cache_map[key] = std::prev(d_cache.end());
                if(size() > max_size()) {
                    d_cache_map.erase(d_cache.front().first);
                    d_cache.pop_front();
                }
                return {};
            }

    };

    constexpr static auto DEFAULT_SIZE = 128u;


    template <typename RETURN_TYPE, typename... ARGUMENTS>
    constexpr auto make_lrucache(std::function<RETURN_TYPE(ARGUMENTS...)> func, unsigned int size = DEFAULT_SIZE) {
        return lru_cache<RETURN_TYPE, ARGUMENTS...>(func, size);
    } 
    template <typename RETURN_TYPE, typename... ARGUMENTS>
    constexpr auto make_lrucache(RETURN_TYPE (*func)(ARGUMENTS...), unsigned int size = DEFAULT_SIZE) {
        return lru_cache<RETURN_TYPE, ARGUMENTS...>(func, size);
    } 

	template<class Ch, class Tr, class Tuple, std::size_t... Is>
	void print_tuple_impl(std::basic_ostream<Ch,Tr>& os,
						  const Tuple& t,
						  std::index_sequence<Is...> = std::make_index_sequence<std::tuple_size<Tuple>::value>())
	{
		((os << (Is == 0? "" : ", ") << std::get<Is>(t)), ...);
	}

    template <typename RETURN_TYPE, typename... ARGUMENTS>
    inline std::ostream& operator<<(std::ostream& os, const typename lru_cache<RETURN_TYPE, ARGUMENTS...>::cache_key& key) {
        os << "(";
        print_tuple_impl(os, key);
        os << ")";
        return os;
    }
    
    template <typename RETURN_TYPE, typename... ARGUMENTS>
    inline std::ostream& operator<<(std::ostream& os, const typename lru_cache<RETURN_TYPE, ARGUMENTS...>::cache_entry& entry) {
        os  << entry.first << " -> " << entry.second; 
        return os;
    }

    template <typename RETURN_TYPE, typename... ARGUMENTS>
    inline std::ostream& operator<<(std::ostream& os,  const lru_cache<RETURN_TYPE, ARGUMENTS...>& cache) {
        os << "lru_cache<" << cache.size() << "/" << cache.max_size() << ">[ ";
        std::copy(cache.d_cache.begin(),cache.d_cache.end(), std::experimental::make_ostream_joiner(os, ", "));
        os << " ]";
        return os;
    }
    }

}
#endif
