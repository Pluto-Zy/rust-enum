#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_CONVERSION_CONSTRUCTOR_HELPER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_CONVERSION_CONSTRUCTOR_HELPER_HPP

#include <memory>
#include <tuple>
#include <type_traits>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/tuple_storage.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <class Tuple, class... Tys>
struct implicitly_conv_constructible;

template <class... Ts, class... Us>
struct implicitly_conv_constructible<tuple<Ts...>, Us...> :
    std::conjunction<
        typename tuple_storage<Ts>::template is_constructible<Us>...,
        std::is_convertible<Us, Ts>...> { };

template <class Tuple, class... Tys>
struct explicitly_conv_constructible;

template <class... Ts, class... Us>
struct explicitly_conv_constructible<tuple<Ts...>, Us...> :
    std::conjunction<
        typename tuple_storage<Ts>::template is_constructible<Us>...,
        std::negation<std::conjunction<std::is_convertible<Us, Ts>...>>> { };

template <class Tuple, class... Tys>
struct nothrow_conv_constructible;

template <class... Ts, class... Us>
struct nothrow_conv_constructible<tuple<Ts...>, Us...> :
    std::conjunction<typename tuple_storage<Ts>::template is_nothrow_constructible<Us>...> { };

template <std::size_t ArgCount, class Tuple, class T0, class DecayedU0>
struct conv_ctor_constraint_impl : std::true_type { };

template <class Tuple, class T0, class DecayedU0>
struct conv_ctor_constraint_impl<1, Tuple, T0, DecayedU0> :
    std::negation<std::is_same<DecayedU0, Tuple>> { };

template <class Tuple, class T0, class DecayedU0>
struct conv_ctor_constraint_impl<2, Tuple, T0, DecayedU0> :
    std::disjunction<
        std::negation<std::is_same<DecayedU0, std::allocator_arg_t>>,
        std::is_same<T0, std::allocator_arg_t>> { };

template <class Tuple, class T0, class DecayedU0>
struct conv_ctor_constraint_impl<3, Tuple, T0, DecayedU0> :
    conv_ctor_constraint_impl<2, Tuple, T0, DecayedU0> { };

template <class Self, class... Us>
struct conv_ctor_constraint : std::false_type { };

template <class T0, class... Ts, class U0, class... Us>
struct conv_ctor_constraint<tuple<T0, Ts...>, U0, Us...> :
    std::conjunction<
        std::bool_constant<sizeof...(Ts) == sizeof...(Us)>,
        conv_ctor_constraint_impl<
            sizeof...(Ts) + 1,
            tuple<T0, Ts...>,
            T0,
            typename remove_cvref<U0>::type>> { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_CONVERSION_CONSTRUCTOR_HELPER_HPP
