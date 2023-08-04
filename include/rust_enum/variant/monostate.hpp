#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_MONOSTATE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_MONOSTATE_HPP

#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
struct monostate { };

CONSTEXPR17 auto operator==(monostate, monostate) noexcept -> bool {
    return true;
}

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
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_MONOSTATE_HPP
