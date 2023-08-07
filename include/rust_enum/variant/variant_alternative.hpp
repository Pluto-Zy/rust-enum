#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_ALTERNATIVE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_ALTERNATIVE_HPP

#include <tuple>

#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
template <std::size_t Idx, class Ty>
struct variant_alternative;

// use std::tuple_element to implement it currently
template <std::size_t Idx, class... Tys>
struct variant_alternative<Idx, variant<Tys...>> : std::tuple_element<Idx, std::tuple<Tys...>> { };

template <std::size_t Idx, class Ty>
using variant_alternative_t = typename variant_alternative<Idx, Ty>::type;

template <std::size_t Idx, class Ty>
struct variant_alternative<Idx, const Ty> : std::add_const<variant_alternative_t<Idx, Ty>> { };

template <std::size_t Idx, class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_alternative<Idx, volatile Ty> :
    std::add_volatile<variant_alternative_t<Idx, Ty>> { };

template <std::size_t Idx, class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_alternative<Idx, const volatile Ty> :
    std::add_cv<variant_alternative_t<Idx, Ty>> { };
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_ALTERNATIVE_HPP
