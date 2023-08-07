#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_DESTRUCTIBLE_UNINITIALIZED_UNION_FWD_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_DESTRUCTIBLE_UNINITIALIZED_UNION_FWD_HPP

#include <type_traits>

namespace rust::detail {
template <bool IsTriviallyDestructible, class... AltStorages>
union variant_destructible_uninitialized_union;

template <class>
struct is_specialization_of_variant_destructible_uninitialized_union : std::false_type { };

template <bool IsTriviallyDestructible, class... Tys>
struct is_specialization_of_variant_destructible_uninitialized_union<
    variant_destructible_uninitialized_union<IsTriviallyDestructible, Tys...>> :
    std::true_type { };

template <class... AltStorages>
using variant_destructible_uninitialized_union_t = variant_destructible_uninitialized_union<
    std::conjunction<typename AltStorages::is_trivially_destructible...>::value,
    AltStorages...>;
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_DESTRUCTIBLE_UNINITIALIZED_UNION_FWD_HPP
