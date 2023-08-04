#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TRAITS_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TRAITS_HPP

#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <class Ty>
struct traits {
    template <class... Args>
    using is_constructible = std::is_constructible<Ty, Args...>;

    template <class... Args>
    using is_trivially_constructible = std::is_trivially_constructible<Ty, Args...>;

    template <class... Args>
    using is_nothrow_constructible = std::is_nothrow_constructible<Ty, Args...>;

    using is_default_constructible = std::is_default_constructible<Ty>;
    using is_trivially_default_constructible = std::is_trivially_default_constructible<Ty>;
    using is_nothrow_default_constructible = std::is_nothrow_default_constructible<Ty>;

    using is_copy_constructible = std::is_copy_constructible<Ty>;
    using is_trivially_copy_constructible = std::is_trivially_copy_constructible<Ty>;
    using is_nothrow_copy_constructible = std::is_nothrow_copy_constructible<Ty>;

    using is_move_constructible = std::is_move_constructible<Ty>;
    using is_trivially_move_constructible = std::is_trivially_move_constructible<Ty>;
    using is_nothrow_move_constructible = std::is_nothrow_move_constructible<Ty>;

    template <class Arg>
    using is_assignable = std::is_assignable<Ty&, Arg>;

    template <class Arg>
    using is_trivially_assignable = std::is_trivially_assignable<Ty&, Arg>;

    template <class Arg>
    using is_nothrow_assignable = std::is_nothrow_assignable<Ty&, Arg>;

    using is_copy_assignable = std::is_copy_assignable<Ty>;
    using is_trivially_copy_assignable = std::is_trivially_copy_assignable<Ty>;
    using is_nothrow_copy_assignable = std::is_nothrow_copy_assignable<Ty>;

    using is_move_assignable = std::is_move_assignable<Ty>;
    using is_trivially_move_assignable = std::is_trivially_move_assignable<Ty>;
    using is_nothrow_move_assignable = std::is_nothrow_move_assignable<Ty>;

    using is_destructible = std::is_destructible<Ty>;
    using is_trivially_destructible = std::is_trivially_destructible<Ty>;
    using is_nothrow_destructible = std::is_nothrow_destructible<Ty>;

    using is_swappable = std::is_swappable<Ty>;
    using is_nothrow_swappable = std::is_nothrow_swappable<Ty>;
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TRAITS_HPP
