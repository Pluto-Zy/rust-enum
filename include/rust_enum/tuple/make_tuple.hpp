#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_MAKE_TUPLE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_MAKE_TUPLE_HPP

#include <utility>

#include <rust_enum/tuple/detail/std_impl.hpp>
#include <rust_enum/tuple/tuple.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
template <class... Args>
CONSTEXPR14 auto make_tuple(Args&&... args) -> tuple<detail::unwrap_ref_decay_t<Args>...> {
    return tuple<detail::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_MAKE_TUPLE_HPP
