#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_IMPL_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_IMPL_HPP

#include <type_traits>

#include <rust_enum/tuple/detail/fwd/tuple_impl_fwd.hpp>
#include <rust_enum/tuple/detail/tuple_storage.hpp>
#include <rust_enum/tuple/detail/universal_get.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <class Head, class... Tails>
struct tuple_impl<Head, Tails...> {
    tuple_storage<Head> value;
    tuple_impl<Tails...> tails;

    tuple_impl() = default;

    template <class Alloc>
    CONSTEXPR11 explicit tuple_impl(std::allocator_arg_t, const Alloc& alloc) :
        value(std::allocator_arg, alloc), tails(std::allocator_arg, alloc) { }

    template <
        class First,
        class... Last,
        typename std::enable_if<
            std::conjunction<
                std::negation<std::is_same<typename remove_cvref<First>::type, tuple_impl>>,
                typename tuple_storage<Head>::template is_constructible<First>>::value,
            int>::type = 0>
    CONSTEXPR11 explicit tuple_impl(First&& first, Last&&... last) :
        value(std::forward<First>(first)), tails(std::forward<Last>(last)...) { }

    template <class Alloc, class First, class... Last>
    CONSTEXPR20 explicit tuple_impl(
        std::allocator_arg_t,
        const Alloc& alloc,
        First&& first,
        Last&&... last
    ) :
        value(std::allocator_arg, alloc, std::forward<First>(first)),
        tails(std::allocator_arg, alloc, std::forward<Last>(last)...) { }

    template <class First, class... Last>
    void assign_from(First&& first, Last&&... last) {
        value = std::forward<First>(first);
        tails.assign_from(std::forward<Last>(last)...);
    }

    template <std::size_t... Is, class Tuple>
    void unpack_assign_from(std::index_sequence<Is...>, Tuple&& other) {
        assign_from(detail::universal_get<Is>(std::forward<Tuple>(other))...);
    }

    CONSTEXPR20 void swap(tuple_impl& other) {
        value.swap(other.value);
        tails.swap(other.tails);
    }
};

template <class Head>
struct tuple_impl<Head> {
    tuple_storage<Head> value;

    tuple_impl() = default;

    template <class Alloc>
    CONSTEXPR11 explicit tuple_impl(std::allocator_arg_t, const Alloc& alloc) :
        value(std::allocator_arg, alloc) { }

    template <
        class Arg,
        typename std::enable_if<
            std::conjunction<
                std::negation<std::is_same<typename remove_cvref<Arg>::type, tuple_impl>>,
                typename tuple_storage<Head>::template is_constructible<Arg>>::value,
            int>::type = 0>
    CONSTEXPR11 explicit tuple_impl(Arg&& arg) : value(std::forward<Arg>(arg)) { }

    template <class Alloc, class Arg>
    CONSTEXPR20 explicit tuple_impl(std::allocator_arg_t, const Alloc& alloc, Arg&& arg) :
        value(std::allocator_arg, alloc, std::forward<Arg>(arg)) { }

    template <class Arg>
    void assign_from(Arg&& arg) {
        value = std::forward<Arg>(arg);
    }

    CONSTEXPR20 void swap(tuple_impl& other) {
        value.swap(other.value);
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_IMPL_HPP
