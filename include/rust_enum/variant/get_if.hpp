#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_GET_IF_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_GET_IF_HPP

#include <memory>
#include <type_traits>

#include <rust_enum/variant/detail/access.hpp>
#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/special_member/constructor_helper.hpp>
#include <rust_enum/variant/variant_alternative.hpp>

namespace rust {
namespace detail {
template <std::size_t Idx, class Variant>
CONSTEXPR17 auto get_if_impl(Variant* pv) noexcept -> typename std::add_pointer<decltype(  //
    get_variant_tagged_content<Idx>(std::declval<Variant>().storage()).content.forward_value()
)>::type {
    return pv && pv->index() == Idx
        ? std::addressof(get_variant_tagged_content<Idx>(pv->storage()).content.forward_value())
        : nullptr;
}
}  // namespace detail

template <std::size_t Idx, class... Tys>
CONSTEXPR17 auto get_if(  //
    variant<Tys...>* pv
) noexcept -> typename std::add_pointer<variant_alternative_t<Idx, variant<Tys...>>>::type {
    static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
    return detail::get_if_impl<Idx>(pv);
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 auto get_if(  //
    const variant<Tys...>* pv
) noexcept -> typename std::add_pointer<const variant_alternative_t<Idx, variant<Tys...>>>::type {
    static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
    return detail::get_if_impl<Idx>(pv);
}

template <class Ty, class... Tys>
CONSTEXPR17 auto get_if(variant<Tys...>* pv) noexcept -> typename std::add_pointer<Ty>::type {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return detail::get_if_impl<match_index>(pv);
}

template <class Ty, class... Tys>
CONSTEXPR17 auto get_if(  //
    const variant<Tys...>* pv
) noexcept -> typename std::add_pointer<const Ty>::type {
    constexpr std::size_t match_index = detail::find_match_index<Ty, Tys...>();
    static_assert(
        match_index != static_cast<std::size_t>(-1),
        "The type Ty must occur exactly once in Tys."
    );

    return detail::get_if_impl<match_index>(pv);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_GET_IF_HPP
