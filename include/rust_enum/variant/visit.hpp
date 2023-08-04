#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VISIT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VISIT_HPP

#include <type_traits>

#include <rust_enum/variant/detail/cartesian_product.hpp>
#include <rust_enum/variant/detail/tagged_visit.hpp>
#include <rust_enum/variant/detail/visit.hpp>

namespace rust {
template <
    class Visitor,
    class... Variants,
    class = std::void_t<decltype(detail::as_variant(std::declval<Variants>()))...>>
CONSTEXPR17 auto visit(Visitor&& visitor, Variants&&... variants) -> typename detail::
    apply_result<Visitor, decltype(detail::as_variant(std::declval<Variants>()))...>::type {
    using ret_type = typename detail::apply_result<Visitor, Variants...>::type;
    // FIXME: Try moving this check into visit_impl.
    using index_sequences = typename detail::status_index_cartesian_product<
        std::remove_reference<decltype(variants.storage())>::type::status_count...>::type;
    static_assert(
        detail::visit_return_type_checker<
            Visitor,
            std::is_same,
            ret_type,
            index_sequences,
            decltype(std::forward<Variants>(variants).storage())...>::value,
        "The call is ill-formed if the invocation is not a valid expression of "
        "the same type and value category, for all combinations of "
        "alternative types of all variants."
    );

    return detail::visit_impl<ret_type, false>(
        std::forward<Visitor>(visitor),
        detail::as_variant(std::forward<Variants>(variants)).storage()...
    );
}

template <
    class Ret,
    class Visitor,
    class... Variants,
    class = std::void_t<decltype(detail::as_variant(std::declval<Variants>()))...>>
CONSTEXPR17 auto visit(Visitor&& visitor, Variants&&... variants) -> Ret {
    // TODO: Add some checks here.
    return detail::visit_impl<Ret, false>(
        std::forward<Visitor>(visitor),
        detail::as_variant(std::forward<Variants>(variants)).storage()...
    );
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VISIT_HPP
