#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_CONSTRUCTOR_HELPER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_CONSTRUCTOR_HELPER_HPP

#include <iterator>
#include <type_traits>

#include <rust_enum/variant/detail/alternative_storage.hpp>

namespace rust::detail {
template <class Ti, typename std::enable_if<std::is_reference<Ti>::value, int>::type = 0>
void narrow_conversion_checker(Ti);

template <class Ty, std::size_t Idx>
struct variant_conversion_constructor_selected_alternative {
    using type = Ty;
    using storage_type = variant_alternative_storage<type>;
    static constexpr std::size_t index = Idx;
};

template <std::size_t Idx, class Ti, class Ty, class = void>
struct check_narrow_conversion { };

// An overload F(T_i) is only considered if the declaration T_i x[] = { std::forward<T>(t) }; is
// valid for some invented variable x.
template <std::size_t Idx, class Ti, class Ty>
struct check_narrow_conversion<
    Idx,
    Ti,
    Ty,
    std::void_t<decltype(narrow_conversion_checker<Ti (&&)[1]>({ std::declval<Ty>() }))>> {
    using type = variant_conversion_constructor_selected_alternative<Ti, Idx>;
};

template <std::size_t Idx, class Ti, class Ty>
struct check_narrow_conversion<
    Idx,
    Ti&,
    Ty,
    std::void_t<decltype(narrow_conversion_checker<Ti&>(std::declval<Ty>()))>> {
    using type = variant_conversion_constructor_selected_alternative<Ti&, Idx>;
};

template <std::size_t Idx, class Ti>
struct imaginary_function_wrapper {
    template <class Ty>
    static auto imaginary_function(Ti) -> typename check_narrow_conversion<Idx, Ti, Ty>::type;
};

template <class, class...>
struct imaginary_function_overload_impl;

template <std::size_t... Is, class... Tys>
struct imaginary_function_overload_impl<std::index_sequence<Is...>, Tys...> :
    imaginary_function_wrapper<Is, Tys>... {
    using imaginary_function_wrapper<Is, Tys>::imaginary_function...;
};

template <class... Tys>
using imaginary_function_overload =
    imaginary_function_overload_impl<std::index_sequence_for<Tys...>, Tys...>;

template <class enable, class Ty, class... Tis>
struct conversion_constructor_index_selector_impl { };

template <class Ty, class... Tis>
struct conversion_constructor_index_selector_impl<
    std::void_t<decltype(imaginary_function_overload<Tis...>::template imaginary_function<
                         Ty>(std::declval<Ty>()))>,
    Ty,
    Tis...> {
    using type = decltype(imaginary_function_overload<Tis...>::template imaginary_function<Ty>(
        std::declval<Ty>()
    ));
};

// Constructs a variant holding the alternative type T_j that would be selected by overload
// resolution for the expression F(std::forward<T>(t)) if there was an overload of imaginary
// function F(T_i) for every T_i from Types... in scope at the same time.
template <class Ty, class... Tis>
using conv_ctor_index =
    typename conversion_constructor_index_selector_impl<void, Ty, Tis...>::type;

template <class Ty, class... Tys>
constexpr auto find_match_index() -> std::size_t {
    constexpr bool results[] = { std::is_same<Ty, Tys>::value... };
    std::size_t match_count = 0;
    auto result = static_cast<std::size_t>(-1);

    for (std::size_t i = 0; i != std::size(results); ++i) {
        if (results[i]) {
            result = i;
            ++match_count;
        }
    }

    if (match_count == 1)
        return result;
    return static_cast<std::size_t>(-1);
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_CONSTRUCTOR_HELPER_HPP
