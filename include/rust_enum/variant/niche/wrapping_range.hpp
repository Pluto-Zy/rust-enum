#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_WRAPPING_RANGE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_WRAPPING_RANGE_HPP

#include <cstddef>

namespace rust {
/// Inclusive wrap-around range of valid values, that is, if Start > End, it represents [Start,
/// MAX], followed by [0, End].
///
/// That is, for an i8 primitive, a range of Start = 254 and End = 2 means following sequence:
///
///    254 (-2), 255 (-1), 0, 1, 2
///
/// This type comes from the implementation of rustc:
/// https://github.com/rust-lang/rust/blob/7a5d2d0138d4a3d7d97cad0ca72ab62e938e0b0b/compiler/rustc_abi/src/lib.rs#L950
template <std::size_t Start, std::size_t End>
struct wrapping_range {
    static constexpr std::size_t start = Start;
    static constexpr std::size_t end = End;
    using type = wrapping_range;
};
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_WRAPPING_RANGE_HPP
