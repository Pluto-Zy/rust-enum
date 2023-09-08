#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_CAT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_CAT_HPP

#include <rust_enum/tuple/detail/tuple_cat.hpp>
#include <rust_enum/tuple/forward_as_tuple.hpp>

namespace rust {
template <class... Tuples>
auto tuple_cat(Tuples&&... tuples) -> detail::tuple_cat_result_of<Tuples...> {
    using ret = detail::tuple_cat_result_of<Tuples...>;
    using helper = detail::tuple_cat_helper_of<Tuples...>;
    using tuple_index_sequence = typename helper::tuple_index_sequence;
    using element_index_sequence = typename helper::element_index_sequence;

    return detail::tuple_cat_impl<ret>(
        rust::forward_as_tuple(std::forward<Tuples>(tuples)...),
        tuple_index_sequence(),
        element_index_sequence()
    );
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_CAT_HPP
