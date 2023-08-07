#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VALUELESS_STRATEGY_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VALUELESS_STRATEGY_HPP

namespace rust {
/// Defines the processing strategy when the variant will become valueless.
enum class variant_valueless_strategy_t {
    /// If it was possible to get rid of valueless state while providing strong exception-safety
    /// guarantee, it would do so.
    LET_VARIANT_DECIDE,
    /// Disallow variants from being valueless. If the variant will never becomes valueless (for
    /// example, has a non-throwing move constructor), the variant still provides the strong
    /// exception-safety guarantee. Otherwise, in order to get rid of the valueless state, no
    /// guarantees will be provided.
    DISALLOW_VALUELESS,
    /// When going to be valueless, fall back to the first member. This requires the first member
    /// to be nothrow-default-constructible. This applies to optional: when an exception is thrown
    /// during construction, fall back to the empty member.
    FALLBACK,
};

template <class Variant>
struct variant_valueless_strategy {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::LET_VARIANT_DECIDE;
};
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VALUELESS_STRATEGY_HPP
