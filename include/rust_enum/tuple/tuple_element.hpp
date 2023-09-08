#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_ELEMENT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_ELEMENT_HPP

#include <tuple>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>

// use std::tuple_element to implement it currently
template <std::size_t Idx, class... Tys>
struct std::tuple_element<Idx, rust::tuple<Tys...>> :
    std::tuple_element<Idx, std::tuple<Tys...>> { };

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_ELEMENT_HPP
