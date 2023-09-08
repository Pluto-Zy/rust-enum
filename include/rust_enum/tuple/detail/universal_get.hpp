#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_UNIVERSAL_GET_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_UNIVERSAL_GET_HPP

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

#include <rust_enum/tuple/detail/fwd/get_fwd.hpp>
#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <
    std::size_t Idx,
    class Tuple,
    typename std::enable_if<
        is_specialization_of_tuple<typename remove_cvref<Tuple>::type>::value,
        int>::type = 0>
auto universal_get(Tuple&& t) noexcept -> decltype(rust::get<Idx>(std::forward<Tuple>(t))) {
    return rust::get<Idx>(std::forward<Tuple>(t));
}

template <
    std::size_t Idx,
    class Tuple,
    typename std::enable_if<
        !is_specialization_of_tuple<typename remove_cvref<Tuple>::type>::value,
        int>::type = 0>
auto universal_get(Tuple&& t) noexcept -> decltype(std::get<Idx>(std::forward<Tuple>(t))) {
    return std::get<Idx>(std::forward<Tuple>(t));
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_UNIVERSAL_GET_HPP
