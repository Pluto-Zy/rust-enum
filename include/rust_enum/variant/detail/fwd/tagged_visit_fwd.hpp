#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_TAGGED_VISIT_FWD_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_TAGGED_VISIT_FWD_HPP

#include <functional>

#include <rust_enum/variant/detail/access.hpp>

namespace rust::detail {
template <class Fn, class... Storages>
struct tagged_apply_result :
    std::invoke_result<
        Fn&&,  //
        decltype(get_variant_tagged_content<0>(std::declval<Storages>()))...> { };

template <class Fn, class... Storages>
CONSTEXPR17 auto tagged_visit(  //
    Fn&& func,
    Storages&&... storages
) -> typename tagged_apply_result<Fn, Storages...>::type;
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_TAGGED_VISIT_FWD_HPP
