#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_DESTRUCTIBLE_UNINITIALIZED_UNION_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_DESTRUCTIBLE_UNINITIALIZED_UNION_HPP

#include <type_traits>
#include <utility>

#include <rust_enum/variant/detail/fwd/destructible_uninitialized_union_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <bool IsTriviallyDestructible, class... AltStorages>
union variant_destructible_uninitialized_union {
    /* For empty Tys. */
};

// clang-format off
#define DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE(is_trivially_destructible, destructor)          \
    template <class Head, class... Tail>                                                          \
    union variant_destructible_uninitialized_union<is_trivially_destructible, Head, Tail...> {    \
        /*                                                                                        \
         * According to the standard, for a non-empty union, its constexpr constructor must       \
         * initialize exactly one variant member. To satisfy this requirement, the constexpr      \
         * constructor of variant_destructible_uninitialized_union will initialize the _dummy     \
         * member. _dummy will be the active member of the union. The reason tail is not          \
         * initialized is that tail's destructor can be non-trivial, which causes us to need to   \
         * explicitly call tail's destructor to make it inactive. (Although its destructor        \
         * doesn't do anything.)                                                                  \
         */                                                                                       \
        unsigned char _dummy;                                                                     \
        Head value;                                                                               \
        variant_destructible_uninitialized_union_t<Tail...> tail;                                 \
                                                                                                  \
        CONSTEXPR17 variant_destructible_uninitialized_union() noexcept : _dummy() { }            \
                                                                                                  \
        template <                                                                                \
            std::size_t Idx,                                                                      \
            class... Args,                                                                        \
            typename std::enable_if<(Idx > 0), int>::type = 0>                                    \
        CONSTEXPR17 explicit variant_destructible_uninitialized_union(                            \
            std::in_place_index_t<Idx>,                                                           \
            Args&&... args                                                                        \
        ) noexcept(std::is_nothrow_constructible<                                                 \
            decltype(tail),                                                                       \
            std::in_place_index_t<Idx - 1>,                                                       \
            Args&&...                                                                             \
        >::value) : tail(std::in_place_index<Idx - 1>, std::forward<Args>(args)...) { }           \
                                                                                                  \
        template <class... Args>                                                                  \
        CONSTEXPR17 explicit variant_destructible_uninitialized_union(                            \
            std::in_place_index_t<0>,                                                             \
            Args&&... args                                                                        \
        ) noexcept(Head::template is_nothrow_constructible<Args&&...>::value) :                   \
            value(std::forward<Args>(args)...) { }                                                \
                                                                                                  \
        /* Defaulted copy/move constructor/assignment will be constexpr/noexcept if possible. */  \
        variant_destructible_uninitialized_union(                                                 \
            const variant_destructible_uninitialized_union&                                       \
        ) = default;                                                                              \
                                                                                                  \
        variant_destructible_uninitialized_union(                                                 \
            variant_destructible_uninitialized_union&&                                            \
        ) = default;                                                                              \
                                                                                                  \
        auto operator=(                                                                           \
            const variant_destructible_uninitialized_union&                                       \
        ) -> variant_destructible_uninitialized_union& = default;                                 \
                                                                                                  \
        auto operator=(                                                                           \
            variant_destructible_uninitialized_union&&                                            \
        ) -> variant_destructible_uninitialized_union& = default;                                 \
                                                                                                  \
        template <std::size_t Idx>                                                                \
        CONSTEXPR20 void destroy_content() noexcept {                                             \
            if constexpr (Idx > 0) {                                                              \
                tail.template destroy_content<Idx - 1>();                                         \
                (destroy_at)(&tail);                                                              \
            } else {                                                                              \
                (destroy_at)(&value);                                                             \
            }                                                                                     \
        }                                                                                         \
                                                                                                  \
        destructor                                                                                \
    };
// clang-format on

DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE(
    true, CONSTEXPR20 ~variant_destructible_uninitialized_union() = default;
)

DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE(
    false,
    CONSTEXPR20 ~variant_destructible_uninitialized_union() {
        /* non-trivial */
    }
)

#undef DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_DESTRUCTIBLE_UNINITIALIZED_UNION_HPP
