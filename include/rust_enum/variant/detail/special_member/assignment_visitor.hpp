#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_ASSIGNMENT_VISITOR_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_ASSIGNMENT_VISITOR_HPP

#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/tagged_reference.hpp>
#include <rust_enum/variant/detail/variant_storage.hpp>

namespace rust::detail {
/// This class template will be used in two places:
///
/// 1. Used in variant_assign_visitor. TargetAltStorages will be variant_alternative_storage for
/// all Tys. OtherTy represents the right hand side operand of the assignment. Since
/// variant_assign_visitor handles assignment from variant, OtherTy will be
/// variant_alternative_storage from the variant. The assignment will be performed by the
/// assignment of the variant_alternative_storage itself.
///
/// 2. Used in the operator=(Ty&&) of the variant. In such cases, the rhs of the assignment will be
/// any value provided by the user, so OtherTy will not be variant_alternative_storage.
template <std::size_t OtherIdx, class OtherTy, class... TargetAltStorages>
struct variant_assign_visitor_impl {
    variant_storage_selector<TargetAltStorages...>& self;
    tagged_reference<OtherIdx, OtherTy> source;

    template <std::size_t SelfIdx, class SelfAltStorageRef>
    CONSTEXPR17 void operator()(tagged_reference<SelfIdx, SelfAltStorageRef> target) const {
        if constexpr (std::is_same<OtherTy, valueless_tag>::value) {
            self.clear_to_valueless(target);
        } else if constexpr (SelfIdx == OtherIdx) {
            target.content = source.forward_content();
        } else {
            // Note that source.forward_content() here may yield variant_alternative_storage or any
            // other value that is not a specialization of variant_alternative_storage according to
            // the value of OtherTy. Do we need to distinguish between these two cases? We can find
            // that the construction in emplace_alt will not be affected, since the target of the
            // construction is variant_alternative_storage, which can be constructed from both
            // variant_alternative_storage and any other kinds of value. However, we need to choose
            // from multiple branches in emplace_alt based on the property of the arguments. We
            // must use the property of the ACTUAL TYPE rather than variant_alternative_storage.
            //
            // For example, if the target is nothrow-move-constructible, we will construct a
            // temporary object first and move it into the variant. If we have a class with a
            // deleted move constructor:
            //
            // struct A {
            //     A(const A&) = default;
            //     A(A&&) = delete;
            // };
            //
            // then std::is_nothrow_move_constructible_v<A> or
            // variant_alternative_storage<A>::is_nothrow_move_constructible::value will be false.
            // However, std::is_nothrow_move_constructible_v<variant_alternative_storage<A>> will
            // be true! This is because the defaulted move constructor of
            // variant_alternative_storage<A> is implicitly deleted, and according to the standard,
            // it will not participate in the overload resolution. So the compiler will use the
            // copy constructor to perform moving operations instead. If we use the property of
            // variant_alternative_storage<A> rather than A itself in emplace_alt, we will run into
            // the branch and use the copy constructor to create the temporary object, which will
            // be unexpected. Maybe in the future, we can add a new strategy to allow the variant
            // to copy from the temporary.
            //
            // Fortunately, in the current implementation, the type of the target comes from
            // variant_alternative_t, which is the actual type of the target.
            self.template emplace_alt<SelfIdx, OtherIdx>(source.forward_content());
        }
    }
};

template <class... AltStorages>
struct variant_assign_visitor {
    variant_storage_selector<AltStorages...>& self;

    template <
        std::size_t SelfIdx,
        class SelfAltStorageRef,
        std::size_t OtherIdx,
        class OtherAltStorageRef>
    CONSTEXPR17 void operator()(
        tagged_reference<SelfIdx, SelfAltStorageRef> target,
        tagged_reference<OtherIdx, OtherAltStorageRef> source
    ) const {
        return variant_assign_visitor_impl<OtherIdx, OtherAltStorageRef, AltStorages...> {
            self,
            source
        }(target);
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_ASSIGNMENT_VISITOR_HPP
