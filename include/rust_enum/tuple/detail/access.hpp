#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_ACCESS_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_ACCESS_HPP

#include <cstddef>
#include <utility>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/fwd/tuple_impl_fwd.hpp>
#include <rust_enum/tuple/tuple_element.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <std::size_t Idx, class TupleImpl>
struct tuple_element_helper { };

template <std::size_t Idx, class... Ts>
struct tuple_element_helper<Idx, tuple_impl<Ts...>&> {
    using type = typename std::tuple_element<Idx, tuple<Ts...>>::type&;
};

template <std::size_t Idx, class... Ts>
struct tuple_element_helper<Idx, tuple_impl<Ts...>&&> {
    using type = typename std::tuple_element<Idx, tuple<Ts...>>::type&&;
};

template <std::size_t Idx, class... Ts>
struct tuple_element_helper<Idx, tuple_impl<Ts...> const&> {
    using type = typename std::tuple_element<Idx, tuple<Ts...>>::type const&;
};

template <std::size_t Idx, class... Ts>
struct tuple_element_helper<Idx, tuple_impl<Ts...> const&&> {
    using type = typename std::tuple_element<Idx, tuple<Ts...>>::type const&&;
};

template <std::size_t Idx, class TupleImpl>
CONSTEXPR11 auto get_tuple_content(TupleImpl&& impl) noexcept ->
    typename tuple_element_helper<Idx, TupleImpl>::type {
    if constexpr (Idx == 0) {
        return std::forward<TupleImpl>(impl).value.forward_value();
    } else if constexpr (Idx == 1) {
        return std::forward<TupleImpl>(impl).tails.value.forward_value();
    } else if constexpr (Idx == 2) {
        return std::forward<TupleImpl>(impl).tails.tails.value.forward_value();
    } else if constexpr (Idx == 3) {
        return std::forward<TupleImpl>(impl).tails.tails.tails.value.forward_value();
    } else if constexpr (Idx == 4) {
        return std::forward<TupleImpl>(impl).tails.tails.tails.tails.value.forward_value();
    } else if constexpr (Idx == 5) {
        return std::forward<TupleImpl>(impl).tails.tails.tails.tails.tails.value.forward_value();
    } else if constexpr (Idx == 6) {
        return std::forward<TupleImpl>(impl)
            .tails.tails.tails.tails.tails.tails.value.forward_value();
    } else if constexpr (Idx == 7) {
        return std::forward<TupleImpl>(impl)
            .tails.tails.tails.tails.tails.tails.tails.value.forward_value();
    } else {
        return get_tuple_content<Idx - 8>(
            std::forward<TupleImpl>(impl).tails.tails.tails.tails.tails.tails.tails.tails
        );
    }
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_ACCESS_HPP
