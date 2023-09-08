#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_GET_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_GET_HPP

#include <utility>

#include <rust_enum/tuple/detail/access.hpp>
#include <rust_enum/tuple/detail/fwd/get_fwd.hpp>
#include <rust_enum/tuple/tuple_element.hpp>

namespace rust {
template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(  //
    tuple<Tys...>& t
) noexcept -> typename std::tuple_element<Idx, tuple<Tys...>>::type& {
    return t.template get<Idx>();
}

template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(  //
    tuple<Tys...>&& t
) noexcept -> typename std::tuple_element<Idx, tuple<Tys...>>::type&& {
    return std::move(t).template get<Idx>();
}

template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(  //
    tuple<Tys...> const& t
) noexcept -> typename std::tuple_element<Idx, tuple<Tys...>>::type const& {
    return t.template get<Idx>();
}

template <std::size_t Idx, class... Tys>
CONSTEXPR14 auto get(  //
    tuple<Tys...> const&& t
) noexcept -> typename std::tuple_element<Idx, tuple<Tys...>>::type const&& {
    return std::move(t).template get<Idx>();
}

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...>& t) noexcept -> Ty& {
    return t.template get<Ty>();
}

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...> const& t) noexcept -> Ty const& {
    return t.template get<Ty>();
}

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...>&& t) noexcept -> Ty&& {
    return std::move(t).template get<Ty>();
}

template <class Ty, class... Tys>
CONSTEXPR14 auto get(tuple<Tys...> const&& t) noexcept -> Ty const&& {
    return std::move(t).template get<Ty>();
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_GET_HPP
