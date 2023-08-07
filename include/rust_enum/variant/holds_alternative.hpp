#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_HOLDS_ALTERNATIVE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_HOLDS_ALTERNATIVE_HPP

#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/special_member/constructor_helper.hpp>
#include <rust_enum/variant/variant.hpp>

namespace rust {
template <class Ty, class... Tys>
CONSTEXPR17 auto holds_alternative(const variant<Tys...>& var) noexcept -> bool {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return var.index() == match_index;
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_HOLDS_ALTERNATIVE_HPP
