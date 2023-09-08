#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_FWD_GET_FWD_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_FWD_GET_FWD_HPP

#include <cstddef>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/tuple_element.hpp>
#include <rust_enum/variant/detail/macro.hpp>

namespace rust {
template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...>&) noexcept ->
    typename std::tuple_element<Idx, tuple<Tys...>>::type&;

template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...>&&) noexcept ->
    typename std::tuple_element<Idx, tuple<Tys...>>::type&&;

template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...> const&) noexcept ->
    typename std::tuple_element<Idx, tuple<Tys...>>::type const&;

template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...> const&&) noexcept ->
    typename std::tuple_element<Idx, tuple<Tys...>>::type const&&;

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...>&) noexcept -> Ty&;

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...>&&) noexcept -> Ty&&;

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...> const&) noexcept -> Ty const&;

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...> const&&) noexcept -> Ty const&&;
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_FWD_GET_FWD_HPP
