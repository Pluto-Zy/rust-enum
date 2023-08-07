#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_SIZE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_SIZE_HPP

#include <type_traits>

#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
template <class Ty>
struct variant_size;

template <class... Tys>
struct variant_size<variant<Tys...>> : std::integral_constant<std::size_t, sizeof...(Tys)> { };

template <class Ty>
struct variant_size<const Ty> : variant_size<Ty> { };

template <class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_size<volatile Ty> : variant_size<Ty> { };

template <class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_size<const volatile Ty> : variant_size<Ty> { };

template <class Ty>
inline constexpr std::size_t variant_size_v = variant_size<Ty>::value;
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_SIZE_HPP
