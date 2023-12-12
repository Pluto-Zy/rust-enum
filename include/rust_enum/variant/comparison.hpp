#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_COMPARISON_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_COMPARISON_HPP

#include <functional>
#include <type_traits>

#include <rust_enum/variant/detail/access.hpp>
#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>
#include <rust_enum/variant/detail/tagged_reference.hpp>
#include <rust_enum/variant/detail/tagged_visit.hpp>

#ifdef USE_CXX20
    #include <compare>
#endif

namespace rust {
namespace detail {
template <class Op, class Variant>
struct variant_comparison_impl {
    const Variant& lhs;

    template <std::size_t Idx, class AltStorageRef>
    CONSTEXPR17 auto operator()(tagged_reference<Idx, AltStorageRef> rhs) const -> bool {
        if constexpr (std::is_same<AltStorageRef, valueless_tag>::value) {
            unreachable();
        } else {
            return Op {}(
                get_variant_tagged_content<Idx>(lhs.storage()).forward_content().forward_value(),
                rhs.forward_content().forward_value()
            );
        }
    }
};
}  // namespace detail

template <class... Tys>
CONSTEXPR17 auto operator==(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<
        std::is_nothrow_invocable_r<bool, std::equal_to<>, const Tys&, const Tys&>...>::value
) -> bool {
    return lhs.index() == rhs.index()
        && (lhs.valueless_by_exception()
            || detail::tagged_visit(
                detail::variant_comparison_impl<std::equal_to<>, variant<Tys...>> { lhs },
                rhs.storage()
            ));
}

template <class... Tys>
CONSTEXPR17 auto operator!=(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<
        std::is_nothrow_invocable_r<bool, std::not_equal_to<>, const Tys&, const Tys&>...>::value
) -> bool {
    return lhs.index() != rhs.index()
        || (!lhs.valueless_by_exception()
            && detail::tagged_visit(
                detail::variant_comparison_impl<std::not_equal_to<>, variant<Tys...>> { lhs },
                rhs.storage()
            ));
}

template <class... Tys>
CONSTEXPR17 auto operator<(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<
        std::is_nothrow_invocable_r<bool, std::less<>, const Tys&, const Tys&>...>::value
) -> bool {
    if (rhs.valueless_by_exception())
        return false;
    if (lhs.valueless_by_exception())
        return true;
    return lhs.index() < rhs.index()
        || (lhs.index() == rhs.index()
            && detail::tagged_visit(
                detail::variant_comparison_impl<std::less<>, variant<Tys...>> { lhs },
                rhs.storage()
            ));
}

template <class... Tys>
CONSTEXPR17 auto operator>(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<
        std::is_nothrow_invocable_r<bool, std::greater<>, const Tys&, const Tys&>...>::value
) -> bool {
    if (lhs.valueless_by_exception())
        return false;
    if (rhs.valueless_by_exception())
        return true;
    return lhs.index() > rhs.index()
        || (lhs.index() == rhs.index()
            && detail::tagged_visit(
                detail::variant_comparison_impl<std::greater<>, variant<Tys...>> { lhs },
                rhs.storage()
            ));
}

template <class... Tys>
CONSTEXPR17 auto operator<=(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<
        std::is_nothrow_invocable_r<bool, std::less_equal<>, const Tys&, const Tys&>...>::value
) -> bool {
    if (lhs.valueless_by_exception())
        return true;
    if (rhs.valueless_by_exception())
        return false;
    return lhs.index() < rhs.index()
        || (lhs.index() == rhs.index()
            && detail::tagged_visit(
                detail::variant_comparison_impl<std::less_equal<>, variant<Tys...>> { lhs },
                rhs.storage()
            ));
}

template <class... Tys>
CONSTEXPR17 auto operator>=(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<
        std::is_nothrow_invocable_r<bool, std::greater_equal<>, const Tys&, const Tys&>...>::value
) -> bool {
    if (rhs.valueless_by_exception())
        return true;
    if (lhs.valueless_by_exception())
        return false;
    return lhs.index() > rhs.index()
        || (lhs.index() == rhs.index()
            && detail::tagged_visit(
                detail::variant_comparison_impl<std::greater_equal<>, variant<Tys...>> { lhs },
                rhs.storage()
            ));
}

#ifdef USE_CXX20
namespace detail {
template <class Variant, class Ret>
struct variant_three_way_comparison_impl {
    const Variant& lhs;

    template <std::size_t Idx, class AltStorageRef>
    CONSTEXPR20 auto operator()(tagged_reference<Idx, AltStorageRef> rhs) const -> Ret {
        if constexpr (std::is_same<AltStorageRef, valueless_tag>::value) {
            unreachable();
        } else {
            return std::compare_three_way {}(
                get_variant_tagged_content<Idx>(lhs.storage()).forward_content().forward_value(),
                rhs.forward_content().forward_value()
            );
        }
    }
};
}  // namespace detail

template <class... Tys>
    requires(std::three_way_comparable<Tys> && ...)
CONSTEXPR20 auto operator<=>(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable_r<
        std::common_comparison_category_t<std::compare_three_way_result_t<Tys>...>,
        std::compare_three_way,
        const Tys&,
        const Tys&>...>::value
) -> std::common_comparison_category_t<std::compare_three_way_result_t<Tys>...> {
    if (lhs.valueless_by_exception() && rhs.valueless_by_exception())
        return std::strong_ordering::equal;
    if (lhs.valueless_by_exception())
        return std::strong_ordering::less;
    if (rhs.valueless_by_exception())
        return std::strong_ordering::greater;
    if (auto const c = lhs.index() <=> rhs.index(); c != 0)
        return c;

    using ret_type = std::common_comparison_category_t<std::compare_three_way_result_t<Tys>...>;
    return detail::tagged_visit(
        detail::variant_three_way_comparison_impl<variant<Tys...>, ret_type> { lhs },
        rhs.storage()
    );
}
#endif  // USE_CXX20
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_COMPARISON_HPP
