#include <bitset>
#include <common.hpp>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace rust {
namespace {

TEST(VariantTestP0608R3, P0608R3) {
    // Test cases from P0608R3
    {
        using v = variant<std::string, bool>;
        static_assert(std::is_constructible<v, const char*>::value);
        static_assert(std::is_assignable<v, const char*>::value);
        static_assert(std::is_constructible<v, const char[4]>::value);
        static_assert(std::is_assignable<v, const char[4]>::value);
        static_assert(std::is_constructible<v, std::string>::value);
        static_assert(std::is_assignable<v, std::string>::value);

        v x1 = "abc";
        EXPECT_EQ(x1.index(), 0);
        EXPECT_TRUE(holds_alternative<std::string>(x1));
        EXPECT_FALSE(holds_alternative<bool>(x1));
        EXPECT_EQ(get<0>(x1), "abc");
        EXPECT_EQ(get<std::string>(x1), "abc");
        EXPECT_THROW(get<1>(x1), bad_variant_access);
        EXPECT_THROW(get<bool>(x1), bad_variant_access);

        v x2(std::in_place_index<1>);
        EXPECT_EQ(x2.index(), 1);
        x2 = "abc";
        EXPECT_EQ(x2.index(), 0);
        EXPECT_TRUE(holds_alternative<std::string>(x2));
        EXPECT_FALSE(holds_alternative<bool>(x2));
        EXPECT_EQ(get<0>(x2), "abc");
        EXPECT_EQ(get<std::string>(x2), "abc");
        EXPECT_THROW(get<1>(x2), bad_variant_access);
        EXPECT_THROW(get<bool>(x2), bad_variant_access);
    }
    {
        using v = variant<char, std::optional<char16_t>>;
        static_assert(std::is_constructible<v, char16_t>::value);
        static_assert(std::is_assignable<v, char16_t>::value);

        v x1 = u'\u2043';
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<std::optional<char16_t>>(x1));
        EXPECT_THROW(get<0>(x1), bad_variant_access);
        EXPECT_THROW(get<char>(x1), bad_variant_access);
        EXPECT_EQ(get<1>(x1), u'\u2043');
        EXPECT_EQ(get<std::optional<char16_t>>(x1), u'\u2043');
    }
    {
        using v = variant<char, std::optional<char16_t>>;
        static_assert(std::is_constructible<v, char16_t>::value);
        static_assert(std::is_assignable<v, char16_t>::value);

        v x1 = u'\u2043';
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<std::optional<char16_t>>(x1));
        EXPECT_THROW(get<0>(x1), bad_variant_access);
        EXPECT_THROW(get<char>(x1), bad_variant_access);
        EXPECT_EQ(get<1>(x1), u'\u2043');
        EXPECT_EQ(get<std::optional<char16_t>>(x1), u'\u2043');

        v x2(std::in_place_index<0>);
        EXPECT_EQ(x2.index(), 0);
        x2 = u'\u2043';
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<std::optional<char16_t>>(x2));
        EXPECT_THROW(get<0>(x2), bad_variant_access);
        EXPECT_THROW(get<char>(x2), bad_variant_access);
        EXPECT_EQ(get<1>(x2), u'\u2043');
        EXPECT_EQ(get<std::optional<char16_t>>(x2), u'\u2043');
    }
    {
        using v = variant<int, std::reference_wrapper<double>>;
        static_assert(std::is_constructible<v, double&>::value);
        static_assert(std::is_assignable<v, double&>::value);

        double d = 3.14;
        v y1 = d;
        EXPECT_EQ(y1.index(), 1);
        EXPECT_TRUE(holds_alternative<std::reference_wrapper<double>>(y1));
        EXPECT_EQ(get<1>(y1), d);
        EXPECT_EQ(get<std::reference_wrapper<double>>(y1), d);

        v y2;
        EXPECT_EQ(y2.index(), 0);
        y2 = d;
        EXPECT_EQ(y2.index(), 1);
        EXPECT_TRUE(holds_alternative<std::reference_wrapper<double>>(y2));
        EXPECT_EQ(get<1>(y2), d);
        EXPECT_EQ(get<std::reference_wrapper<double>>(y2), d);
    }
    {
        using v = variant<float, int>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = 0;
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<int>(x));
    }
    {
        using v = variant<float, long>;
        static_assert(std::is_constructible<v, int>::value);
        static_assert(std::is_assignable<v, int>::value);

        v x;
        EXPECT_EQ(x.index(), 0);
        x = 0;
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x));
    }
    {
        static_assert(!std::is_constructible<variant<float>, int>::value);
        static_assert(!std::is_assignable<variant<float>, int>::value);
        static_assert(!std::is_constructible<variant<float, std::vector<int>>, int>::value);
        static_assert(!std::is_assignable<variant<float, std::vector<int>>, int>::value);
        static_assert(!std::is_constructible<variant<float, char>, int>::value);
        static_assert(!std::is_assignable<variant<float, char>, int>::value);
    }
    {
        using v = variant<float, int>;
        static_assert(std::is_constructible<v, char>::value);
        static_assert(std::is_assignable<v, char>::value);

        v x1 = 'a';
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<int>(x1));
        EXPECT_EQ(get<1>(x1), int('a'));
        EXPECT_EQ(get<int>(x1), int('a'));

        v x2;
        EXPECT_EQ(x2.index(), 0);
        x2 = 'a';
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<int>(x2));
        EXPECT_EQ(get<1>(x2), int('a'));
        EXPECT_EQ(get<int>(x2), int('a'));
    }
    {
        using v = variant<float, long>;
        static_assert(std::is_constructible<v, int>::value);
        static_assert(std::is_assignable<v, int>::value);

        v x1 = 0;
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x1));

        v x2;
        EXPECT_EQ(x2.index(), 0);
        x2 = 0;
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x2));
    }
    {
        using v = variant<float, long, double>;
        static_assert(std::is_constructible<v, int>::value);
        static_assert(std::is_assignable<v, int>::value);

        v x1 = 0;
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x1));

        v x2;
        EXPECT_EQ(x2.index(), 0);
        x2 = 0;
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x2));
    }
}

TEST(VariantTestP0608R3, P1957R2) {
    // Test cases from P1957R2 and MSVC STL
    static_assert(std::is_constructible<variant<bool, int>, bool>::value);
    static_assert(std::is_constructible<variant<bool, int>, std::bitset<4>::reference>::value);
    static_assert(std::is_constructible<variant<bool>, std::bitset<4>::reference>::value);

    static_assert(std::is_assignable<variant<bool, int>, bool>::value);
    static_assert(std::is_assignable<variant<bool, int>, std::bitset<4>::reference>::value);
    static_assert(std::is_assignable<variant<bool>, std::bitset<4>::reference>::value);

    std::bitset<4> a_bitset("0101");
    bool a_data = a_bitset[2];
    variant<bool, int> a = a_data;  // bool
    EXPECT_EQ(a.index(), 0);
    EXPECT_TRUE(get<0>(a));

    std::bitset<4> b_bitset("0101");
    variant<bool, int> b = b_bitset[2];  // bool
    variant<bool, int> b2 = b_bitset[1];  // bool
    EXPECT_EQ(b.index(), 0);
    EXPECT_TRUE(get<0>(b));
    EXPECT_EQ(b2.index(), 0);
    EXPECT_FALSE(get<0>(b2));
}

struct double_double {
    double x;

    double_double(double x) : x(x) { }
};

struct convertible_bool {
    convertible_bool(bool x) : x(x) { }
    ~convertible_bool() = default;

    operator bool() const noexcept {
        return x;
    }

    bool x;
};

struct default_struct { };

TEST(VariantTestP0608R3, MoreCases) {
    // More cases from MSVC STL.
    static_assert(std::is_constructible<variant<double_double>, double>::value);
    static_assert(std::is_constructible<
                  variant<std::vector<std::vector<int>>, std::optional<int>, int>,
                  int>::value);
    static_assert(
        std::is_constructible<variant<std::vector<std::vector<int>>, std::optional<int>>, int>::
            value
    );
    static_assert(
        std::is_constructible<variant<std::vector<int>, std::optional<int>, float>, int>::value
    );
    static_assert(std::is_constructible<variant<bool>, convertible_bool>::value);
    static_assert(std::is_constructible<variant<bool, int>, convertible_bool>::value);
    static_assert(std::is_constructible<variant<convertible_bool>, bool>::value);
    static_assert(
        std::is_constructible<variant<float, bool, convertible_bool>, convertible_bool>::value
    );
    static_assert(std::is_constructible<variant<float, bool, convertible_bool>, bool>::value);
    static_assert(std::is_constructible<variant<char, int>, bool>::value);
    static_assert(std::is_constructible<variant<double_double>, int>::value);
    static_assert(!std::is_constructible<variant<float>, unsigned int>::value);
    static_assert(!std::is_constructible<variant<char, default_struct>, int>::value);
    static_assert(!std::is_constructible<variant<float, long, long long>, int>::value);

    static_assert(std::is_assignable<variant<double_double>, double>::value);
    static_assert(
        std::is_assignable<variant<std::vector<std::vector<int>>, std::optional<int>, int>, int>::
            value
    );
    static_assert(
        std::is_assignable<variant<std::vector<std::vector<int>>, std::optional<int>>, int>::value
    );
    static_assert(
        std::is_assignable<variant<std::vector<int>, std::optional<int>, float>, int>::value
    );
    static_assert(std::is_assignable<variant<bool>, convertible_bool>::value);
    static_assert(std::is_assignable<variant<bool, int>, convertible_bool>::value);
    static_assert(std::is_assignable<variant<convertible_bool>, bool>::value);
    static_assert(
        std::is_assignable<variant<float, bool, convertible_bool>, convertible_bool>::value
    );
    static_assert(std::is_assignable<variant<float, bool, convertible_bool>, bool>::value);
    static_assert(std::is_assignable<variant<char, int>, bool>::value);
    static_assert(std::is_assignable<variant<double_double>, int>::value);
    static_assert(!std::is_assignable<variant<float>, unsigned int>::value);
    static_assert(!std::is_assignable<variant<char, default_struct>, int>::value);
    static_assert(!std::is_assignable<variant<float, long, long long>, int>::value);

    {
        using v = variant<char, int, float, bool, std::vector<bool>>;
        v x = true;
        EXPECT_EQ(x.index(), 3);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<3>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<char, int, float, bool, std::vector<bool>>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = true;
        EXPECT_EQ(x.index(), 3);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<3>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<bool, int>;
        v x = convertible_bool { true };
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<0>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<int, bool>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = convertible_bool { true };
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<1>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<char, int, bool>;
        v x = true;
        EXPECT_EQ(x.index(), 2);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<2>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<char, int, bool>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = true;
        EXPECT_EQ(x.index(), 2);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<2>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<char, int, convertible_bool>;
        v x = convertible_bool { true };
        EXPECT_EQ(x.index(), 2);
        EXPECT_TRUE(holds_alternative<convertible_bool>(x));
        EXPECT_TRUE(get<2>(x));
        EXPECT_TRUE(get<convertible_bool>(x));
    }
    {
        using v = variant<char, int, convertible_bool>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = convertible_bool { true };
        EXPECT_EQ(x.index(), 2);
        EXPECT_TRUE(holds_alternative<convertible_bool>(x));
        EXPECT_TRUE(get<2>(x));
        EXPECT_TRUE(get<convertible_bool>(x));
    }
    {
        using v = variant<float, bool, convertible_bool>;
        v x = bool {};
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_FALSE(get<1>(x));
        EXPECT_FALSE(get<bool>(x));
    }
    {
        using v = variant<float, bool, convertible_bool>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = bool {};
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_FALSE(get<1>(x));
        EXPECT_FALSE(get<bool>(x));
    }
    {
        using v = variant<float, bool>;
        v x = convertible_bool { false };
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_FALSE(get<1>(x));
        EXPECT_FALSE(get<bool>(x));
    }
    {
        using v = variant<float, bool>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = convertible_bool { false };
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_FALSE(get<1>(x));
        EXPECT_FALSE(get<bool>(x));
    }
    {
        using v = variant<int, bool>;
        v x = std::true_type {};
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<1>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        using v = variant<int, bool>;
        v x;
        EXPECT_EQ(x.index(), 0);
        x = std::true_type {};
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<bool>(x));
        EXPECT_TRUE(get<1>(x));
        EXPECT_TRUE(get<bool>(x));
    }
    {
        variant<std::string, bool, int> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<std::string>(x), "");
        x = 3;
        EXPECT_EQ(x.index(), 2);
        EXPECT_EQ(get<int>(x), 3);
        x = true;
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(get<bool>(x));
    }
    {
        bool data = true;
        variant<bool, int, double_double> x = data;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), data);
        x = 12;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 12);
        x = 12.5;
        EXPECT_EQ(x.index(), 2);
        EXPECT_EQ(get<2>(x).x, 12.5);
    }
    {
        variant<void*, bool, double_double> x;
        EXPECT_EQ(x.index(), 0);
        x = false;
        EXPECT_EQ(x.index(), 1);
        EXPECT_FALSE(get<1>(x));
        x = 5.12;
        EXPECT_EQ(x.index(), 2);
        EXPECT_EQ(get<2>(x).x, 5.12);
        double_double data = 1.2;
        x = &data;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(static_cast<double_double*>(get<0>(x))->x, 1.2);
    }
}
}  // namespace
}  // namespace rust