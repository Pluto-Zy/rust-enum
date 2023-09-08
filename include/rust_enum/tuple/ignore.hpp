#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_IGNORE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_IGNORE_HPP

#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
namespace detail {
struct tuple_ignore_t {
    template <class T>
    CONSTEXPR14 const tuple_ignore_t& operator=(T&&) const noexcept {
        return *this;
    }
};
}  // namespace detail

#ifdef USE_CXX17
constexpr inline detail::tuple_ignore_t ignore {};
#else
const detail::tuple_ignore_t ignore {};
#endif
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_IGNORE_HPP
