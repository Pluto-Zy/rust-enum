#include "common.hpp"

#ifdef USE_CXX20
    #include <compare>

namespace rust {
namespace {
template <class T1, class T2>
constexpr auto test_comparison_complete(  //
    const T1& lhs,
    const T2& rhs,
    std::partial_ordering order
) -> bool {
    #define CHECK_BODY(op)                                                                        \
        if ((lhs op rhs) != (order op 0))                                                         \
            return false;                                                                         \
        if ((rhs op lhs) != (0 op order))                                                         \
            return false;

    CHECK_BODY(==)
    CHECK_BODY(!=)
    CHECK_BODY(<)
    CHECK_BODY(>)
    CHECK_BODY(<=)
    CHECK_BODY(>=)

    return true;
    #undef CHECK_BODY
}

template <class T1, class T2, class Order>
constexpr auto test_order(const T1& lhs, const T2& rhs, Order order) -> bool {
    if ((lhs <=> rhs) != order)
        return false;
    return test_comparison_complete(lhs, rhs, order);
}

TEST(VariantTestThreeWayComparison, Valueless) {
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x2);
        EXPECT_TRUE(test_order(x1, x2, std::weak_ordering::greater));
    }
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x1);
        EXPECT_TRUE(test_order(x1, x2, std::weak_ordering::less));
    }
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x1);
        make_valueless(x2);
        EXPECT_TRUE(test_order(x1, x2, std::weak_ordering::equivalent));
    }
}

template <class T1, class T2, class Order>
void test_with_types() {
    using v = variant<T1, T2>;

    static_assert(std::is_same<decltype(std::declval<T1>() <=> std::declval<T2>()), Order>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() <=> std::declval<T1>()), Order>::value);
    static_assert(std::is_same<decltype(std::declval<T1>() == std::declval<T2>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() == std::declval<T1>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T1>() != std::declval<T2>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() != std::declval<T1>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T1>() < std::declval<T2>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() < std::declval<T1>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T1>() <= std::declval<T2>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() <= std::declval<T1>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T1>() > std::declval<T2>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() > std::declval<T1>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T1>() >= std::declval<T2>()), bool>::value);
    static_assert(std::is_same<decltype(std::declval<T2>() >= std::declval<T1>()), bool>::value);

    {
        constexpr v x1(std::in_place_index<0>, T1 { 1 });
        constexpr v x2(std::in_place_index<0>, T1 { 1 });
        static_assert(test_order(x1, x2, Order::equivalent));
    }
    {
        constexpr v x1(std::in_place_index<0>, T1 { 0 });
        constexpr v x2(std::in_place_index<0>, T1 { 1 });
        static_assert(test_order(x1, x2, Order::less));
    }
    {
        constexpr v x1(std::in_place_index<0>, T1 { 1 });
        constexpr v x2(std::in_place_index<0>, T1 { 0 });
        static_assert(test_order(x1, x2, Order::greater));
    }
    {
        constexpr v x1(std::in_place_index<0>, T1 { 1 });
        constexpr v x2(std::in_place_index<1>, T1 { 1 });
        static_assert(test_order(x1, x2, Order::less));
    }
    {
        constexpr v x1(std::in_place_index<1>, T1 { 1 });
        constexpr v x2(std::in_place_index<0>, T1 { 1 });
        static_assert(test_order(x1, x2, Order::greater));
    }
}

TEST(VariantTestThreeWayComparison, Basic) {
    test_with_types<int, double, std::partial_ordering>();
    test_with_types<int, long, std::strong_ordering>();

    {
        using v = variant<int, double>;
        constexpr double nan = std::numeric_limits<double>::quiet_NaN();
        {
            constexpr v x1(std::in_place_type<int>, 1);
            constexpr v x2(std::in_place_type<double>, nan);
            EXPECT_TRUE(test_order(x1, x2, std::partial_ordering::less));
        }
        {
            constexpr v x1(std::in_place_type<double>, nan);
            constexpr v x2(std::in_place_type<int>, 2);
            EXPECT_TRUE(test_order(x1, x2, std::partial_ordering::greater));
        }
        {
            constexpr v x1(std::in_place_type<double>, nan);
            constexpr v x2(std::in_place_type<double>, nan);
            EXPECT_TRUE(test_order(x1, x2, std::partial_ordering::unordered));
        }
    }
}

template <class T, class U = T>
concept has_three_way_op = requires(T& t, U& u) { t <=> u; };

TEST(VariantTestThreeWayComparison, Deleted) {
    // std::three_way_comparable is a more stringent requirement that demands operator== and a few
    // other things.
    using std::three_way_comparable;

    struct has_simple_ordering {
        constexpr auto operator==(const has_simple_ordering&) const -> bool;
        constexpr auto operator<(const has_simple_ordering&) const -> bool;
    };

    struct has_only_spaceship {
        constexpr auto operator==(const has_only_spaceship&) const -> bool = delete;
        constexpr auto operator<=>(const has_only_spaceship&) const -> std::weak_ordering;
    };

    struct has_full_ordering {
        constexpr auto operator==(const has_full_ordering&) const -> bool;
        constexpr auto operator<=>(const has_full_ordering&) const -> std::weak_ordering;
    };

    // operator<=> must resolve the return types of all its union types' operator<=>s to determine
    // its own return type, so it is detectable by SFINAE
    static_assert(!has_three_way_op<has_simple_ordering>);
    static_assert(!has_three_way_op<variant<int, has_simple_ordering>>);

    static_assert(!three_way_comparable<has_simple_ordering>);
    static_assert(!three_way_comparable<variant<int, has_simple_ordering>>);

    static_assert(has_three_way_op<has_only_spaceship>);
    static_assert(!has_three_way_op<variant<int, has_only_spaceship>>);

    static_assert(!three_way_comparable<has_only_spaceship>);
    static_assert(!three_way_comparable<variant<int, has_only_spaceship>>);

    static_assert(has_three_way_op<has_full_ordering>);
    static_assert(has_three_way_op<variant<int, has_full_ordering>>);

    static_assert(three_way_comparable<has_full_ordering>);
    static_assert(three_way_comparable<variant<int, has_full_ordering>>);
}
}  // namespace
}  // namespace rust

#endif
