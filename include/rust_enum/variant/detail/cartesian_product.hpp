#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_CARTESIAN_PRODUCT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_CARTESIAN_PRODUCT_HPP

#include <type_traits>
#include <utility>

namespace rust::detail {
/// The following piece of code produces a Cartesian product of type indices. For example, given
/// variant<T1, T2>, variant<W1, W2, W3>, variant<U1, U2>, when visiting these variants, we will
/// generate 2*3*2=12 combinations of states. The content of these combinations can be obtained
/// through status_index_cartesian_product<2, 3, 2>::type, which will be
///
/// index_sequence_list<
///     std::index_sequence<0, 0, 0>, std::index_sequence<0, 0, 1>,
///     std::index_sequence<0, 1, 0>, std::index_sequence<0, 1, 1>,
///     std::index_sequence<0, 2, 0>, std::index_sequence<0, 2, 1>,
///     std::index_sequence<1, 0, 0>, std::index_sequence<1, 0, 1>,
///     std::index_sequence<1, 1, 0>, std::index_sequence<1, 1, 1>,
///     std::index_sequence<1, 2, 0>, std::index_sequence<1, 2, 1>
/// >.

template <class...>
struct index_sequence_list { };

template <class...>
struct index_sequence_list_concat;

template <class List>
struct index_sequence_list_concat<List> {
    using type = List;
};

template <class... HeadIndexSequence, class... NextIndexSequence, class... TailLists>
struct index_sequence_list_concat<
    index_sequence_list<HeadIndexSequence...>,
    index_sequence_list<NextIndexSequence...>,
    TailLists...> :
    index_sequence_list_concat<
        index_sequence_list<HeadIndexSequence..., NextIndexSequence...>,
        TailLists...> { };

template <class...>
struct status_index_cartesian_product_impl {
    using type = index_sequence_list<std::index_sequence<>>;
};

template <std::size_t... Already, std::size_t... Current>
struct status_index_cartesian_product_impl<
    std::index_sequence<Already...>,
    std::index_sequence<Current...>> {
    using type = index_sequence_list<std::index_sequence<Already..., Current>...>;
};

template <std::size_t... Already, std::size_t... Current, class... Others>
struct status_index_cartesian_product_impl<
    std::index_sequence<Already...>,
    std::index_sequence<Current...>,
    Others...> :
    index_sequence_list_concat<typename status_index_cartesian_product_impl<
        std::index_sequence<Already..., Current>,
        Others...>::type...> { };

template <std::size_t... StatusCount>
struct status_index_cartesian_product :
    status_index_cartesian_product_impl<
        std::index_sequence<>,
        std::make_index_sequence<StatusCount>...> { };

struct index_sequence_list_iterator_end;

template <class List>
struct index_sequence_list_iterator {
    using type = index_sequence_list_iterator_end;
    using next = index_sequence_list_iterator<List>;
};

template <class Head, class... Tails>
struct index_sequence_list_iterator<index_sequence_list<Head, Tails...>> {
    // dereference
    using type = Head;
    using next = index_sequence_list_iterator<index_sequence_list<Tails...>>;
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_CARTESIAN_PRODUCT_HPP
