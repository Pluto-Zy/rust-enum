#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_ALTERNATIVE_STORAGE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_ALTERNATIVE_STORAGE_HPP

#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/traits.hpp>

namespace rust::detail {
template <class Ty, bool = std::is_class<Ty>::value && !std::is_final<Ty>::value>
struct is_empty_type_impl {
    int dummy;
};

template <class Ty>
struct is_empty_type_impl<Ty, true> : Ty { };

template <class Ty>
struct is_empty_type :
    std::disjunction<std::is_void<Ty>, std::bool_constant<sizeof(is_empty_type_impl<Ty>) == 1>> {
};

template <class Ty>
class variant_alternative_storage : public traits<Ty> {
    using self = variant_alternative_storage;
    typename std::remove_const<Ty>::type value;

public:
    using value_type = Ty;

    static constexpr std::size_t storage_size = [] {
        if constexpr (is_empty_type<Ty>::value) {
            return 0;
        } else {
            return sizeof(Ty);
        }
    }();

    template <
        class Self = self,
        typename std::enable_if<Self::is_default_constructible::value, int>::type = 0>
    CONSTEXPR17 variant_alternative_storage() noexcept(  //
        self::is_nothrow_default_constructible::value
    ) :
        value() { }

    template <
        class First,
        class... Last,
        typename std::enable_if<
            std::conjunction<
                std::negation<
                    std::is_same<typename remove_cvref<First>::type, variant_alternative_storage>>,
                typename self::template is_constructible<First, Last...>>::value,
            int>::type = 0>
    CONSTEXPR17 explicit variant_alternative_storage(First&& first, Last&&... last) noexcept(
        self::template is_nothrow_constructible<First, Last...>::value
    ) :
        value(std::forward<First>(first), std::forward<Last>(last)...) { }

    template <
        class Arg,
        typename std::enable_if<
            std::conjunction<
                std::negation<
                    std::is_same<typename remove_cvref<Arg>::type, variant_alternative_storage>>,
                typename self::template is_assignable<Arg>>::value,
            int>::type = 0>
    CONSTEXPR17 auto operator=(Arg&& arg) noexcept(  //
        std::is_nothrow_assignable<Ty&, Arg>::value
    ) -> variant_alternative_storage& {
        value = std::forward<Arg>(arg);
        return *this;
    }

    CONSTEXPR17 void swap(variant_alternative_storage& other) {
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

    CONSTEXPR17 auto raw_value() const noexcept -> Ty const& {
        return value;
    }

    CONSTEXPR17 auto raw_value() noexcept -> Ty& {
        return value;
    }
};

template <class Ty>
struct is_reference_constructible_impl {
    static void imaginary_function(Ty&) noexcept;
    static void imaginary_function(Ty&&) = delete;
};

template <class Ty, class Arg, class = void>
struct is_reference_constructible : std::false_type { };

template <class Ty, class Arg>
struct is_reference_constructible<
    Ty,
    Arg,
    decltype(is_reference_constructible_impl<Ty>::imaginary_function(std::declval<Arg>()))> :
    std::true_type { };

template <class Ty, class Arg, class = void>
struct is_nothrow_reference_constructible : std::false_type { };

template <class Ty, class Arg>
struct is_nothrow_reference_constructible<
    Ty,
    Arg,
    decltype(is_reference_constructible_impl<Ty>::imaginary_function(std::declval<Arg>()))> :
    std::bool_constant<
        noexcept(is_reference_constructible_impl<Ty>::imaginary_function(std::declval<Arg>()))> {
};

template <class Ty>
class variant_alternative_storage<Ty&> : public traits<Ty*> {
    Ty* value;

public:
    using value_type = Ty&;
    static constexpr std::size_t storage_size = sizeof(Ty*);

    // rewrite some traits in the base class
    template <class... Args>
    using is_constructible = std::conjunction<
        std::bool_constant<sizeof...(Args) == 1>,
        is_reference_constructible<Ty, Args...>>;

    template <class... Args>
    using is_trivially_constructible =
        std::conjunction<is_constructible<Args...>, std::is_trivially_constructible<Ty&, Args...>>;

    template <class... Args>
    using is_nothrow_constructible =
        std::conjunction<is_constructible<Args...>, std::is_nothrow_constructible<Ty&, Args...>>;

    using is_default_constructible = std::false_type;
    using is_trivially_default_constructible = std::false_type;
    using is_nothrow_default_constructible = std::false_type;

    template <class Arg>
    using is_assignable = is_constructible<Arg>;

    template <class Arg>
    using is_trivially_assignable = is_trivially_constructible<Arg>;

    template <class Arg>
    using is_nothrow_assignable = is_nothrow_constructible<Arg>;

    template <
        class Arg,
        typename std::enable_if<
            std::conjunction<
                std::negation<
                    std::is_same<typename remove_cvref<Arg>::type, variant_alternative_storage>>,
                is_constructible<Arg>>::value,
            int>::type = 0>
    CONSTEXPR17 explicit variant_alternative_storage(Arg&& arg) noexcept(  //
        is_nothrow_constructible<Arg>::value
    ) :
        value(std::addressof(static_cast<Ty&>(std::forward<Arg>(arg)))) { }

    template <
        class Arg,
        typename std::enable_if<
            std::conjunction<
                std::negation<
                    std::is_same<typename remove_cvref<Arg>::type, variant_alternative_storage>>,
                is_assignable<Arg>>::value,
            int>::type = 0>
    CONSTEXPR17 auto operator=(Arg&& arg) noexcept(  //
        is_nothrow_assignable<Arg>::value
    ) -> variant_alternative_storage& {
        value = std::addressof(static_cast<Ty&>(std::forward<Arg>(arg)));
        return *this;
    }

    CONSTEXPR17 void swap(variant_alternative_storage& other) {
        using std::swap;
        swap(value, other.value);
    }

    CONSTEXPR17 auto forward_value() const noexcept -> Ty& {
        return *value;
    }

    CONSTEXPR17 auto raw_value() const noexcept -> Ty* {
        return value;
    }

    CONSTEXPR17 auto raw_value() noexcept -> Ty*& {
        return value;
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_ALTERNATIVE_STORAGE_HPP
