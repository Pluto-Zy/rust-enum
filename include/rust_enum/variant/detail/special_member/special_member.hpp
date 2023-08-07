#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_SPECIAL_MEMBER_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_SPECIAL_MEMBER_HPP

#include <type_traits>

#include <rust_enum/variant/detail/fwd/tagged_visit_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/special_member/assignment_visitor.hpp>
#include <rust_enum/variant/detail/special_member/constructor_visitor.hpp>
#include <rust_enum/variant/detail/variant_storage.hpp>

namespace rust::detail {
template <class... AltStorages>
struct variant_non_trivially_destructible_storage : variant_storage_selector<AltStorages...> {
    using self = variant_non_trivially_destructible_storage;
    using base = variant_storage_selector<AltStorages...>;
    using base::base;

    variant_non_trivially_destructible_storage() = default;
    variant_non_trivially_destructible_storage(const self&) = default;
    variant_non_trivially_destructible_storage(self&&) = default;
    auto operator=(const self&) -> self& = default;
    auto operator=(self&&) -> self& = default;

    CONSTEXPR20 ~variant_non_trivially_destructible_storage() {
        this->clear();
    }
};

template <class... AltStorages>
using variant_destructible_storage_t = typename std::conditional<
    std::conjunction<typename AltStorages::is_trivially_destructible...>::value,
    variant_storage_selector<AltStorages...>,
    variant_non_trivially_destructible_storage<AltStorages...>>::type;

template <class... AltStorages>
struct variant_non_trivially_copy_constructible_storage :
    variant_destructible_storage_t<AltStorages...> {
    using self = variant_non_trivially_copy_constructible_storage;
    using base = variant_destructible_storage_t<AltStorages...>;
    using base::base;

    variant_non_trivially_copy_constructible_storage() = default;
    variant_non_trivially_copy_constructible_storage(self&&) = default;
    auto operator=(const self&) -> self& = default;
    auto operator=(self&&) -> self& = default;

    CONSTEXPR17
    variant_non_trivially_copy_constructible_storage(const self& other) noexcept(  //
        std::conjunction<typename AltStorages::is_nothrow_copy_constructible...>::value
    ) :
        base() {
        tagged_visit(variant_construct_visitor<AltStorages...> { *this }, other.storage());
    }
};

template <class... AltStorages>
struct variant_deleted_copy_storage : variant_destructible_storage_t<AltStorages...> {
    using self = variant_deleted_copy_storage;
    using base = variant_destructible_storage_t<AltStorages...>;
    using base::base;

    variant_deleted_copy_storage() = default;
    variant_deleted_copy_storage(const self&) = delete;
    variant_deleted_copy_storage(self&&) = default;
    auto operator=(const self&) -> self& = default;
    auto operator=(self&&) -> self& = default;
};

template <class... AltStorages>
using variant_copy_constructible_storage_t = typename std::conditional<
    std::conjunction<typename AltStorages::is_trivially_copy_constructible...>::value,
    variant_destructible_storage_t<AltStorages...>,
    typename std::conditional<
        std::conjunction<typename AltStorages::is_copy_constructible...>::value,
        variant_non_trivially_copy_constructible_storage<AltStorages...>,
        variant_deleted_copy_storage<AltStorages...>>::type>::type;

template <class... AltStorages>
struct variant_non_trivially_move_constructible_storage :
    variant_copy_constructible_storage_t<AltStorages...> {
    using self = variant_non_trivially_move_constructible_storage;
    using base = variant_copy_constructible_storage_t<AltStorages...>;
    using base::base;

    variant_non_trivially_move_constructible_storage() = default;
    variant_non_trivially_move_constructible_storage(const self&) = default;
    auto operator=(const self&) -> self& = default;
    auto operator=(self&&) -> self& = default;

    CONSTEXPR17
    variant_non_trivially_move_constructible_storage(self&& other) noexcept(  //
        std::conjunction<typename AltStorages::is_nothrow_move_constructible...>::value
    ) {
        tagged_visit(
            variant_construct_visitor<AltStorages...> { *this },
            std::move(other).storage()
        );
    }
};

template <class... AltStorages>
struct variant_deleted_move_storage : variant_copy_constructible_storage_t<AltStorages...> {
    using self = variant_deleted_move_storage;
    using base = variant_copy_constructible_storage_t<AltStorages...>;
    using base::base;

    variant_deleted_move_storage() = default;
    variant_deleted_move_storage(const self&) = default;
    variant_deleted_move_storage(self&&) = delete;
    auto operator=(const self&) -> self& = default;
    auto operator=(self&&) -> self& = default;
};

template <class... AltStorages>
using variant_move_constructible_storage_t = typename std::conditional<
    std::conjunction<typename AltStorages::is_trivially_move_constructible...>::value,
    variant_copy_constructible_storage_t<AltStorages...>,
    typename std::conditional<
        std::conjunction<typename AltStorages::is_move_constructible...>::value,
        variant_non_trivially_move_constructible_storage<AltStorages...>,
        variant_deleted_move_storage<AltStorages...>>::type>::type;

template <class... AltStorages>
struct variant_non_trivially_copy_assignable_storage :
    variant_move_constructible_storage_t<AltStorages...> {
    using self = variant_non_trivially_copy_assignable_storage;
    using base = variant_move_constructible_storage_t<AltStorages...>;
    using base::base;

    variant_non_trivially_copy_assignable_storage() = default;
    variant_non_trivially_copy_assignable_storage(const self&) = default;
    variant_non_trivially_copy_assignable_storage(self&&) = default;
    auto operator=(self&&) -> self& = default;

    self& operator=(const self& other) noexcept(
        std::conjunction<
            typename AltStorages::is_nothrow_copy_constructible...,
            typename AltStorages::is_nothrow_copy_assignable...>::value
    ) {
        tagged_visit(
            variant_assign_visitor<AltStorages...> { *this },
            this->storage(),
            other.storage()
        );
        return *this;
    }
};

template <class... AltStorages>
struct variant_deleted_copy_assignment_storage :
    variant_move_constructible_storage_t<AltStorages...> {
    using self = variant_deleted_copy_assignment_storage;
    using base = variant_move_constructible_storage_t<AltStorages...>;
    using base::base;

    variant_deleted_copy_assignment_storage() = default;
    variant_deleted_copy_assignment_storage(const self&) = default;
    variant_deleted_copy_assignment_storage(self&&) = default;
    self& operator=(const self&) = delete;
    auto operator=(self&&) -> self& = default;
};

template <class... AltStorages>
using variant_copy_assignment_storage_t = typename std::conditional<
    std::conjunction<
        typename AltStorages::is_trivially_copy_constructible...,
        typename AltStorages::is_trivially_copy_assignable...,
        typename AltStorages::is_trivially_destructible...>::value,
    variant_move_constructible_storage_t<AltStorages...>,
    typename std::conditional<
        std::conjunction<
            typename AltStorages::is_copy_constructible...,
            typename AltStorages::is_copy_assignable...>::value,
        variant_non_trivially_copy_assignable_storage<AltStorages...>,
        variant_deleted_copy_assignment_storage<AltStorages...>>::type>::type;

template <class... AltStorages>
struct variant_non_trivially_move_assignable_storage :
    variant_copy_assignment_storage_t<AltStorages...> {
    using self = variant_non_trivially_move_assignable_storage;
    using base = variant_copy_assignment_storage_t<AltStorages...>;
    using base::base;

    variant_non_trivially_move_assignable_storage() = default;
    variant_non_trivially_move_assignable_storage(const self&) = default;
    variant_non_trivially_move_assignable_storage(self&&) = default;
    auto operator=(const self&) -> self& = default;

    self& operator=(self&& other) noexcept(
        std::conjunction<
            typename AltStorages::is_nothrow_move_constructible...,
            typename AltStorages::is_nothrow_move_assignable...>::value
    ) {
        tagged_visit(
            variant_assign_visitor<AltStorages...> { *this },
            this->storage(),
            std::move(other).storage()
        );
        return *this;
    }
};

template <class... AltStorages>
struct variant_deleted_move_assignment_storage :
    variant_copy_assignment_storage_t<AltStorages...> {
    using self = variant_deleted_move_assignment_storage;
    using base = variant_copy_assignment_storage_t<AltStorages...>;
    using base::base;

    variant_deleted_move_assignment_storage() = default;
    variant_deleted_move_assignment_storage(const self&) = default;
    variant_deleted_move_assignment_storage(self&&) = default;
    auto operator=(const self&) -> self& = default;
    self& operator=(self&&) = delete;
};

template <class... AltStorages>
using variant_move_assignment_storage_t = typename std::conditional<
    std::conjunction<
        typename AltStorages::is_trivially_move_constructible...,
        typename AltStorages::is_trivially_move_assignable...,
        typename AltStorages::is_trivially_destructible...>::value,
    variant_copy_assignment_storage_t<AltStorages...>,
    typename std::conditional<
        std::conjunction<
            typename AltStorages::is_move_constructible...,
            typename AltStorages::is_move_assignable...>::value,
        variant_non_trivially_move_assignable_storage<AltStorages...>,
        variant_deleted_move_assignment_storage<AltStorages...>>::type>::type;
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_SPECIAL_MEMBER_HPP
