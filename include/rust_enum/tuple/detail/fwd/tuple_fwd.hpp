#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_FWD_TUPLE_FWD_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_FWD_TUPLE_FWD_HPP

#include <type_traits>

namespace rust {
template <class...>
class tuple;

namespace detail {
template <class Tuple>
struct is_specialization_of_tuple : std::false_type { };

template <class... Tys>
struct is_specialization_of_tuple<tuple<Tys...>> : std::true_type { };
}  // namespace detail
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_FWD_TUPLE_FWD_HPP
