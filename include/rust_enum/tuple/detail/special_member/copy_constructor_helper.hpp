#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_COPY_CONSTRUCTOR_HELPER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_COPY_CONSTRUCTOR_HELPER_HPP

#include <type_traits>

#include <rust_enum/tuple/detail/tuple_storage.hpp>

namespace rust::detail {
template <class... Tys>
using implicitly_copy_constructible = std::conjunction<
    typename tuple_storage<Tys>::is_copy_constructible...,
    std::is_convertible<const Tys&, Tys>...>;

template <class... Tys>
using explicitly_copy_constructible = std::conjunction<
    typename tuple_storage<Tys>::is_copy_constructible...,
    std::negation<std::conjunction<std::is_convertible<const Tys&, Tys>...>>>;

template <class... Tys>
using nothrow_copy_constructible =
    std::conjunction<typename tuple_storage<Tys>::is_nothrow_copy_constructible...>;

template <class... Tys>
using copy_constructible = std::conjunction<typename tuple_storage<Tys>::is_copy_constructible...>;

template <class... Tys>
using move_constructible = std::conjunction<typename tuple_storage<Tys>::is_move_constructible...>;
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_COPY_CONSTRUCTOR_HELPER_HPP
