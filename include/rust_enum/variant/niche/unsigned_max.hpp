#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_UNSIGNED_MAX_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_UNSIGNED_MAX_HPP

#include <cstddef>
#include <limits>

namespace rust {
template <std::size_t SizeInBits>
struct unsigned_max {
    static constexpr std::size_t value = [] {
        constexpr std::size_t max_bit = sizeof(std::size_t) * CHAR_BIT;
        if constexpr (SizeInBits >= max_bit) {
            return (std::numeric_limits<std::size_t>::max)();
        } else {
            return (std::size_t(1) << SizeInBits) - 1;
        }
    }();
};
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_UNSIGNED_MAX_HPP
