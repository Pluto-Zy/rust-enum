#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestDefaultConstructor, Deleted) {
    static_assert(std::is_default_constructible<variant<int>>::value);
    static_assert(std::is_default_constructible<variant<int, long>>::value);
    static_assert(std::is_default_constructible<variant<monostate>>::value);
    static_assert(std::is_default_constructible<variant<int, monostate>>::value);

    static_assert(!std::is_default_constructible<variant<int&>>::value);
    static_assert(!std::is_default_constructible<variant<int const&>>::value);
    static_assert(!std::is_default_constructible<variant<int&, int>>::value);
    static_assert(std::is_default_constructible<variant<int, int&>>::value);

    {
        struct non_default_constructible {
            non_default_constructible(int);
        };
        static_assert(!std::is_default_constructible<variant<non_default_constructible>>::value);
        static_assert(std::is_default_constructible<variant<int, non_default_constructible>>::value
        );
        static_assert(
            !std::is_default_constructible<variant<non_default_constructible, int>>::value
        );
    }
}

TEST(VariantTestDefaultConstructor, Noexcept) {
    static_assert(std::is_nothrow_default_constructible<variant<int>>::value);
    static_assert(std::is_nothrow_default_constructible<variant<int, long>>::value);
    static_assert(std::is_nothrow_default_constructible<variant<monostate>>::value);
    static_assert(std::is_nothrow_default_constructible<variant<int, monostate>>::value);

    static_assert(!std::is_nothrow_default_constructible<variant<int&>>::value);
    static_assert(!std::is_nothrow_default_constructible<variant<int const&>>::value);
    static_assert(!std::is_nothrow_default_constructible<variant<int&, int>>::value);
    static_assert(std::is_nothrow_default_constructible<variant<int, int&>>::value);

    {
        struct may_throw_default_constructible {
            may_throw_default_constructible();
        };
        static_assert(
            !std::is_nothrow_default_constructible<variant<may_throw_default_constructible>>::value
        );
        static_assert(!std::is_nothrow_default_constructible<
                      variant<may_throw_default_constructible, int>>::value);
        static_assert(std::is_nothrow_default_constructible<
                      variant<int, may_throw_default_constructible>>::value);
    }
    {
        struct throw_default_constructible {
            throw_default_constructible() {
                throw 1;
            }
        };

        using v1 = variant<throw_default_constructible>;
        EXPECT_THROW(v1 x, int);

        using v2 = variant<throw_default_constructible, int>;
        EXPECT_THROW(v2 x, int);

        using v3 = variant<int, throw_default_constructible>;
        v3 x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 0);
    }
}

TEST(VariantTestDefaultConstructor, BasicBehavior) {
    {
        variant<int> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<int>(x));
        EXPECT_EQ(get<0>(x), 0);
        EXPECT_EQ(get<int>(x), 0);
    }
    {
        variant<long, int> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<long>(x));
        EXPECT_EQ(get<0>(x), 0l);
        EXPECT_EQ(get<long>(x), 0l);
    }
    {
        variant<std::string> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<std::string>(x));
        EXPECT_EQ(get<0>(x), "");
        EXPECT_EQ(get<std::string>(x), "");
    }
    {
        variant<std::string, int> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<std::string>(x));
        EXPECT_EQ(get<0>(x), "");
        EXPECT_EQ(get<std::string>(x), "");
    }
    {
        variant<counter> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(counter::alive_count, 1);
        counter::reset();
    }
    {
        variant<counter, int> x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(counter::alive_count, 1);
        counter::reset();
    }
}

TEST(VariantTestDefaultConstructor, Constexpr) {
    {
        constexpr variant<int> x;
        static_assert(x.index() == 0);
        static_assert(holds_alternative<int>(x));
        static_assert(get<0>(x) == 0);
        static_assert(get<int>(x) == 0);
    }
    {
        constexpr variant<long, int> x;
        static_assert(x.index() == 0);
        static_assert(holds_alternative<long>(x));
        static_assert(get<0>(x) == 0l);
        static_assert(get<long>(x) == 0l);
    }
}
}  // namespace
}  // namespace rust
