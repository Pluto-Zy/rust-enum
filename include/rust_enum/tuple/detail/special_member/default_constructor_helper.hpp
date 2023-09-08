#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_DEFAULT_CONSTRUCTOR_HELPER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_DEFAULT_CONSTRUCTOR_HELPER_HPP

#include <type_traits>

#include <rust_enum/tuple/detail/tuple_storage.hpp>

namespace rust::detail {
template <class Ty>
struct is_implicitly_default_constructible_impl {
    static void imaginary_function(Ty);
};

template <class Ty, class = void>
struct is_implicitly_default_constructible : std::false_type { };

// Note that being able to legally call imaginary_function via {} does not mean that type Ty is
// default-constructible. For example, before C++20, the class below is not default-constructible:
//
// struct A {
//     A() = delete;
// };
//
// However, we can construct an object of A through `A a{}`. This is because A is an aggregate.
// When the initializer is an empty list, the compiler will use aggregate initialization to
// initialize it, which will skip the deleted constructor.
template <class Ty>
struct is_implicitly_default_constructible<
    Ty,
    decltype(is_implicitly_default_constructible_impl<Ty>::imaginary_function({}))> :
    tuple_storage<Ty>::is_default_constructible { };

template <class... Tys>
using implicitly_default_constructible =
    std::conjunction<is_implicitly_default_constructible<Tys>...>;

template <class... Tys>
using explicitly_default_constructible = std::conjunction<
    typename tuple_storage<Tys>::is_default_constructible...,
    std::negation<implicitly_default_constructible<Tys>>...>;

template <class... Ty>
using nothrow_default_constructible =
    std::conjunction<typename tuple_storage<Ty>::is_nothrow_default_constructible...>;
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_SPECIAL_MEMBER_DEFAULT_CONSTRUCTOR_HELPER_HPP
