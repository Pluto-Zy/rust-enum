#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_GET_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_GET_HPP

#include <type_traits>

#include <rust_enum/variant/bad_variant_access.hpp>
#include <rust_enum/variant/detail/access.hpp>
#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/special_member/constructor_helper.hpp>
#include <rust_enum/variant/variant_alternative.hpp>

namespace rust {
namespace detail {
template <std::size_t Idx, class Variant>
CONSTEXPR17 auto get_impl(Variant&& var) -> decltype(  //
    get_variant_tagged_content<Idx>(std::declval<Variant>().storage())
        .forward_content()
        .forward_value()
) {
    if (var.index() != Idx)
        throw_bad_variant_access();
    return get_variant_tagged_content<Idx>(std::forward<Variant>(var).storage())
        .forward_content()
        .forward_value();
}
}  // namespace detail

template <std::size_t Idx, class... Tys>
CONSTEXPR17 auto get(variant<Tys...>& v) -> variant_alternative_t<Idx, variant<Tys...>>& {
    static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
    return detail::get_impl<Idx>(v);
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 auto get(variant<Tys...>&& v) -> variant_alternative_t<Idx, variant<Tys...>>&& {
    static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
    return detail::get_impl<Idx>(std::move(v));
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 auto get(  //
    const variant<Tys...>& v
) -> const variant_alternative_t<Idx, variant<Tys...>>& {
    static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
    return detail::get_impl<Idx>(v);
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 auto get(  //
    const variant<Tys...>&& v
) -> const variant_alternative_t<Idx, variant<Tys...>>&& {
    static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
    return detail::get_impl<Idx>(std::move(v));
}

template <class Ty, class... Tys>
CONSTEXPR17 auto get(variant<Tys...>& v) -> Ty& {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return detail::get_impl<match_index>(v);
}

template <class Ty, class... Tys>
CONSTEXPR17 auto get(variant<Tys...>&& v) -> Ty&& {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return detail::get_impl<match_index>(std::move(v));
}

template <class Ty, class... Tys>
CONSTEXPR17 auto get(const variant<Tys...>& v) -> const Ty& {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return detail::get_impl<match_index>(v);
}

template <class Ty, class... Tys>
CONSTEXPR17 auto get(const variant<Tys...>&& v) -> const Ty&& {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return detail::get_impl<match_index>(std::move(v));
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_GET_HPP
