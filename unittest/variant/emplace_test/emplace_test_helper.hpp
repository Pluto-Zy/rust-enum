#ifndef RUST_ENUM_EMPLACE_TEST_HELPER_HPP
#define RUST_ENUM_EMPLACE_TEST_HELPER_HPP

#include <type_traits>

namespace rust {
template <
    class Variant,
    std::size_t Index,
    class... Args,
    class = std::void_t<
        decltype(std::declval<Variant>().template emplace<Index>(std::declval<Args>()...))>>
std::true_type has_index_emplace_impl(int);

template <class, std::size_t, class...>
std::false_type has_index_emplace_impl(...);

template <class Variant, std::size_t Index, class... Args>
struct has_index_emplace : decltype(has_index_emplace_impl<Variant, Index, Args...>(0)) { };

template <
    class Variant,
    class Type,
    class... Args,
    class = std::void_t<
        decltype(std::declval<Variant>().template emplace<Type>(std::declval<Args>()...))>>
std::true_type has_type_emplace_impl(int);

template <class, class, class...>
std::false_type has_type_emplace_impl(...);

template <class Variant, class Type, class... Args>
struct has_type_emplace : decltype(has_type_emplace_impl<Variant, Type, Args...>(0)) { };
}  // namespace rust

#endif  // RUST_ENUM_EMPLACE_TEST_HELPER_HPP
