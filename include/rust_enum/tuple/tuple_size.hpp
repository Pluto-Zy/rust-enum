#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_SIZE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_SIZE_HPP

#include <tuple>
#include <type_traits>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>

template <class... Tys>
struct ::std::tuple_size<rust::tuple<Tys...>> :
    ::std::integral_constant<::std::size_t, sizeof...(Tys)> { };

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_SIZE_HPP
