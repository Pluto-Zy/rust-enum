#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_STATUS_COUNT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_STATUS_COUNT_HPP

#include <type_traits>

#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/valueless.hpp>

namespace rust::detail {
template <class... AltStorages>
struct variant_status_count :
    std::integral_constant<
        std::size_t,
        variant_no_valueless_state<variant<typename AltStorages::value_type...>>::value
            ? sizeof...(AltStorages)
            : sizeof...(AltStorages) + 1> { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_STATUS_COUNT_HPP
