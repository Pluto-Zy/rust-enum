#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_MEMBER_OFFSET_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_MEMBER_OFFSET_HPP

#include <cstddef>
#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>

namespace rust::detail {
template <
    std::size_t Idx,
    class TupleImpl,
    typename std::enable_if<std::is_standard_layout<TupleImpl>::value, int>::type = 0>
CONSTEXPR11 auto get_tuple_member_offset(const TupleImpl& impl) noexcept -> std::size_t {
    if constexpr (Idx == 0) {
        return offsetof(TupleImpl, value);
    } else if constexpr (Idx == 1) {
        return offsetof(TupleImpl, tails.value);
    } else if constexpr (Idx == 2) {
        return offsetof(TupleImpl, tails.tails.value);
    } else if constexpr (Idx == 3) {
        return offsetof(TupleImpl, tails.tails.tails.value);
    } else if constexpr (Idx == 4) {
        return offsetof(TupleImpl, tails.tails.tails.tails.value);
    } else if constexpr (Idx == 5) {
        return offsetof(TupleImpl, tails.tails.tails.tails.tails.value);
    } else if constexpr (Idx == 6) {
        return offsetof(TupleImpl, tails.tails.tails.tails.tails.tails.value);
    } else if constexpr (Idx == 7) {
        return offsetof(TupleImpl, tails.tails.tails.tails.tails.tails.tails.value);
    } else {
        return get_tuple_member_offset<Idx - 8>(
                   impl.tails.tails.tails.tails.tails.tails.tails.tails
               )
            + offsetof(TupleImpl, tails.tails.tails.tails.tails.tails.tails.tails);
    }
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_MEMBER_OFFSET_HPP
