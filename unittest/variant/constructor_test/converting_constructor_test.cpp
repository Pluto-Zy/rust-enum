#include <common.hpp>
#include <type_traits>

#ifdef USE_CXX20
    #include <memory>
    #include <string>
#endif

namespace rust {
namespace {
TEST(VariantTestConvertingConstructor, ConvertingConstructible) {
    // Test cases from MSVC STL.
    static_assert(std::is_constructible<variant<int>, int>::value);
    static_assert(!std::is_constructible<variant<int, int>, int>::value);
    static_assert(!std::is_constructible<variant<long, long long>, int>::value);
    static_assert(std::is_constructible<variant<float, long, double>, int>::value);

#ifdef USE_CXX20
    static_assert(!std::is_constructible<variant<char>, int>::value);

    static_assert(!std::is_constructible<variant<std::string, float>, int>::value);
    static_assert(!std::is_constructible<variant<std::string, double>, int>::value);
    static_assert(!std::is_constructible<variant<std::string, bool>, int>::value);

    static_assert(!std::is_constructible<variant<int, bool>, decltype("meow")>::value);
    static_assert(!std::is_constructible<variant<int, const bool>, decltype("meow")>::value);
    #ifndef USE_CXX20
    static_assert(
        !std::is_constructible<variant<int, const volatile bool>, decltype("meow")>::value
    );
    #endif

    static_assert(std::is_constructible<variant<bool>, std::true_type>::value);
    static_assert(!std::is_constructible<variant<bool>, std::unique_ptr<char>>::value);

    static_assert(!std::is_constructible<variant<bool>, decltype(nullptr)>::value);
#endif

    static_assert(!std::is_constructible<variant<int&>, int>::value);
    static_assert(std::is_constructible<variant<int&>, int&>::value);
    static_assert(!std::is_constructible<variant<int&>, float&>::value);
    static_assert(!std::is_constructible<variant<int&>, const int&>::value);
    static_assert(std::is_constructible<variant<int const&>, int&>::value);
    static_assert(std::is_constructible<variant<int const&>, int const&>::value);
    static_assert(!std::is_constructible<variant<int const&>, int>::value);
    static_assert(!std::is_constructible<variant<int, int&>, int&>::value);
    static_assert(std::is_constructible<variant<int&, const int&>, int&>::value);
    static_assert(std::is_constructible<variant<int&, const int&>, const int&>::value);
    static_assert(!std::is_constructible<variant<int, const int&>, int&>::value);
    static_assert(!std::is_constructible<variant<int&>, int*>::value);
    static_assert(!std::is_constructible<variant<int const&>, int*>::value);
    static_assert(!std::is_constructible<variant<int&>, int*&>::value);

    {
        struct base { };
        struct derived : base { };

        static_assert(std::is_constructible<variant<base&>, derived&>::value);
        static_assert(std::is_constructible<variant<base&, derived&>, derived&>::value);
    }
    {
        struct conv {
            operator int&();
        };

        static_assert(std::is_constructible<variant<int&>, conv&>::value);
        static_assert(std::is_constructible<variant<int&>, conv>::value);
        static_assert(std::is_constructible<variant<int&, conv&>, conv&>::value);
    }
}

TEST(VariantTestConvertingConstructor, Deleted) {
    static_assert(std::is_constructible<variant<long>, int>::value);
    static_assert(std::is_constructible<variant<monostate>, monostate>::value);
    static_assert(std::is_constructible<variant<int, monostate>, monostate>::value);
    static_assert(std::is_constructible<variant<const int>, int>::value);
    static_assert(std::is_constructible<variant<int, const double>, int>::value);
    static_assert(std::is_constructible<variant<int, const double>, double>::value);
    static_assert(!std::is_constructible<variant<long, long long>, int>::value);
    static_assert(!std::is_constructible<variant<std::string, std::string>, const char*>::value);
    static_assert(!std::is_constructible<variant<std::string, void*>, int>::value);

#ifdef USE_CXX20
    static_assert(!std::is_constructible<variant<std::string, float>, int>::value);
    static_assert(
        !std::is_constructible<variant<std::unique_ptr<int>, bool>, std::unique_ptr<char>>::value
    );
#endif  // USE_CXX20

    {
        using v = variant<int, float, int, double, float>;
        static_assert(!std::is_constructible<v, int>::value);
        static_assert(!std::is_constructible<v, float>::value);
        static_assert(std::is_constructible<v, double>::value);
    }

    {
        struct X { };
        struct Y {
            operator X();
        };
        static_assert(std::is_constructible<variant<X>, Y>::value);
    }
}

TEST(VariantTestConvertingConstructor, Noexcept) {
    static_assert(std::is_nothrow_constructible<variant<int>, int>::value);
    static_assert(std::is_nothrow_constructible<variant<long>, int>::value);
    static_assert(std::is_nothrow_constructible<variant<monostate>, monostate>::value);
    static_assert(std::is_nothrow_constructible<variant<int, monostate>, monostate>::value);
    static_assert(std::is_nothrow_constructible<variant<const int>, int>::value);
    static_assert(std::is_nothrow_constructible<variant<int, const double>, int>::value);

    static_assert(std::is_nothrow_constructible<variant<int&>, int&>::value);
    static_assert(std::is_nothrow_constructible<variant<int&, double&>, double&>::value);
    static_assert(std::is_nothrow_constructible<variant<std::string, int&>, int&>::value);

    // Test cases from MSVC STL.
    struct dummy { };
    {
        struct nothrow {
            nothrow(int) noexcept { }
            auto operator=(int) noexcept -> nothrow& {
                return *this;
            }
        };
        static_assert(std::is_nothrow_constructible<variant<dummy, nothrow>, int>::value);
    }
    {
        struct throws_constructor {
            throws_constructor(int) noexcept(false) { }
            auto operator=(int) noexcept -> throws_constructor& {
                return *this;
            }
        };
        static_assert(
            !std::is_nothrow_constructible<variant<dummy, throws_constructor>, int>::value
        );
    }
    {
        struct throws_assignment {
            throws_assignment(int) noexcept { }
            auto operator=(int) noexcept(false) -> throws_assignment& {
                return *this;
            }
        };
        static_assert(  //
            std::is_nothrow_constructible<variant<dummy, throws_assignment>, int>::value
        );
    }
    {
        struct throws_copy {
            throws_copy(const throws_copy&) noexcept(false);
        };
        static_assert(!std::is_nothrow_constructible<variant<throws_copy>, throws_copy>::value);
        static_assert(std::is_nothrow_constructible<variant<throws_copy&>, throws_copy&>::value);
    }
    {
        struct throws_conv {
            operator int&() noexcept(false);
        };

        static_assert(!std::is_nothrow_constructible<variant<int&>, throws_conv&>::value);
    }
}

TEST(VariantTestConvertingConstructor, BasicBehavior) {
    {
        using v = variant<int>;
        v x = 3;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 3);
    }
    {
        using v = variant<int, long>;
        v x = 3l;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
    }
    {
        using v = variant<int, long>;
        v x = 2;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 2);
    }
#ifdef USE_CXX20
    {
        using v = variant<unsigned, long>;
        v x = 3;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), 3l);
    }
    {
        using v = variant<std::string, bool>;
        v x = "bar";
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), "bar");
    }
#endif  // USE_CXX20
#ifndef USE_CXX20
    {
        using v = variant<bool volatile, std::unique_ptr<int>>;
        v x = nullptr;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x), nullptr);
    }
    {
        using v = variant<bool volatile const, int>;
        v x = true;
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(get<0>(x));
    }
#endif
    {
        struct convertible_from_rvalue {
            convertible_from_rvalue(int&&) { }
        };
        struct convertible_from_lvalue {
            convertible_from_lvalue(int&) { }
        };

        using v = variant<convertible_from_rvalue, convertible_from_lvalue>;
        {
            v x = 3;
            EXPECT_EQ(x.index(), 0);
        }
        {
            int data = 3;
            v x = data;
            EXPECT_EQ(x.index(), 1);
        }
    }
    {
        using v = variant<int&>;
        int data = 3;
        v x = data;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(&get<0>(x), &data);
    }
    {
        using v = variant<int&, double&>;
        double data = 3.0;
        v x = data;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(&get<1>(x), &data);
    }
    {
        using v = variant<int, long&>;
        long data = 3;
        v x = data;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(&get<1>(x), &data);
    }
    {
        struct conv {
            int const x = 3;
            operator const int&() {
                return x;
            }
        };
        using v = variant<const int&, double&>;
        conv data;
        v x = data;
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(&get<0>(x), &data.x);
    }
    {
        struct conv {
            int const x = 3;
            operator const int&() {
                return x;
            }
        };
        using v = variant<const int&, conv&>;
        conv data;
        v x = data;
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(&get<1>(x), &data);
    }
}
}  // namespace
}  // namespace rust
