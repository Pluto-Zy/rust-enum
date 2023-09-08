#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_ASSIGNMENT_HELPER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_ASSIGNMENT_HELPER_HPP

#include <type_traits>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/tuple_storage.hpp>
#include <rust_enum/tuple/detail/universal_get.hpp>

namespace rust::detail {
template <class... Ts>
using copy_assignable = std::conjunction<typename tuple_storage<Ts>::is_copy_assignable...>;

template <class... Ts>
using nothrow_copy_assignable =
    std::conjunction<typename tuple_storage<Ts>::is_nothrow_copy_assignable...>;

template <class... Ts>
using move_assignable = std::conjunction<typename tuple_storage<Ts>::is_move_assignable...>;

template <class... Ts>
using nothrow_move_assignable =
    std::conjunction<typename tuple_storage<Ts>::is_nothrow_move_assignable...>;

template <class IndexSeq, class SelfTuple, class OtherTuple>
struct unpack_conv_assignable;

template <std::size_t... Is, class... Ts, class OtherTuple>
struct unpack_conv_assignable<std::index_sequence<Is...>, tuple<Ts...>, OtherTuple> :
    std::conjunction<typename tuple_storage<Ts>::template is_assignable<
        decltype(detail::universal_get<Is>(std::declval<OtherTuple>()))>...> { };

template <class IndexSeq, class SelfTuple, class OtherTuple>
struct nothrow_unpack_conv_assignable;

template <std::size_t... Is, class... Ts, class OtherTuple>
struct nothrow_unpack_conv_assignable<std::index_sequence<Is...>, tuple<Ts...>, OtherTuple> :
    std::conjunction<typename tuple_storage<Ts>::template is_nothrow_assignable<
        decltype(detail::universal_get<Is>(std::declval<OtherTuple>()))>...> { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_ASSIGNMENT_HELPER_HPP
