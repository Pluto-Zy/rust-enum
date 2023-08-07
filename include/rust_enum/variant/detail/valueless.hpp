#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VALUELESS_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VALUELESS_HPP

#include <type_traits>

#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/valueless_strategy.hpp>

namespace rust::detail {
// Checks whether a variant must not contain valueless state.
template <
    class Variant,
    variant_valueless_strategy_t strategy = variant_valueless_strategy<Variant>::value>
struct variant_no_valueless_state;

template <class... Tys, variant_valueless_strategy_t strategy>
struct variant_no_valueless_state<variant<Tys...>, strategy> :
    std::disjunction<
        std::bool_constant<strategy == variant_valueless_strategy_t::DISALLOW_VALUELESS>,
        std::bool_constant<strategy == variant_valueless_strategy_t::FALLBACK>,
        std::conjunction<
            typename variant_alternative_storage<Tys>::is_nothrow_move_constructible...>> { };

// FIXME: Here `Is` is index, but `valueless_raw_index` is raw_index.
template <std::size_t Idx, class Storage>
struct is_valueless :
    std::bool_constant<
        !std::remove_reference<Storage>::type::no_valueless_state
        && Idx == std::remove_reference<Storage>::type::status_count - 1> { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VALUELESS_HPP
