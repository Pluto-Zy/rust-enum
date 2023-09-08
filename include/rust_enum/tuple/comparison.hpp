#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_COMPARISON_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_COMPARISON_HPP

#include <tuple>
#include <type_traits>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/universal_get.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
namespace detail {
template <class Lhs, class Rhs, std::size_t... Is>
CONSTEXPR14 auto tuple_equal(const Lhs& lhs, const Rhs& rhs, std::index_sequence<Is...>) -> bool {
    return (... && (universal_get<Is>(lhs) == universal_get<Is>(rhs)));
}
}  // namespace detail

template <class... Ts, class... Us>
CONSTEXPR14 auto operator==(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    static_assert(sizeof...(Ts) == sizeof...(Us), "Cannot compare tuples of different sizes.");
    return detail::tuple_equal(lhs, rhs, std::index_sequence_for<Ts...>());
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator!=(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return !(lhs == rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator==(const tuple<Ts...>& lhs, const std::tuple<Us...>& rhs) -> bool {
    static_assert(sizeof...(Ts) == sizeof...(Us), "Cannot compare tuples of different sizes.");
    return detail::tuple_equal(lhs, rhs, std::index_sequence_for<Ts...>());
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator!=(const tuple<Ts...>& lhs, const std::tuple<Us...>& rhs) -> bool {
    return !(lhs == rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator==(const std::tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    static_assert(sizeof...(Ts) == sizeof...(Us), "Cannot compare tuples of different sizes.");
    return detail::tuple_equal(lhs, rhs, std::index_sequence_for<Ts...>());
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator!=(const std::tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return !(lhs == rhs);
}

namespace detail {
template <std::size_t Size, std::size_t Idx = 0, class Lhs, class Rhs>
CONSTEXPR14 auto tuple_less(const Lhs& lhs, const Rhs& rhs) -> bool {
    if constexpr (Idx == Size) {
        return false;
    } else {
        if (universal_get<Idx>(lhs) < universal_get<Idx>(rhs))
            return true;
        if (universal_get<Idx>(rhs) < universal_get<Idx>(lhs))
            return false;
        return tuple_less_impl<Idx + 1, Size>(lhs, rhs);
    }
}
}  // namespace detail

template <class... Ts, class... Us>
CONSTEXPR14 auto operator<(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    static_assert(sizeof...(Ts) == sizeof...(Us), "Cannot compare tuples of different sizes.");
    return detail::tuple_less<sizeof...(Ts)>(lhs, rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator<(const tuple<Ts...>& lhs, const std::tuple<Us...>& rhs) -> bool {
    static_assert(sizeof...(Ts) == sizeof...(Us), "Cannot compare tuples of different sizes.");
    return detail::tuple_less<sizeof...(Ts)>(lhs, rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator<(const std::tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    static_assert(sizeof...(Ts) == sizeof...(Us), "Cannot compare tuples of different sizes.");
    return detail::tuple_less<sizeof...(Ts)>(lhs, rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator>(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return rhs < lhs;
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator>(const tuple<Ts...>& lhs, const std::tuple<Us...>& rhs) -> bool {
    return rhs < lhs;
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator>(const std::tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return rhs < lhs;
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator<=(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return !(rhs < lhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator<=(const tuple<Ts...>& lhs, const std::tuple<Us...>& rhs) -> bool {
    return !(rhs < lhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator<=(const std::tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return !(rhs < lhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator>=(const tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return !(lhs < rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator>=(const tuple<Ts...>& lhs, const std::tuple<Us...>& rhs) -> bool {
    return !(lhs < rhs);
}

template <class... Ts, class... Us>
CONSTEXPR14 auto operator>=(const std::tuple<Ts...>& lhs, const tuple<Us...>& rhs) -> bool {
    return !(lhs < rhs);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_COMPARISON_HPP
