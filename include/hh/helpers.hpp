#ifndef INCLUDED_HH_HELPERS_H
#define INCLUDED_HH_HELPERS_H

#include <chrono>
#include <thread>
#include <functional>

namespace hh::helpers{
    
template <unsigned int DELAY, typename TIME_UNITS = std::chrono::milliseconds, typename RETURN_TYPE, typename... ARGUMENTS>
std::function<RETURN_TYPE(ARGUMENTS...)> make_delayed(RETURN_TYPE (*func)(ARGUMENTS...)) {
    return [func](ARGUMENTS&& ...args) -> RETURN_TYPE {
        std::this_thread::sleep_for(TIME_UNITS(DELAY));
        return func(std::forward<ARGUMENTS>(args)...);
    };
}

template <unsigned int DELAY, typename TIME_UNITS = std::chrono::milliseconds, typename RETURN_TYPE, typename... ARGUMENTS>
std::function<RETURN_TYPE(ARGUMENTS...)> make_delayed(std::function<RETURN_TYPE(ARGUMENTS...)> func) {
    return [func](ARGUMENTS&& ...args) -> RETURN_TYPE {
        std::this_thread::sleep_for(TIME_UNITS(DELAY));
        return func(std::forward<ARGUMENTS>(args)...);
    };
}


template <unsigned int DELAY, typename TIME_UNITS = std::chrono::milliseconds, typename RETURN_TYPE, typename C, typename... ARGUMENTS>
std::function<RETURN_TYPE(ARGUMENTS...)> make_delayed(RETURN_TYPE (C::*func)(ARGUMENTS...)) {
    return [func](ARGUMENTS&& ...args) -> RETURN_TYPE {
        std::this_thread::sleep_for(TIME_UNITS(DELAY));
        return func(std::forward<ARGUMENTS>(args)...);
    };
}

template <unsigned int DELAY, typename TIME_UNITS = std::chrono::milliseconds, typename RETURN_TYPE, typename C, typename... ARGUMENTS>
std::function<RETURN_TYPE(ARGUMENTS...)> make_delayed(RETURN_TYPE (C::*func)(ARGUMENTS...) const) {
    return [func](ARGUMENTS&& ...args) -> RETURN_TYPE {
        std::this_thread::sleep_for(TIME_UNITS(DELAY));
        return func(std::forward<ARGUMENTS>(args)...);
    };
}
}

#endif
