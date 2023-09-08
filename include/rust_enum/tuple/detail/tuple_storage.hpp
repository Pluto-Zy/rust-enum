#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_STORAGE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_STORAGE_HPP

#include <memory>
#include <type_traits>
#include <utility>

#include <rust_enum/variant/detail/std_impl.hpp>
#include <rust_enum/variant/detail/traits.hpp>

namespace rust::detail {
template <class Ty>
class tuple_storage : public traits<Ty> {
    using base = traits<Ty>;
    Ty value;

public:
    using value_type = Ty;

    CONSTEXPR11 tuple_storage() noexcept(base::is_nothrow_default_constructible::value) :
        value() { }

    template <
        class Arg,
        typename std::enable_if<
            !std::is_same<typename remove_cvref<Arg>::type, tuple_storage>::value,
            int>::type = 0>
    CONSTEXPR14 explicit tuple_storage(Arg&& arg) noexcept(  //
        base::template is_nothrow_constructible<Arg&&>::value
    ) :
        value(std::forward<Arg>(arg)) { }

private:
    template <class Alloc>
    using uses_allocator = std::uses_allocator<value_type, Alloc>;

public:
    template <
        class Alloc,
        class... Args,
        typename std::enable_if<!uses_allocator<Alloc>::value, int>::type = 0>
    CONSTEXPR20 tuple_storage(std::allocator_arg_t, const Alloc&, Args&&... args) :
        value(std::forward<Args>(args)...) { }

    template <
        class Alloc,
        class... Args,
        typename std::enable_if<
            std::conjunction<
                uses_allocator<Alloc>,
                typename base::template is_constructible<  //
                    std::allocator_arg_t,
                    Alloc,
                    Args...>>::value,
            int>::type = 0>
    CONSTEXPR20 tuple_storage(std::allocator_arg_t, const Alloc& alloc, Args&&... args) :
        value(std::allocator_arg, alloc, std::forward<Args>(args)...) { }

    template <
        class Alloc,
        class... Args,
        typename std::enable_if<
            std::conjunction<
                uses_allocator<Alloc>,
                std::negation<typename base::template is_constructible<  //
                    std::allocator_arg_t,
                    Alloc,
                    Args...>>>::value,
            int>::type = 0>
    CONSTEXPR20 tuple_storage(std::allocator_arg_t, const Alloc& alloc, Args&&... args) :
        value(std::forward<Args>(args)..., alloc) { }

    template <
        class Arg,
        typename std::enable_if<
            !std::is_same<typename remove_cvref<Arg>::type, tuple_storage>::value,
            int>::type = 0>
    CONSTEXPR20 auto operator=(Arg&& arg) noexcept(  //
        base::template is_nothrow_assignable<Ty&, Arg>::value
    ) -> tuple_storage& {
        value = std::forward<Arg>(arg);
        return *this;
    }

    CONSTEXPR17 void swap(tuple_storage& other) {
        using std::swap;
        swap(value, other.value);
    }

    CONSTEXPR17 auto forward_value() & noexcept -> Ty& {
        return value;
    }

    CONSTEXPR17 auto forward_value() const& noexcept -> Ty const& {
        return value;
    }

    CONSTEXPR17 auto forward_value() && noexcept -> Ty&& {
        return std::move(value);
    }

    CONSTEXPR17 auto forward_value() const&& noexcept -> Ty const&& {
        return std::move(value);
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_STORAGE_HPP
