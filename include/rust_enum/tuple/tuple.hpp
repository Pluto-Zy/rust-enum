#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_HPP

#include <memory>
#include <type_traits>

#include <rust_enum/tuple/detail/access.hpp>
#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/member_offset.hpp>
#include <rust_enum/tuple/detail/special_member/assignment_helper.hpp>
#include <rust_enum/tuple/detail/special_member/conversion_constructor_helper.hpp>
#include <rust_enum/tuple/detail/special_member/copy_constructor_helper.hpp>
#include <rust_enum/tuple/detail/special_member/default_constructor_helper.hpp>
#include <rust_enum/tuple/detail/special_member/unpack_conversion_constructor_helper.hpp>
#include <rust_enum/tuple/detail/tuple_impl.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/special_member/constructor_helper.hpp>

namespace rust {
template <class...>
class tuple {
public:
    CONSTEXPR11 tuple() = default;

    template <class Alloc>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc&) noexcept { }

    tuple(const tuple&) = default;

    template <class Alloc>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc&, const tuple&) noexcept { }

    tuple(const std::tuple<>&) noexcept { }

    template <class Alloc>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc&, const std::tuple<>&) noexcept { }

    tuple(tuple&&) = default;

    template <class Alloc>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc&, tuple&&) noexcept { }

    tuple(std::tuple<>&&) noexcept { }

    template <class Alloc>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc&, std::tuple<>&&) noexcept { }

    tuple& operator=(const tuple&) = default;
    tuple& operator=(tuple&&) = default;

    tuple& operator=(const std::tuple<>&) noexcept {
        return *this;
    }

    tuple& operator=(std::tuple<>&&) noexcept {
        return *this;
    }

    CONSTEXPR20 void swap(tuple&) noexcept { }

    template <std::size_t Idx>
    auto get() & noexcept -> typename std::tuple_element<Idx, tuple>::type& {
        // always false
        static_assert(Idx < 0, "index Idx out of bound");
    }

    template <std::size_t Idx>
    auto get() const& noexcept -> typename std::tuple_element<Idx, tuple>::type const& {
        // always false
        static_assert(Idx < 0, "index Idx out of bound");
    }

    template <std::size_t Idx>
    auto get() && noexcept -> typename std::tuple_element<Idx, tuple>::type&& {
        // always false
        static_assert(Idx < 0, "index Idx out of bound");
    }

    template <std::size_t Idx>
    auto get() const&& noexcept -> typename std::tuple_element<Idx, tuple>::type const&& {
        // always false
        static_assert(Idx < 0, "index Idx out of bound");
    }

    template <class Ty>
    auto get() & noexcept -> Ty& {
        constexpr std::size_t match_index = detail::find_match_index<Ty>();
        // always false
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
    }

    template <class Ty>
    auto get() const& noexcept -> Ty const& {
        constexpr std::size_t match_index = detail::find_match_index<Ty>();
        // always false
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
    }

    template <class Ty>
    auto get() && noexcept -> Ty&& {
        constexpr std::size_t match_index = detail::find_match_index<Ty>();
        // always false
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
    }

    template <class Ty>
    auto get() const&& noexcept -> Ty const&& {
        constexpr std::size_t match_index = detail::find_match_index<Ty>();
        // always false
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
    }
};

template <class Head, class... Tails>
class tuple<Head, Tails...> : private detail::tuple_impl<Head, Tails...> {
    using base = detail::tuple_impl<Head, Tails...>;
    using self = tuple;

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Default Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    template <
        class HeadArg = Head,
        typename std::enable_if<
            detail::implicitly_default_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR11 tuple() noexcept(detail::nothrow_default_constructible<Head, Tails...>::value) :
        base() { }

    template <
        class Alloc,
        class HeadArg = Head,
        typename std::enable_if<
            detail::implicitly_default_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc) :
        base(std::allocator_arg, alloc) { }

    template <
        class HeadArg = Head,
        typename std::enable_if<
            detail::explicitly_default_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR11 explicit tuple() noexcept(  //
        detail::nothrow_default_constructible<Head, Tails...>::value
    ) :
        base() { }

    template <
        class Alloc,
        class HeadArg = Head,
        typename std::enable_if<
            detail::explicitly_default_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR20 explicit tuple(std::allocator_arg_t, const Alloc& alloc) :
        base(std::allocator_arg, alloc) { }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Direct Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    template <
        class HeadArg = Head,
        typename std::enable_if<
            detail::implicitly_copy_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR14 tuple(const Head& head, const Tails&... tails) noexcept(
        detail::nothrow_copy_constructible<Head, Tails...>::value
    ) :
        base(std::forward<Head>(head), std::forward<Tails>(tails)...) { }

    template <
        class Alloc,
        class HeadArg = Head,
        typename std::enable_if<
            detail::implicitly_copy_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR20 tuple(
        std::allocator_arg_t,
        const Alloc& alloc,
        const Head& head,
        const Tails&... tails
    ) :
        base(std::allocator_arg, alloc, std::forward<Head>(head), std::forward<Tails>(tails)...) {
    }

    template <
        class HeadArg = Head,
        typename std::enable_if<
            detail::explicitly_copy_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR14 explicit tuple(const Head& head, const Tails&... tails) noexcept(
        detail::nothrow_copy_constructible<Head, Tails...>::value
    ) :
        base(std::forward<Head>(head), std::forward<Tails>(tails)...) { }

    template <
        class Alloc,
        class HeadArg = Head,
        typename std::enable_if<
            detail::explicitly_copy_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR20 explicit tuple(
        std::allocator_arg_t,
        const Alloc& alloc,
        const Head& head,
        const Tails&... tails
    ) :
        base(std::allocator_arg, alloc, std::forward<Head>(head), std::forward<Tails>(tails)...) {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Converting Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    template <
        class First,
        class... Last,
        typename std::enable_if<
            std::conjunction<
                detail::implicitly_conv_constructible<self, First, Last...>,
                detail::conv_ctor_constraint<self, First, Last...>>::value,
            int>::type = 0>
    CONSTEXPR14 tuple(First&& first, Last&&... last) noexcept(
        detail::nothrow_conv_constructible<self, First, Last...>::value
    ) :
        base(std::forward<First>(first), std::forward<Last>(last)...) { }

    template <
        class Alloc,
        class First,
        class... Last,
        typename std::enable_if<
            std::conjunction<
                detail::implicitly_conv_constructible<self, First, Last...>,
                detail::conv_ctor_constraint<self, First, Last...>>::value,
            int>::type = 0>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc, First&& first, Last&&... last) :
        base(std::allocator_arg, alloc, std::forward<First>(first), std::forward<Last>(last)...) {
    }

    template <
        class First,
        class... Last,
        typename std::enable_if<
            std::conjunction<
                detail::explicitly_conv_constructible<self, First, Last...>,
                detail::conv_ctor_constraint<self, First, Last...>>::value,
            int>::type = 0>
    CONSTEXPR14 explicit tuple(First&& first, Last&&... last) noexcept(
        detail::nothrow_conv_constructible<self, First, Last...>::value
    ) :
        base(std::forward<First>(first), std::forward<Last>(last)...) { }

    template <
        class Alloc,
        class First,
        class... Last,
        typename std::enable_if<
            std::conjunction<
                detail::explicitly_conv_constructible<self, First, Last...>,
                detail::conv_ctor_constraint<self, First, Last...>>::value,
            int>::type = 0>
    CONSTEXPR20 explicit tuple(
        std::allocator_arg_t,
        const Alloc& alloc,
        First&& first,
        Last&&... last
    ) :
        base(std::allocator_arg, alloc, std::forward<First>(first), std::forward<Last>(last)...) {
    }

private:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Implementation of Unpacking Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    struct unpack_tag { };
    struct unpack_alloc_tag { };

    template <std::size_t... Is, class Tuple>
    CONSTEXPR14 tuple(unpack_tag, std::index_sequence<Is...>, Tuple&& other) :
        base(detail::universal_get<Is>(std::forward<Tuple>(other))...) { }

    template <std::size_t... Is, class Alloc, class Tuple>
    CONSTEXPR20 tuple(
        unpack_alloc_tag,
        std::index_sequence<Is...>,
        const Alloc& alloc,
        Tuple&& other
    ) :
        base(std::allocator_arg, alloc, detail::universal_get<Is>(std::forward<Tuple>(other))...) {
    }

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Unpacking Tuple Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // clang-format off
#define CONV_CTOR(arg_type)                                                                       \
    template <                                                                                    \
        class... Us, class IndexSeq = std::index_sequence_for<Us...>,                             \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                detail::implicitly_unpack_conv_constructible<IndexSeq, self, arg_type>,           \
                detail::unpack_conv_ctor_constraint<self, arg_type, Us...>>::value,               \
            int>::type = 0>                                                                       \
    CONSTEXPR14 tuple(arg_type other) noexcept(                                                   \
        detail::nothrow_unpack_conv_constructible<IndexSeq, self, arg_type>::value                \
    ) :                                                                                           \
        tuple(unpack_tag(), IndexSeq(), std::forward<arg_type>(other)) { }                        \
                                                                                                  \
    template <                                                                                    \
        class Alloc, class... Us, class IndexSeq = std::index_sequence_for<Us...>,                \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                detail::implicitly_unpack_conv_constructible<IndexSeq, self, arg_type>,           \
                detail::unpack_conv_ctor_constraint<self, arg_type, Us...>>::value,               \
            int>::type = 0>                                                                       \
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc, arg_type other) :                 \
        tuple(unpack_alloc_tag(), IndexSeq(), alloc, std::forward<arg_type>(other)) { }           \
                                                                                                  \
    template <                                                                                    \
        class... Us, class IndexSeq = std::index_sequence_for<Us...>,                             \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                detail::explicitly_unpack_conv_constructible<IndexSeq, self, arg_type>,           \
                detail::unpack_conv_ctor_constraint<self, arg_type, Us...>>::value,               \
            int>::type = 0>                                                                       \
    CONSTEXPR14 explicit tuple(arg_type other) noexcept(                                          \
        detail::nothrow_unpack_conv_constructible<IndexSeq, self, arg_type>::value                \
    ) :                                                                                           \
        tuple(unpack_tag(), IndexSeq(), std::forward<arg_type>(other)) { }                        \
                                                                                                  \
    template <                                                                                    \
        class Alloc, class... Us, class IndexSeq = std::index_sequence_for<Us...>,                \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                detail::explicitly_unpack_conv_constructible<IndexSeq, self, arg_type>,           \
                detail::unpack_conv_ctor_constraint<self, arg_type, Us...>>::value,               \
            int>::type = 0>                                                                       \
    CONSTEXPR20 explicit tuple(std::allocator_arg_t, const Alloc& alloc, arg_type other) :        \
        tuple(unpack_alloc_tag(), IndexSeq(), alloc, std::forward<arg_type>(other)) { }
    // clang-format on

    CONV_CTOR(tuple<Us...>&)
    CONV_CTOR(const tuple<Us...>&)
    CONV_CTOR(tuple<Us...>&&)
    CONV_CTOR(const tuple<Us...>&&)

    CONV_CTOR(std::tuple<Us...>&)
    CONV_CTOR(const std::tuple<Us...>&)
    CONV_CTOR(std::tuple<Us...>&&)
    CONV_CTOR(const std::tuple<Us...>&&)

#undef CONV_CTOR

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Unpacking Pair Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // clang-format off
#define PAIR_CTOR(...)                                                                            \
    template <                                                                                    \
        class U1, class U2, class IndexSeq = std::index_sequence_for<U1, U2>,                     \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                std::bool_constant<sizeof...(Tails) == 1>,                                        \
                detail::implicitly_unpack_conv_constructible<IndexSeq, self, __VA_ARGS__>>::value,\
            int>::type = 0>                                                                       \
    CONSTEXPR14 tuple(__VA_ARGS__ p) noexcept(                                                    \
        detail::nothrow_unpack_conv_constructible<IndexSeq, self, __VA_ARGS__>::value             \
    ) :                                                                                           \
        tuple(IndexSeq(), std::forward<__VA_ARGS__>(p)) { }                                       \
                                                                                                  \
    template <                                                                                    \
        class Alloc, class U1, class U2, class IndexSeq = std::index_sequence_for<U1, U2>,        \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                std::bool_constant<sizeof...(Tails) == 1>,                                        \
                detail::implicitly_unpack_conv_constructible<IndexSeq, self, __VA_ARGS__>>::value,\
            int>::type = 0>                                                                       \
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc, __VA_ARGS__ p) :                  \
        tuple(IndexSeq(), std::allocator_arg, alloc, std::forward<__VA_ARGS__>(p)) { }            \
                                                                                                  \
    template <                                                                                    \
        class U1, class U2, class IndexSeq = std::index_sequence_for<U1, U2>,                     \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                std::bool_constant<sizeof...(Tails) == 1>,                                        \
                detail::explicitly_unpack_conv_constructible<IndexSeq, self, __VA_ARGS__>>::      \
                value,                                                                            \
            int>::type = 0>                                                                       \
    CONSTEXPR14 tuple(__VA_ARGS__ p) noexcept(                                                    \
        detail::nothrow_unpack_conv_constructible<IndexSeq, self, __VA_ARGS__>::value             \
    ) :                                                                                           \
        tuple(IndexSeq(), std::forward<__VA_ARGS__>(p)) { }                                       \
                                                                                                  \
    template <                                                                                    \
        class Alloc, class U1, class U2, class IndexSeq = std::index_sequence_for<U1, U2>,        \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                std::bool_constant<sizeof...(Tails) == 1>,                                        \
                detail::explicitly_unpack_conv_constructible<IndexSeq, self, __VA_ARGS__>>::value,\
            int>::type = 0>                                                                       \
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc, __VA_ARGS__ p) :                  \
        tuple(IndexSeq(), std::allocator_arg, alloc, std::forward<__VA_ARGS__>(p)) { }
    // clang-format on

    PAIR_CTOR(std::pair<U1, U2>&)
    PAIR_CTOR(const std::pair<U1, U2>&)
    PAIR_CTOR(std::pair<U1, U2>&&)
    PAIR_CTOR(const std::pair<U1, U2>&&)

#undef PAIR_CTOR

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Copy Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    tuple(const tuple&) = default;

    template <
        class Alloc,
        class HeadArg = Head,
        typename std::enable_if<  //
            detail::copy_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc, const tuple& other) :
        tuple(std::index_sequence_for<Head, Tails...>(), std::allocator_arg, alloc, other) { }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Move Constructor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    tuple(tuple&&) = default;

    template <
        class Alloc,
        class HeadArg = Head,
        typename std::enable_if<  //
            detail::move_constructible<HeadArg, Tails...>::value,
            int>::type = 0>
    CONSTEXPR20 tuple(std::allocator_arg_t, const Alloc& alloc, tuple&& other) :
        tuple(
            std::index_sequence_for<Head, Tails...>(),
            std::allocator_arg,
            alloc,
            std::move(other)
        ) { }

private:
    struct placeholder { };

public:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Copy Assignment
    ///////////////////////////////////////////////////////////////////////////////////////////////

    CONSTEXPR20 auto operator=(  //
        typename std::conditional<
            detail::copy_assignable<Head, Tails...>::value,
            tuple,
            placeholder>::type const& other
    ) noexcept(detail::nothrow_copy_assignable<Head, Tails...>::value) -> tuple& {
        base::unpack_assign_from(std::index_sequence_for<Head, Tails...>(), other);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Move Assignment
    ///////////////////////////////////////////////////////////////////////////////////////////////

    CONSTEXPR20 auto operator=(  //
        typename std::conditional<  //
            detail::move_assignable<Head, Tails...>::value,
            tuple,
            placeholder>::type&& other
    ) noexcept(detail::nothrow_move_assignable<Head, Tails...>::value) -> tuple& {
        base::unpack_assign_from(std::index_sequence_for<Head, Tails...>(), std::move(other));
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Unpacking Tuple Assignment
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // clang-format off
#define UNPACK_ASSIGNMENT(arg_type)                                                               \
    template <                                                                                    \
        class... Us, class IndexSeq = std::index_sequence_for<Us...>,                             \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                std::bool_constant<sizeof...(Us) == sizeof...(Tails) + 1>,                        \
                detail::unpack_conv_assignable<IndexSeq, tuple, arg_type>>::value,                \
            int>::type = 0>                                                                       \
    CONSTEXPR20 auto operator=(arg_type other) noexcept(                                          \
        detail::nothrow_unpack_conv_assignable<IndexSeq, tuple, arg_type>::value                  \
    ) -> tuple& {                                                                                 \
        base::unpack_assign_from(IndexSeq(), std::forward<arg_type>(other));                      \
        return *this;                                                                             \
    }
    // clang-format on

    UNPACK_ASSIGNMENT(const tuple<Us...>&)
    UNPACK_ASSIGNMENT(tuple<Us...>&&)
    UNPACK_ASSIGNMENT(const std::tuple<Us...>&)
    UNPACK_ASSIGNMENT(std::tuple<Us...>&&)

#undef UNPACK_ASSIGNMENT

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Unpacking Pair Assignment
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // clang-format off
#define UNPACK_PAIR_ASSIGNMENT(...)                                                               \
    template <                                                                                    \
        class U1,                                                                                 \
        class U2,                                                                                 \
        class IndexSeq = std::index_sequence_for<U1, U2>,                                         \
        typename std::enable_if<                                                                  \
            std::conjunction<                                                                     \
                std::bool_constant<sizeof...(Tails) == 1>,                                        \
                detail::unpack_conv_assignable<IndexSeq, tuple, __VA_ARGS__>>::value,             \
            int>::type = 0>                                                                       \
    CONSTEXPR20 auto operator=(__VA_ARGS__ other) noexcept(                                       \
        detail::nothrow_unpack_conv_assignable<IndexSeq, tuple, __VA_ARGS__>::value               \
    ) -> tuple& {                                                                                 \
        base::unpack_assign_from(IndexSeq(), std::forward<__VA_ARGS__>(other));                   \
        return *this;                                                                             \
    }
    // clang-format on

    UNPACK_PAIR_ASSIGNMENT(const std::pair<U1, U2>&)
    UNPACK_PAIR_ASSIGNMENT(std::pair<U1, U2>&&)

#undef UNPACK_PAIR_ASSIGNMENT

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Swap
    ///////////////////////////////////////////////////////////////////////////////////////////////

    CONSTEXPR20 void swap(tuple& other) noexcept(
        std::conjunction<
            typename detail::tuple_storage<Head>::is_nothrow_swappable,
            typename detail::tuple_storage<Tails>::is_nothrow_swappable...>::value
    ) {
        base::swap(other);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Get
    ///////////////////////////////////////////////////////////////////////////////////////////////

    template <std::size_t Idx>
    CONSTEXPR14 auto get() & noexcept -> typename std::tuple_element<Idx, tuple>::type& {
        static_assert(Idx <= sizeof...(Tails), "index Idx out of bound");
        return detail::get_tuple_content<Idx>(static_cast<base&>(*this));
    }

    template <std::size_t Idx>
    CONSTEXPR14 auto get() const& noexcept ->
        typename std::tuple_element<Idx, tuple>::type const& {
        static_assert(Idx <= sizeof...(Tails), "index Idx out of bound");
        return detail::get_tuple_content<Idx>(static_cast<base const&>(*this));
    }

    template <std::size_t Idx>
    CONSTEXPR14 auto get() && noexcept -> typename std::tuple_element<Idx, tuple>::type&& {
        static_assert(Idx <= sizeof...(Tails), "index Idx out of bound");
        return detail::get_tuple_content<Idx>(static_cast<base&&>(*this));
    }

    template <std::size_t Idx>
    CONSTEXPR14 auto get() const&& noexcept ->
        typename std::tuple_element<Idx, tuple>::type const&& {
        static_assert(Idx <= sizeof...(Tails), "index Idx out of bound");
        return detail::get_tuple_content<Idx>(static_cast<base const&&>(*this));
    }

    template <class Ty>
    CONSTEXPR14 auto get() & noexcept -> Ty& {
        constexpr std::size_t match_index = detail::find_match_index<Ty, Head, Tails...>();
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
        return detail::get_tuple_content<match_index>(static_cast<base&>(*this));
    }

    template <class Ty>
    CONSTEXPR14 auto get() const& noexcept -> Ty const& {
        constexpr std::size_t match_index = detail::find_match_index<Ty, Head, Tails...>();
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
        return detail::get_tuple_content<match_index>(static_cast<base const&>(*this));
    }

    template <class Ty>
    CONSTEXPR14 auto get() && noexcept -> Ty&& {
        constexpr std::size_t match_index = detail::find_match_index<Ty, Head, Tails...>();
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
        return detail::get_tuple_content<match_index>(static_cast<base&&>(*this));
    }

    template <class Ty>
    CONSTEXPR14 auto get() const&& noexcept -> Ty const&& {
        constexpr std::size_t match_index = detail::find_match_index<Ty, Head, Tails...>();
        static_assert(
            match_index != static_cast<std::size_t>(-1),
            "The type Ty must occur exactly once in Tys."
        );
        return detail::get_tuple_content<match_index>(static_cast<base const&&>(*this));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // OffsetOf
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <std::size_t Idx>
    CONSTEXPR11 auto member_offset() const noexcept -> std::size_t {
        static_assert(Idx <= sizeof...(Tails), "index Idx out of bound");
        return detail::get_tuple_member_offset<Idx>(static_cast<base const&&>(*this));
    }
};

template <
    class... Tys,
    typename std::enable_if<
        std::conjunction<typename detail::tuple_storage<Tys>::is_swappable...>::value,
        int>::type = 0>
CONSTEXPR20 void swap(tuple<Tys...>& lhs, tuple<Tys...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_TUPLE_HPP
