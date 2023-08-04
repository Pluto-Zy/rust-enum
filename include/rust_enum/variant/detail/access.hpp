#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_ACCESS_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_ACCESS_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

#include <rust_enum/variant/detail/alternative_storage.hpp>
#include <rust_enum/variant/detail/fwd/destructible_uninitialized_union_fwd.hpp>
#include <rust_enum/variant/detail/tagged_reference.hpp>
#include <rust_enum/variant/detail/valueless.hpp>

namespace rust::detail {
template <std::size_t Idx, class Union>
struct variant_element_helper;

template <std::size_t Idx, class... AltStorages>
struct variant_element_helper<Idx, variant_destructible_uninitialized_union_t<AltStorages...>&> {
    using raw_type = variant_alternative_t<Idx, variant<typename AltStorages::value_type...>>;
    using reference_type = variant_alternative_storage<raw_type>&;
};

template <std::size_t Idx, class... AltStorages>
struct variant_element_helper<
    Idx,
    variant_destructible_uninitialized_union_t<AltStorages...> const&> {
    using raw_type = variant_alternative_t<Idx, variant<typename AltStorages::value_type...>>;
    using reference_type = variant_alternative_storage<raw_type> const&;
};

template <std::size_t Idx, class... AltStorages>
struct variant_element_helper<Idx, variant_destructible_uninitialized_union_t<AltStorages...>&&> {
    using raw_type = variant_alternative_t<Idx, variant<typename AltStorages::value_type...>>;
    using reference_type = variant_alternative_storage<raw_type>&&;
};

template <std::size_t Idx, class... AltStorages>
struct variant_element_helper<
    Idx,
    variant_destructible_uninitialized_union_t<AltStorages...> const&&> {
    using raw_type = variant_alternative_t<Idx, variant<typename AltStorages::value_type...>>;
    using reference_type = variant_alternative_storage<raw_type> const&&;
};

/// This function is used to get the pointer to the Idx-th member of the
/// variant. It is functionally equivalent to the following implementation:
///
/// template <std::size_t Idx, class Union,
///           class Trait = typename std::enable_if<
///               is_specialization_of_variant_destructible_uninitialized_union<
///                   typename remove_cvref<Union>::type>::value,
///               variant_element_helper<Idx, Union&&>>::type>
/// typename Trait::reference_type
/// get_variant_content(Union&& _union) noexcept {
///   return static_cast<typename Trait::reference_type>(
///       *reinterpret_cast<typename Trait::storage_type*>(&_union));
/// }
///
/// However, we cannot use `reinterpret_cast` in constexpr function.
template <
    std::size_t Idx,
    class Union,
    class Trait = typename std::enable_if<
        is_specialization_of_variant_destructible_uninitialized_union<
            typename remove_cvref<Union>::type>::value,
        variant_element_helper<Idx, Union&&>>::type>
CONSTEXPR17 auto get_variant_content(Union&& _union) noexcept -> typename Trait::reference_type {
    if constexpr (Idx == 0) {
        return std::forward<Union>(_union).value;
    } else if constexpr (Idx == 1) {
        return std::forward<Union>(_union).tail.value;
    } else if constexpr (Idx == 2) {
        return std::forward<Union>(_union).tail.tail.value;
    } else if constexpr (Idx == 3) {
        return std::forward<Union>(_union).tail.tail.tail.value;
    } else if constexpr (Idx == 4) {
        return std::forward<Union>(_union).tail.tail.tail.tail.value;
    } else if constexpr (Idx == 5) {
        return std::forward<Union>(_union).tail.tail.tail.tail.tail.value;
    } else if constexpr (Idx == 6) {
        return std::forward<Union>(_union).tail.tail.tail.tail.tail.tail.value;
    } else if constexpr (Idx == 7) {
        return std::forward<Union>(_union).tail.tail.tail.tail.tail.tail.tail.value;
    } else {
        return get_variant_content<Idx - 8>(
            std::forward<Union>(_union).tail.tail.tail.tail.tail.tail.tail.tail
        );
    }
}

template <std::size_t Idx, class Storage, bool = is_valueless<Idx, Storage>::value>
struct get_variant_tagged_content_impl {
    using ref_type = decltype(get_variant_content<Idx>(std::declval<Storage>().value_storage()));

    static CONSTEXPR17 auto call(Storage&& storage) noexcept -> tagged_reference<Idx, ref_type> {
        return tagged_reference<Idx, ref_type> {
            get_variant_content<Idx>(std::forward<Storage>(storage).value_storage())
        };
    }
};

template <std::size_t Idx, class Storage>
struct get_variant_tagged_content_impl<Idx, Storage, true> {
    static CONSTEXPR17 auto call(Storage&&) noexcept -> tagged_reference<Idx, valueless_tag> {
        return tagged_reference<Idx, valueless_tag> {};
    }
};

template <std::size_t Idx, class Storage>
CONSTEXPR17 auto get_variant_tagged_content(Storage&& storage) noexcept
    -> decltype(get_variant_tagged_content_impl<Idx, Storage>::call(std::declval<Storage>())) {
    return get_variant_tagged_content_impl<Idx, Storage>::call(std::forward<Storage>(storage));
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_ACCESS_HPP
