#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_FORWARD_AS_TUPLE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_FORWARD_AS_TUPLE_HPP

#include <utility>

#include <rust_enum/tuple/tuple.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
template <class... Args>
CONSTEXPR14 auto forward_as_tuple(Args&&... args) noexcept -> tuple<Args&&...> {
    return tuple<Args&&...>(std::forward<Args>(args)...);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_FORWARD_AS_TUPLE_HPP
