#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DEDUCTION_GUIDE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DEDUCTION_GUIDE_HPP

#include <memory>
#include <tuple>
#include <utility>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>

#ifdef USE_CXX17
namespace rust {
template <class... Us>
tuple(Us...) -> tuple<Us...>;

template <class U1, class U2>
tuple(std::pair<U1, U2>) -> tuple<U1, U2>;

template <class... Us>
tuple(std::tuple<Us...>) -> tuple<Us...>;

template <class Alloc, class... Us>
tuple(std::allocator_arg_t, Alloc, Us...) -> tuple<Us...>;

template <class Alloc, class U1, class U2>
tuple(std::allocator_arg_t, Alloc, std::pair<U1, U2>) -> tuple<U1, U2>;

template <class Alloc, class... Us>
tuple(std::allocator_arg_t, Alloc, tuple<Us...>) -> tuple<Us...>;

template <class Alloc, class... Us>
tuple(std::allocator_arg_t, Alloc, std::tuple<Us...>) -> tuple<Us...>;
}  // namespace rust
#endif

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DEDUCTION_GUIDE_HPP
