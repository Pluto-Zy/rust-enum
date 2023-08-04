#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

#ifdef USE_CXX20
    #include <memory>
    #include <string>
#endif

namespace rust {
namespace {
TEST(VariantTestConvertingAssignment, ConvertingAssignable) {
    // Test cases from MSVC STL.
    static_assert(std::is_assignable<variant<int>, int>::value);
    static_assert(!std::is_assignable<variant<int, int>, int>::value);
    static_assert(!std::is_assignable<variant<long, long long>, int>::value);
    static_assert(std::is_assignable<variant<float, long, double>, int>::value);

#ifdef USE_CXX20
    static_assert(!std::is_assignable<variant<char>, int>::value);

    static_assert(!std::is_assignable<variant<std::string, float>, int>::value);
    static_assert(!std::is_assignable<variant<std::string, double>, int>::value);
    static_assert(!std::is_assignable<variant<std::string, bool>, int>::value);

    static_assert(!std::is_assignable<variant<int, bool>, decltype("meow")>::value);
    static_assert(!std::is_assignable<variant<int, const bool>, decltype("meow")>::value);
    #ifndef USE_CXX20
    static_assert(!std::is_assignable<variant<int, const volatile bool>, decltype("meow")>::value);
    #endif

    static_assert(std::is_assignable<variant<bool>, std::true_type>::value);
    static_assert(!std::is_assignable<variant<bool>, std::unique_ptr<char>>::value);

    static_assert(!std::is_assignable<variant<bool>, decltype(nullptr)>::value);
#endif

    static_assert(!std::is_assignable<variant<int&>, int>::value);
    static_assert(std::is_assignable<variant<int&>, int&>::value);
    static_assert(!std::is_assignable<variant<int&>, float&>::value);
    static_assert(!std::is_assignable<variant<int&>, const int&>::value);
    static_assert(std::is_assignable<variant<int const&>, int&>::value);
    static_assert(std::is_assignable<variant<int const&>, int const&>::value);
    static_assert(!std::is_assignable<variant<int const&>, int>::value);
    static_assert(!std::is_assignable<variant<int, int&>, int&>::value);
    static_assert(std::is_assignable<variant<int&, const int&>, int&>::value);
    static_assert(std::is_assignable<variant<int&, const int&>, const int&>::value);
    static_assert(!std::is_assignable<variant<int, const int&>, int&>::value);

    {
        struct base { };
        struct derived : base { };

        static_assert(std::is_assignable<variant<base&>, derived&>::value);
        static_assert(std::is_assignable<variant<base&, derived&>, derived&>::value);
    }
    {
        struct conv {
            operator int&();
        };

        static_assert(std::is_assignable<variant<int&>, conv&>::value);
        static_assert(std::is_assignable<variant<int&>, conv>::value);
    }
}

TEST(VariantTestConvertingAssignment, Deleted) {
    static_assert(std::is_assignable<variant<long>, int>::value);
    static_assert(std::is_assignable<variant<monostate>, monostate>::value);
    static_assert(std::is_assignable<variant<int, monostate>, monostate>::value);
    static_assert(!std::is_assignable<variant<const int>, int>::value);
    static_assert(std::is_assignable<variant<int, const double>, int>::value);
    static_assert(!std::is_assignable<variant<int, const double>, double>::value);
    static_assert(!std::is_assignable<variant<long, long long>, int>::value);
    static_assert(!std::is_assignable<variant<std::string, std::string>, const char*>::value);
    static_assert(!std::is_assignable<variant<std::string, void*>, int>::value);

#ifdef USE_CXX20
    static_assert(!std::is_assignable<variant<std::string, float>, int>::value);
    static_assert(
        !std::is_assignable<variant<std::unique_ptr<int>, bool>, std::unique_ptr<char>>::value
    );
#endif  // USE_CXX20

    {
        using v = variant<int, float, int, double, float>;
        static_assert(!std::is_assignable<v, int>::value);
        static_assert(!std::is_assignable<v, float>::value);
        static_assert(std::is_assignable<v, double>::value);
    }

    {
        struct X { };
        struct Y {
            operator X();
        };
        static_assert(std::is_assignable<variant<X>, Y>::value);
    }
}

TEST(VariantTestConvertingAssignment, Noexcept) {
    static_assert(std::is_nothrow_assignable<variant<int>, int>::value);
    static_assert(std::is_nothrow_assignable<variant<long>, int>::value);
    static_assert(std::is_nothrow_assignable<variant<monostate>, monostate>::value);
    static_assert(std::is_nothrow_assignable<variant<int, monostate>, monostate>::value);
    static_assert(std::is_nothrow_assignable<variant<int, const double>, int>::value);

    static_assert(std::is_nothrow_assignable<variant<int&>, int&>::value);
    static_assert(std::is_nothrow_assignable<variant<int&, double&>, double&>::value);
    static_assert(std::is_nothrow_assignable<variant<std::string, int&>, int&>::value);

    // Test cases from MSVC STL.
    struct dummy { };
    {
        struct nothrow {
            nothrow(int) noexcept { }
            nothrow& operator=(int) noexcept {
                return *this;
            }
        };
        static_assert(std::is_nothrow_assignable<variant<dummy, nothrow>, int>::value);
    }
    {
        struct throws_constructor {
            throws_constructor(int) noexcept(false) { }
            throws_constructor& operator=(int) noexcept {
                return *this;
            }
        };
        static_assert(!std::is_nothrow_assignable<variant<dummy, throws_constructor>, int>::value);
    }
    {
        struct throws_assignment {
            throws_assignment(int) noexcept { }
            throws_assignment& operator=(int) noexcept(false) {
                return *this;
            }
        };
        static_assert(!std::is_nothrow_assignable<variant<dummy, throws_assignment>, int>::value);
    }
    {
        struct throws_copy {
            throws_copy& operator=(const throws_copy&) noexcept(false) {
                return *this;
            }
        };
        static_assert(!std::is_nothrow_assignable<variant<throws_copy>, throws_copy>::value);
        static_assert(std::is_nothrow_assignable<variant<throws_copy&>, throws_copy&>::value);
    }
    {
        struct throws_conv {
            operator int&() noexcept(false);
        };

        static_assert(!std::is_nothrow_assignable<variant<int&>, throws_conv&>::value);
    }
}

TEST(VariantTestConvertingAssignment, BasicBehavior) {
    {
        using v = variant<int>;
        v x(3);
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 3);
        x = 2;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 2);
    }
    {
        using v = variant<int, long>;
        v x(3l);
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
        x = 2;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 2);
        x = 3l;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
    }
#ifdef USE_CXX20
    {
        using v = variant<unsigned, long>;
        v x;
        x = 3;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
        x = 2u;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 2u);
    }
    {
        using v = variant<std::string, bool>;
        v x = true;
        x = "bar";
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), "bar");
    }
#endif  // USE_CXX20
#ifndef USE_CXX20
    {
        using v = variant<bool volatile, int>;
        v x = 42;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 42);
        x = false;
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(get<0>(x));
        bool lvt = true;
        x = lvt;
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(get<0>(x));
    }
#endif
    {
        struct throw_assignable {
            int value;
            throw_assignable(int val) : value(val) { }
            throw_assignable(const throw_assignable&) = default;
            throw_assignable(throw_assignable&&) = default;
            throw_assignable& operator=(int) {
                throw 1;
            }
            throw_assignable& operator=(const throw_assignable&) = default;
            throw_assignable& operator=(throw_assignable&&) = default;
        };

        using v = variant<throw_assignable>;
        v x = 2;
        EXPECT_EQ(get<throw_assignable>(x).value, 2);
        EXPECT_THROW(x = 3, int);
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<throw_assignable>(x).value, 2);
    }
    {
        struct nothrow_assignable {
            int value;
            nothrow_assignable() : value(2) { }
            nothrow_assignable(int) {
                throw 1;
            }
            nothrow_assignable(const nothrow_assignable&) = default;
            nothrow_assignable(nothrow_assignable&&) = default;
            nothrow_assignable& operator=(int val) noexcept {
                value = val;
                return *this;
            }
            nothrow_assignable& operator=(const nothrow_assignable&) = default;
            nothrow_assignable& operator=(nothrow_assignable&&) = default;
        };

        using v = variant<nothrow_assignable>;
        v x;
        EXPECT_EQ(get<nothrow_assignable>(x).value, 2);
        x = 3;
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<nothrow_assignable>(x).value, 3);
    }
    {
        struct nothrow_constructible {
            int value;
            nothrow_constructible(int val) noexcept : value(val) { }
            nothrow_constructible(const nothrow_constructible&) = default;
            nothrow_constructible(nothrow_constructible&&) { }
            nothrow_constructible& operator=(int) {
                throw 1;
            }
            nothrow_constructible& operator=(const nothrow_constructible&) = default;
            nothrow_constructible& operator=(nothrow_constructible&&) = default;
        };

        using v = variant<std::string, nothrow_constructible>;
        v x(std::in_place_index<0>, "abc");
        EXPECT_EQ(x.index(), 0);
        x = 2;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<nothrow_constructible>(x).value, 2);
    }
    {
        struct throw_constructible_noexcept_move {
            int value;
            bool move_ctor;
            throw_constructible_noexcept_move() : value(2), move_ctor(false) { }
            throw_constructible_noexcept_move(int) {
                throw 1;
            }
            throw_constructible_noexcept_move(const throw_constructible_noexcept_move&) = default;
            throw_constructible_noexcept_move(throw_constructible_noexcept_move&&) noexcept {
                move_ctor = true;
            }
            throw_constructible_noexcept_move& operator=(int) {
                return *this;
            }
            throw_constructible_noexcept_move&
            operator=(const throw_constructible_noexcept_move&) = default;
            throw_constructible_noexcept_move& operator=(throw_constructible_noexcept_move&&) =
                default;
        };

        using v = variant<std::string, throw_constructible_noexcept_move>;
        v x = "abc";
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), "abc");
        EXPECT_THROW(x = 3, int);
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), "abc");
    }
    {
        struct throw_constructible_throw_move {
            int value;
            bool move_ctor;

            throw_constructible_throw_move() : value(2), move_ctor(false) { }
            throw_constructible_throw_move(int) {
                throw 1;
            }
            throw_constructible_throw_move(const throw_constructible_throw_move&) = default;
            throw_constructible_throw_move(throw_constructible_throw_move&&) {
                move_ctor = true;
            }
            throw_constructible_throw_move& operator=(const throw_constructible_throw_move&) =
                default;
            throw_constructible_throw_move& operator=(throw_constructible_throw_move&&) = default;
        };

        using v = variant<std::string, throw_constructible_throw_move>;
        v x = "abc";
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), "abc");
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_THROW(x = 3, int);
        EXPECT_TRUE(x.valueless_by_exception());
    }
    {
        using v = variant<int&>;
        int val = 3;
        v x = val;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 3);
        int val2 = 4;
        x = val2;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 4);
        EXPECT_EQ(val, 3);
        get<0>(x) = 5;
        EXPECT_EQ(val, 3);
        EXPECT_EQ(val2, 5);
    }
    {
        using v = variant<int&, long&>;
        long val = 3;
        v x = val;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
        int val2 = 4;
        x = val2;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 4);
        val2 = 5;
        EXPECT_EQ(get<0>(x), 5);
        x = val;
        EXPECT_EQ(get<1>(x), 3l);
        val = 6;
        EXPECT_EQ(get<1>(x), 6l);
    }
    {
        using v = variant<int&, long>;
        long val = 3;
        v x = val;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
        val = 4;
        EXPECT_EQ(get<1>(x), 3l);
        get<1>(x) = 5l;
        EXPECT_EQ(val, 4l);
        int val2 = 4;
        x = val2;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 4);
        val2 = 5;
        EXPECT_EQ(get<0>(x), 5);
        get<0>(x) = 6;
        EXPECT_EQ(val2, 6);
    }
}

TEST(VariantTestConvertingAssignment, ValuelessStrategy) {
    {
        using v = variant<dummy, may_throw_constructible_strategy_default>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::LET_VARIANT_DECIDE
        );
        // TODO: Remove this test.
        static_assert(!detail::variant_no_valueless_state<v>::value);

        v x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(x.valueless_by_exception());
        x = 3;
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(x.index(), 1);
        EXPECT_FALSE(get<1>(x).move_ctor);
    }
    {
        using v = variant<dummy, throw_constructible_strategy_default>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::LET_VARIANT_DECIDE
        );
        static_assert(!detail::variant_no_valueless_state<v>::value);

        v x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_THROW(x = 3, int);
        EXPECT_TRUE(x.valueless_by_exception());
    }
    {
        using v = variant<dummy, may_throw_constructible_strategy_fallback>;
        static_assert(
            variant_valueless_strategy<v>::value == variant_valueless_strategy_t::FALLBACK
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(x.valueless_by_exception());
        x = 3;
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(x.index(), 1);
        EXPECT_FALSE(get<1>(x).move_ctor);
    }
    {
        using v = variant<dummy, throw_constructible_strategy_fallback>;
        static_assert(
            variant_valueless_strategy<v>::value == variant_valueless_strategy_t::FALLBACK
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x(std::in_place_index<0>, 3);
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(get<0>(x).value, 3);
        EXPECT_THROW(x = 4, int);
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x).value, 2);
    }
    {
        using v = variant<dummy, may_throw_constructible_strategy_novalueless>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::DISALLOW_VALUELESS
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x;
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(x.valueless_by_exception());
        x = 3;
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(x.index(), 1);
        EXPECT_FALSE(get<1>(x).move_ctor);
    }
    {
        using v = variant<dummy, throw_constructible_strategy_novalueless>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::DISALLOW_VALUELESS
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x(std::in_place_index<0>, 3);
        EXPECT_EQ(x.index(), 0);
        EXPECT_FALSE(x.valueless_by_exception());
        EXPECT_EQ(get<0>(x).value, 3);
        EXPECT_THROW(x = 3, int);
        EXPECT_FALSE(x.valueless_by_exception());
    }
}
}  // namespace
}  // namespace rust
