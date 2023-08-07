#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
struct my_bool {
    bool value;

    constexpr explicit my_bool(bool v) : value(v) { }
    constexpr operator bool() const noexcept {
        return value;
    }
};

struct compares_to_my_bool {
    int value = 0;
};

constexpr auto operator==(  //
    const compares_to_my_bool& lhs,
    const compares_to_my_bool& rhs
) noexcept -> my_bool {
    return my_bool(lhs.value == rhs.value);
}

constexpr auto operator!=(  //
    const compares_to_my_bool& lhs,
    const compares_to_my_bool& rhs
) noexcept -> my_bool {
    return my_bool(lhs.value != rhs.value);
}

constexpr auto operator<(  //
    const compares_to_my_bool& lhs,
    const compares_to_my_bool& rhs
) noexcept -> my_bool {
    return my_bool(lhs.value < rhs.value);
}

constexpr auto operator<=(  //
    const compares_to_my_bool& lhs,
    const compares_to_my_bool& rhs
) noexcept -> my_bool {
    return my_bool(lhs.value <= rhs.value);
}

constexpr auto operator>(  //
    const compares_to_my_bool& lhs,
    const compares_to_my_bool& rhs
) noexcept -> my_bool {
    return my_bool(lhs.value > rhs.value);
}

constexpr auto operator>=(  //
    const compares_to_my_bool& lhs,
    const compares_to_my_bool& rhs
) noexcept -> my_bool {
    return my_bool(lhs.value >= rhs.value);
}

template <class T1, class T2>
void test_equality_basic() {
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 42 });
        constexpr v x2(std::in_place_index<0>, T1 { 42 });
        static_assert(x1 == x2);
        static_assert(x2 == x1);
        static_assert(!(x1 != x2));
        static_assert(!(x2 != x1));
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 42 });
        constexpr v x2(std::in_place_index<0>, T1 { 43 });
        static_assert(!(x1 == x2));
        static_assert(!(x2 == x1));
        static_assert(x1 != x2);
        static_assert(x2 != x1);
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 42 });
        constexpr v x2(std::in_place_index<1>, T2 { 42 });
        static_assert(!(x1 == x2));
        static_assert(!(x2 == x1));
        static_assert(x1 != x2);
        static_assert(x2 != x1);
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<1>, T2 { 42 });
        constexpr v x2(std::in_place_index<1>, T2 { 42 });
        static_assert(x1 == x2);
        static_assert(x2 == x1);
        static_assert(!(x1 != x2));
        static_assert(!(x2 != x1));
    }
}

TEST(VariantTestComparison, Equality) {
    test_equality_basic<int, int>();
    test_equality_basic<int, long>();
    test_equality_basic<compares_to_my_bool, int>();
    test_equality_basic<int, compares_to_my_bool>();
    test_equality_basic<compares_to_my_bool, compares_to_my_bool>();

    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x2);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        EXPECT_FALSE(x1 == x2);
        EXPECT_FALSE(x2 == x1);
        EXPECT_TRUE(x1 != x2);
        EXPECT_TRUE(x2 != x1);
    }
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x1);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_FALSE(x1 == x2);
        EXPECT_FALSE(x2 == x1);
        EXPECT_TRUE(x1 != x2);
        EXPECT_TRUE(x2 != x1);
    }
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x1);
        make_valueless(x2);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        EXPECT_TRUE(x1 == x2);
        EXPECT_TRUE(x2 == x1);
        EXPECT_FALSE(x1 != x2);
        EXPECT_FALSE(x2 != x1);
    }
    {
        using v = variant<int&, long>;
        int x1 = 3, x2 = 3;
        v v1(std::in_place_index<0>, x1);
        v v2(std::in_place_index<0>, x2);
        EXPECT_TRUE(v1 == v2);
        EXPECT_FALSE(v1 != v2);
        x1 = 4;
        EXPECT_FALSE(v1 == v2);
        EXPECT_TRUE(v1 != v2);
        v v3(std::in_place_index<1>, 3);
        EXPECT_FALSE(v2 == v3);
        EXPECT_TRUE(v2 != v3);
    }
    {
        using v = variant<int&, int&>;
        int x = 3;
        v v1(std::in_place_index<0>, x);
        v v2(std::in_place_index<1>, x);
        EXPECT_FALSE(v1 == v2);
        EXPECT_TRUE(v1 != v2);
        v v3(std::in_place_index<0>, x);
        EXPECT_TRUE(v1 == v3);
        EXPECT_FALSE(v1 != v3);
    }
}

template <class V>
constexpr auto test_less(const V& l, const V& r, bool expect_less, bool expect_greater) -> bool {
    static_assert(std::is_same<decltype(l < r), bool>::value);
    static_assert(std::is_same<decltype(l <= r), bool>::value);
    static_assert(std::is_same<decltype(l > r), bool>::value);
    static_assert(std::is_same<decltype(l >= r), bool>::value);

    return ((l < r) == expect_less) && (!(l >= r) == expect_less) && ((l > r) == expect_greater)
        && (!(l <= r) == expect_greater);
}

template <class T1, class T2>
void test_relational_basic() {
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 1 });
        constexpr v x2(std::in_place_index<0>, T1 { 1 });
        static_assert(test_less(x1, x2, false, false));
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 0 });
        constexpr v x2(std::in_place_index<0>, T1 { 1 });
        static_assert(test_less(x1, x2, true, false));
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 1 });
        constexpr v x2(std::in_place_index<0>, T1 { 0 });
        static_assert(test_less(x1, x2, false, true));
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<0>, T1 { 0 });
        constexpr v x2(std::in_place_index<1>, T2 { 0 });
        static_assert(test_less(x1, x2, true, false));
    }
    {
        using v = variant<T1, T2>;
        constexpr v x1(std::in_place_index<1>, T2 { 0 });
        constexpr v x2(std::in_place_index<0>, T1 { 0 });
        static_assert(test_less(x1, x2, false, true));
    }
}

TEST(VariantTestComparison, Relational) {
    test_relational_basic<int, int>();
    test_relational_basic<int, long>();
    test_relational_basic<compares_to_my_bool, int>();
    test_relational_basic<int, compares_to_my_bool>();
    test_relational_basic<compares_to_my_bool, compares_to_my_bool>();

    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x2);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        EXPECT_TRUE(test_less(x1, x2, false, true));
    }
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x1);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_TRUE(test_less(x1, x2, true, false));
    }
    {
        using v = variant<int, valueless_t>;
        v x1, x2;
        make_valueless(x1);
        make_valueless(x2);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        EXPECT_TRUE(test_less(x1, x2, false, false));
    }
    {
        using v = variant<int&, const long>;
        int x1 = 3, x2 = 3;
        v v1(std::in_place_index<0>, x1);
        v v2(std::in_place_index<0>, x2);
        EXPECT_TRUE(test_less(v1, v2, false, false));
        x2 = 4;
        EXPECT_TRUE(test_less(v1, v2, true, false));
        v v3(std::in_place_index<1>, 2);
        EXPECT_TRUE(test_less(v1, v3, true, false));
    }
    {
        using v = variant<int&, int&>;
        int x = 3;
        v v1(std::in_place_index<0>, x);
        v v2(std::in_place_index<1>, x);
        EXPECT_TRUE(test_less(v1, v2, true, false));
    }
}
}  // namespace
}  // namespace rust
