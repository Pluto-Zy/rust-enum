#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_HPP

#include <type_traits>

#include <rust_enum/variant/detail/access.hpp>
#include <rust_enum/variant/detail/alternative_storage.hpp>
#include <rust_enum/variant/detail/special_member/constructor_helper.hpp>
#include <rust_enum/variant/detail/special_member/special_member.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>
#include <rust_enum/variant/detail/tagged_reference.hpp>
#include <rust_enum/variant/detail/tagged_visit.hpp>
#include <rust_enum/variant/variant_alternative.hpp>

namespace rust {
template <class... Tys>
class variant :
    private detail::variant_move_assignment_storage_t<
        detail::variant_alternative_storage<Tys>...> {
    using self = variant;

    static_assert(
        std::conjunction<
            typename detail::variant_alternative_storage<Tys>::is_destructible...>::value,
        "Cpp17Destructible requires that all types must be destructible."
    );
    static_assert(
        std::conjunction<
            std::disjunction<std::is_object<Tys>, std::is_lvalue_reference<Tys>>...,
            std::negation<std::is_array<Tys>>...>::value,
        "Array types and non-object types (except lvalue reference types) cannot "
        "be used in variant<>."
    );
    static_assert(sizeof...(Tys) > 0, "Cannot construct empty variant.");

    using base =
        detail::variant_move_assignment_storage_t<detail::variant_alternative_storage<Tys>...>;

    template <class Ty>
    struct is_specialization_of_in_place : std::false_type { };

    template <class Ty>
    struct is_specialization_of_in_place<std::in_place_type_t<Ty>> : std::true_type { };

    template <std::size_t Idx>
    struct is_specialization_of_in_place<std::in_place_index_t<Idx>> : std::true_type { };
public:
    using base::storage;

    template <
        class FirstType = detail::variant_alternative_storage<variant_alternative_t<0, variant>>,
        typename std::enable_if<FirstType::is_default_constructible::value, int>::type = 0>
    CONSTEXPR17 variant() noexcept(FirstType::is_nothrow_default_constructible::value) :
        base(std::in_place_index<0>) { }

    template <
        class Ty,
        typename std::enable_if<
            std::conjunction<
                std::bool_constant<(sizeof...(Tys) > 0)>,
                std::negation<std::is_same<typename detail::remove_cvref<Ty>::type, variant>>,
                std::negation<
                    is_specialization_of_in_place<typename detail::remove_cvref<Ty>::type>>,
                typename detail::conv_ctor_index<Ty, Tys...>::storage_type::
                    template is_constructible<Ty>>::value,
            int>::type = 0>
    CONSTEXPR17 variant(Ty&& other) noexcept(detail::conv_ctor_index<Ty, Tys...>::storage_type::
                                                 template is_nothrow_constructible<Ty>::value) :
        base(
            std::in_place_index<detail::conv_ctor_index<Ty, Tys...>::index>,
            std::forward<Ty>(other)
        ) { }

    template <
        class Ty,
        class... Args,
        std::size_t MatchIndex = detail::find_match_index<Ty, Tys...>(),
        typename std::enable_if<
            MatchIndex != static_cast<std::size_t>(-1)
                && detail::variant_alternative_storage<Ty>::template is_constructible<
                    Args...>::value,
            int>::type = 0>
    CONSTEXPR17 explicit variant(std::in_place_type_t<Ty>, Args&&... args) noexcept(
        detail::variant_alternative_storage<Ty>::template is_nothrow_constructible<Args...>::value
    ) :
        base(std::in_place_index<MatchIndex>, std::forward<Args>(args)...) { }

    template <
        class Ty,
        class U,
        class... Args,
        std::size_t MatchIndex = detail::find_match_index<Ty, Tys...>(),
        typename std::enable_if<
            MatchIndex != static_cast<std::size_t>(-1)
                && detail::variant_alternative_storage<
                    Ty>::template is_constructible<std::initializer_list<U>&, Args...>::value,
            int>::type = 0>
    CONSTEXPR17 explicit variant(
        std::in_place_type_t<Ty>,
        std::initializer_list<U> list,
        Args&&... args
    ) noexcept(detail::variant_alternative_storage<Ty>::
                   template is_nothrow_constructible<std::initializer_list<U>&, Args...>::value) :
        base(std::in_place_index<MatchIndex>, list, std::forward<Args>(args)...) { }

    template <
        std::size_t Idx,
        class... Args,
        typename std::enable_if<(Idx < sizeof...(Tys)), int>::type = 0,
        class TargetType =
            detail::variant_alternative_storage<variant_alternative_t<Idx, variant>>,
        typename std::enable_if<  //
            TargetType::template is_constructible<Args...>::value,
            int>::type = 0>
    CONSTEXPR17 explicit variant(std::in_place_index_t<Idx>, Args&&... args) noexcept(
        TargetType::template is_nothrow_constructible<Args&&...>::value
    ) :
        base(std::in_place_index<Idx>, std::forward<Args>(args)...) { }

    template <
        std::size_t Idx,
        class U,
        class... Args,
        typename std::enable_if<(Idx < sizeof...(Tys)), int>::type = 0,
        class TargetType =
            detail::variant_alternative_storage<variant_alternative_t<Idx, variant>>,
        typename std::enable_if<
            TargetType::template is_constructible<std::initializer_list<U>&, Args...>::value,
            int>::type = 0>
    CONSTEXPR17 explicit variant(
        std::in_place_index_t<Idx>,
        std::initializer_list<U> list,
        Args&&... args
    ) noexcept(  //
        TargetType::template is_nothrow_constructible<std::initializer_list<U>&, Args&&...>::value
    ) :
        base(std::in_place_index<Idx>, list, std::forward<Args>(args)...) { }

    template <
        class Ty,
        class Selected = detail::conv_ctor_index<Ty, Tys...>,
        typename std::enable_if<
            std::conjunction<
                std::negation<std::is_same<typename detail::remove_cvref<Ty>::type, variant>>,
                typename Selected::storage_type::template is_assignable<Ty>,
                typename Selected::storage_type::template is_constructible<Ty>>::value,
            int>::type = 0>
    CONSTEXPR17 auto operator=(Ty&& other
    ) noexcept(Selected::storage_type::template is_nothrow_assignable<Ty>::value&&
                   Selected::storage_type::template is_nothrow_constructible<Ty>::value)
        -> variant& {
        detail::tagged_visit(
            detail::variant_assign_visitor_impl<
                Selected::index,
                Ty&&,
                detail::variant_alternative_storage<Tys>...> {
                *this,
                detail::tagged_reference<Selected::index, Ty&&> { std::forward<Ty>(other) },
            },
            this->storage()
        );
        return *this;
    }

    using base::index;
    using base::valueless_by_exception;

private:
    template <std::size_t Idx, class... Args>
    CONSTEXPR20 auto emplace_impl(Args&&... args) -> variant_alternative_t<Idx, variant>& {
        detail::tagged_visit(
            [&](auto t) {
                this->template emplace_alt<decltype(t)::index, Idx>(std::forward<Args>(args)...);
            },
            this->storage()
        );
        return detail::get_variant_tagged_content<Idx>(this->storage()).content.forward_value();
    }

public:
    template <
        class Ty,
        class... Args,
        std::size_t MatchIndex = detail::find_match_index<Ty, Tys...>(),
        typename std::enable_if<
            MatchIndex != static_cast<std::size_t>(-1)
                && detail::variant_alternative_storage<Ty>::template is_constructible<
                    Args...>::value,
            int>::type = 0>
    CONSTEXPR20 auto emplace(Args&&... args) noexcept(
        detail::variant_alternative_storage<Ty>::template is_nothrow_constructible<Args...>::value
    ) -> Ty& {
        return emplace_impl<MatchIndex>(std::forward<Args>(args)...);
    }

    template <
        class Ty,
        class U,
        class... Args,
        std::size_t MatchIndex = detail::find_match_index<Ty, Tys...>(),
        typename std::enable_if<
            MatchIndex != static_cast<std::size_t>(-1)
                && detail::variant_alternative_storage<
                    Ty>::template is_constructible<std::initializer_list<U>&, Args...>::value,
            int>::type = 0>
    CONSTEXPR17 auto emplace(std::initializer_list<U> list, Args&&... args) noexcept(
        detail::variant_alternative_storage<
            Ty>::template is_nothrow_constructible<std::initializer_list<U>&, Args...>::value
    ) -> Ty& {
        return emplace_impl<MatchIndex>(list, std::forward<Args>(args)...);
    }

    template <
        std::size_t Idx,
        class... Args,
        class TargetType =
            detail::variant_alternative_storage<variant_alternative_t<Idx, variant>>,
        typename std::enable_if<  //
            TargetType::template is_constructible<Args...>::value,
            int>::type = 0>
    CONSTEXPR17 auto emplace(Args&&... args) noexcept(  //
        TargetType::template is_nothrow_constructible<Args...>::value
    ) -> variant_alternative_t<Idx, variant>& {
        return emplace_impl<Idx>(std::forward<Args>(args)...);
    }

    template <
        std::size_t Idx,
        class U,
        class... Args,
        class TargetType =
            detail::variant_alternative_storage<variant_alternative_t<Idx, variant>>,
        typename std::enable_if<
            TargetType::template is_constructible<std::initializer_list<U>&, Args...>::value,
            int>::type = 0>
    CONSTEXPR17 auto emplace(std::initializer_list<U> list, Args&&... args) noexcept(
        TargetType::template is_nothrow_constructible<std::initializer_list<U>&, Args...>::value
    ) -> variant_alternative_t<Idx, variant>& {
        return emplace_impl<Idx>(list, std::forward<Args>(args)...);
    }

private:
    struct _swap_impl {
        variant& lhs_variant;
        variant& rhs_variant;

        template <
            std::size_t LhsIdx,
            class LhsAltStorageRef,
            std::size_t RhsIdx,
            class RhsAltStorageRef>
        void operator()(
            detail::tagged_reference<LhsIdx, LhsAltStorageRef> lhs,
            detail::tagged_reference<RhsIdx, RhsAltStorageRef> rhs
        ) const {
            if constexpr (std::is_same<RhsAltStorageRef, detail::valueless_tag>::value) {
                detail::unreachable();
            } else if constexpr (LhsIdx == RhsIdx) {
                lhs.content.swap(rhs.content);
            } else {
                if constexpr (std::is_same<LhsAltStorageRef, detail::valueless_tag>::value) {
                    lhs_variant.template emplace_alt<LhsIdx, RhsIdx>(std::move(rhs.content));
                    rhs_variant.clear_to_valueless(rhs);
                } else {
                    auto _temp(std::move(rhs.content));
                    rhs_variant.template emplace_alt<RhsIdx, LhsIdx>(std::move(lhs.content));
                    lhs_variant.template emplace_alt<LhsIdx, RhsIdx>(std::move(_temp));
                }
            }
        }
    };

public:
    CONSTEXPR20 void swap(variant& rhs) noexcept(
        std::conjunction<
            typename detail::variant_alternative_storage<Tys>::is_nothrow_move_constructible...,
            typename detail::variant_alternative_storage<Tys>::is_nothrow_swappable...>::value
    ) {
        if (this->valueless_by_exception() && rhs.valueless_by_exception()) {
            return;
        } else {
            auto _lhs_ptr = this, _rhs_ptr = &rhs;
            if (rhs.valueless_by_exception())
                std::swap(_lhs_ptr, _rhs_ptr);

            detail::tagged_visit(
                _swap_impl { *_lhs_ptr, *_rhs_ptr },
                _lhs_ptr->storage(),
                _rhs_ptr->storage()
            );
        }
    }
};

template <
    class... Tys,
    typename std::enable_if<
        std::conjunction<
            typename detail::variant_alternative_storage<Tys>::is_move_constructible...,
            typename detail::variant_alternative_storage<Tys>::is_swappable...>::value,
        int>::type = 0>
CONSTEXPR20 void swap(  //
    variant<Tys...>& lhs,
    variant<Tys...>& rhs
) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_VARIANT_HPP
