#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TAGGED_REFERENCE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TAGGED_REFERENCE_HPP

#include <cstddef>
#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>

namespace rust::detail {
struct valueless_tag { };

template <std::size_t Idx, class Ty>
struct tagged_reference {
    static_assert(std::is_reference<Ty>::value, "Ty must be reference.");

    using type = Ty;
    static constexpr std::size_t index = Idx;
    Ty content;

    template <
        class Self = Ty,
        typename std::enable_if<std::is_reference<Self>::value, int>::type = 0>
    CONSTEXPR17 explicit tagged_reference(Ty content) : content(std::forward<Ty>(content)) { }

    CONSTEXPR17 tagged_reference(const tagged_reference& other) :
        content(std::forward<Ty>(other.content)) { }

    CONSTEXPR17 auto forward_content() const -> Ty {
        return std::forward<Ty>(content);
    }
};

template <std::size_t Idx>
struct tagged_reference<Idx, valueless_tag> {
    using type = valueless_tag;
    static constexpr std::size_t index = Idx;

    CONSTEXPR17 auto forward_content() const -> valueless_tag {
        return {};
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TAGGED_REFERENCE_HPP
