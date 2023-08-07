#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VISIT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VISIT_HPP

#include <type_traits>

#include <rust_enum/variant/detail/access.hpp>
#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>

namespace rust::detail {
// The implementation of as-variant.
template <class... Tys>
CONSTEXPR17 auto as_variant(variant<Tys...>& var) -> variant<Tys...>& {
    return var;
}

template <class... Tys>
CONSTEXPR17 auto as_variant(const variant<Tys...>& var) -> const variant<Tys...>& {
    return var;
}

template <class... Tys>
CONSTEXPR17 auto as_variant(variant<Tys...>&& var) -> variant<Tys...>&& {
    return std::move(var);
}

template <class... Tys>
CONSTEXPR17 auto as_variant(const variant<Tys...>&& var) -> const variant<Tys...>&& {
    return std::move(var);
}

template <class Fn, class... Variants>
struct apply_result :
    std::invoke_result<
        Fn&&,
        decltype(get_variant_tagged_content<0>(std::declval<Variants>().storage())
                     .forward_content()
                     .forward_value())...> { };

template <
    class Fn,
    template <class...>
    class check_trait,
    class Check,
    class IndexSequenceList,
    class... Storages>
struct visit_return_type_checker;

template <class Fn, class IndexSequence, class... Storages>
struct single_visit_return_type;

template <class Fn, std::size_t... Is, class... Storages>
struct single_visit_return_type<Fn, std::index_sequence<Is...>, Storages...> :
    std::invoke_result<
        Fn&&,
        decltype(get_variant_tagged_content<(
                     std::remove_reference<Storages>::type::valueless_raw_index == Is ? 0 : Is
                 )>(std::declval<Storages>())
                     .forward_content()
                     .forward_value())...> { };

template <
    class Fn,
    template <class...>
    class check_trait,
    class CheckBase,
    class... IndexSequences,
    class... Storages>
struct visit_return_type_checker<
    Fn,
    check_trait,
    CheckBase,
    index_sequence_list<IndexSequences...>,
    Storages...> :
    std::conjunction<check_trait<
        CheckBase,
        typename single_visit_return_type<Fn, IndexSequences, Storages...>::type>...> { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VISIT_HPP
