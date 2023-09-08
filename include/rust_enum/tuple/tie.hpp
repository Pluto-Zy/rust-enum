#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TIE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TIE_HPP

#include <rust_enum/tuple/tuple.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
template <class... Args>
CONSTEXPR14 auto tie(Args&... args) noexcept -> tuple<Args&...> {
    return tuple<Args&...>(args...);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TIE_HPP
