#ifndef INCLUDED_HH_OPTIONAL_HPP
#define INCLUDED_HH_OPTIONAL_HPP

#include <tl/expected.hpp>

namespace hh {
    struct null_optional{
        constexpr null_optional() {}
    };
    template <typename T>
    using optional = tl::expected<T, null_optional>;
    static auto nullopt = tl::make_unexpected<null_optional>({});
}

#endif
