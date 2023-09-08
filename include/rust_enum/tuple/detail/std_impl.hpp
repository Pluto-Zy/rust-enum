#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_STD_IMPL_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_STD_IMPL_HPP

#include <functional>
#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>

namespace rust::detail {
#ifdef USE_CXX20
using std::unwrap_ref_decay_t;
#else
// Implementation of C++20 std::unwrap_ref_decay_t
template <class Ty>
struct unwrap_ref_decay_impl {
    using type = Ty;
};

template <class Ty>
struct unwrap_ref_decay_impl<std::reference_wrapper<Ty>> {
    using type = Ty&;
};

template <class Ty>
using unwrap_ref_decay_t = typename unwrap_ref_decay_impl<typename std::decay<Ty>::type>::type;
#endif
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_STD_IMPL_HPP
