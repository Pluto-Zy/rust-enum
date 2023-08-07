#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_MONOSTATE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_MONOSTATE_HPP

#include <rust_enum/variant/detail/macro.hpp>

#ifdef USE_CXX20
    #include <compare>
#endif

namespace rust {
struct monostate { };

CONSTEXPR17 auto operator==(monostate, monostate) noexcept -> bool {
    return true;
}

#ifdef USE_CXX20
CONSTEXPR20 auto operator<=>(monostate, monostate) noexcept -> std::strong_ordering {
    return std::strong_ordering::equal;
}
#else
CONSTEXPR17 auto operator!=(monostate, monostate) noexcept -> bool {
    return false;
}

CONSTEXPR17 auto operator<(monostate, monostate) noexcept -> bool {
    return false;
}

CONSTEXPR17 auto operator>(monostate, monostate) noexcept -> bool {
    return false;
}

CONSTEXPR17 auto operator<=(monostate, monostate) noexcept -> bool {
    return true;
}

CONSTEXPR17 auto operator>=(monostate, monostate) noexcept -> bool {
    return true;
}
#endif  // USE_CXX20
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_MONOSTATE_HPP
