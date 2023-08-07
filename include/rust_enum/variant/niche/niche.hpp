#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_NICHE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_NICHE_HPP

#include <climits>
#include <cstddef>

#include <rust_enum/variant/niche/unsigned_max.hpp>
#include <rust_enum/variant/niche/wrapping_range.hpp>

namespace rust {
template <class Ty>
struct niche {
    /// Represents the size of type Ty (in bits).
    static constexpr std::size_t size_in_bits = sizeof(Ty) * CHAR_BIT;
    /// Represents the range of valid values.
    using valid_range = wrapping_range<0, unsigned_max<size_in_bits>::value>;

    static constexpr void set_niche(Ty&, std::size_t) { }
    static constexpr auto is_niche_value(Ty const&) -> bool {
        return false;
    }
    static constexpr auto get_niche(Ty const&) -> std::size_t {
        return 0;
    }
};

template <class Ty>
struct niche<Ty&> {
    static constexpr std::size_t size_in_bits = sizeof(Ty*) * CHAR_BIT;
    using valid_range = wrapping_range<1, unsigned_max<size_in_bits>::value>;

    static constexpr void set_niche(Ty*& target, std::size_t) {
        target = nullptr;
    }

    static constexpr auto is_niche_value(Ty* target) -> bool {
        return target == nullptr;
    }

    static constexpr auto get_niche(Ty*) -> std::size_t {
        return 0;
    }
};
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_NICHE_HPP
