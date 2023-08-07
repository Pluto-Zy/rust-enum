#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_CONSTRUCTOR_VISITOR_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_CONSTRUCTOR_VISITOR_HPP

#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/tagged_reference.hpp>
#include <rust_enum/variant/detail/variant_storage.hpp>

namespace rust::detail {
template <class... AltStorages>
struct variant_construct_visitor {
    variant_storage<AltStorages...>& self;

    template <std::size_t Idx, class AltStorageRef>
    CONSTEXPR17 void operator()(tagged_reference<Idx, AltStorageRef> source) const {
        if constexpr (!std::is_same<AltStorageRef, valueless_tag>::value) {
            self.template construct_union_alt<Idx>(source.forward_content());
        } else {
            self.set_index(Idx);
        }
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_SPECIAL_MEMBER_CONSTRUCTOR_VISITOR_HPP
