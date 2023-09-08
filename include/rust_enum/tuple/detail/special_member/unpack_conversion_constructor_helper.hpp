#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_UNPACK_CONVERSION_CONSTRUCTOR_HELPER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_UNPACK_CONVERSION_CONSTRUCTOR_HELPER_HPP

#include <cstddef>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/special_member/conversion_constructor_helper.hpp>
#include <rust_enum/tuple/detail/universal_get.hpp>

namespace rust::detail {
template <class Is, class SelfTuple, class OtherTuple>
struct implicitly_unpack_conv_constructible;

template <std::size_t... Is, class SelfTuple, class OtherTuple>
struct implicitly_unpack_conv_constructible<std::index_sequence<Is...>, SelfTuple, OtherTuple> :
    implicitly_conv_constructible<
        SelfTuple,
        decltype(detail::universal_get<Is>(std::declval<OtherTuple>()))...> { };

template <class Is, class SelfTuple, class OtherTuple>
struct explicitly_unpack_conv_constructible;

template <std::size_t... Is, class SelfTuple, class OtherTuple>
struct explicitly_unpack_conv_constructible<std::index_sequence<Is...>, SelfTuple, OtherTuple> :
    explicitly_conv_constructible<
        SelfTuple,
        decltype(detail::universal_get<Is>(std::declval<OtherTuple>()))...> { };

template <class Is, class SelfTuple, class OtherTuple>
struct nothrow_unpack_conv_constructible;

template <std::size_t... Is, class SelfTuple, class OtherTuple>
struct nothrow_unpack_conv_constructible<std::index_sequence<Is...>, SelfTuple, OtherTuple> :
    nothrow_conv_constructible<
        SelfTuple,
        decltype(detail::universal_get<Is>(std::declval<OtherTuple>()))...> { };

template <class Self, class Other, class... Us>
struct unpack_conv_ctor_constraint : std::false_type { };

template <class T0, class... Ts, class Other, class U0, class... Us>
struct unpack_conv_ctor_constraint<tuple<T0, Ts...>, Other, U0, Us...> :
    std::conjunction<
        std::bool_constant<sizeof...(Ts) == sizeof...(Us)>,
        std::disjunction<
            std::bool_constant<sizeof...(Ts) != 0>,
            std::negation<std::disjunction<
                std::is_convertible<Other, T0>,
                std::is_constructible<T0, Other>,
                std::is_same<T0, U0>>>>> { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_UNPACK_CONVERSION_CONSTRUCTOR_HELPER_HPP
