#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestInPlaceTypeConstructor, Deleted) {
    static_assert(std::is_constructible<variant<int>, std::in_place_type_t<int>, int>::value);
    static_assert(std::is_constructible<variant<int>, std::in_place_type_t<int>>::value);
    static_assert(!std::is_constructible<variant<int>, std::in_place_type_t<int>, int, int>::value
    );
    static_assert(!std::is_constructible<variant<int>, std::in_place_type_t<double>, int>::value);
    static_assert(!std::is_convertible<std::in_place_type_t<int>, variant<int>>::value);
    static_assert(std::is_constructible<variant<int, long>, std::in_place_type_t<long>, int>::value
    );
    static_assert(
        !std::is_constructible<variant<int, long>, std::in_place_type_t<float>, int>::value
    );
    static_assert(!std::is_convertible<std::in_place_type_t<long>, variant<int, long>>::value);
    static_assert(
        std::is_constructible<variant<int, long, long long>, std::in_place_type_t<long>, int>::
            value
    );
    static_assert(
        std::is_constructible<variant<int, long, int*>, std::in_place_type_t<int*>, int*>::value
    );
    static_assert(
        !std::is_constructible<variant<int, long, int*>, std::in_place_type_t<void*>, void*>::value
    );
    static_assert(!std::is_convertible<std::in_place_type_t<int*>, variant<int, long, int*>>::value
    );
    static_assert(
        !std::is_constructible<variant<int, long, int>, std::in_place_type_t<int>, int>::value
    );
    static_assert(
        std::is_constructible<variant<int, long, int>, std::in_place_type_t<long>, int>::value
    );
    static_assert(
        !std::is_constructible<variant<int, long, int*>, std::in_place_type_t<int>, int*>::value
    );
    static_assert(
        !std::is_constructible<variant<int, long, int*>, std::in_place_type_t<short>, int>::value
    );
    static_assert(std::is_constructible<
                  variant<int, long, int*>,
                  std::in_place_type_t<int*>,
                  std::nullptr_t>::value);

    static_assert(std::is_constructible<variant<int&>, std::in_place_type_t<int&>, int&>::value);
    static_assert(!std::is_constructible<variant<int&>, std::in_place_type_t<int&>, short&>::value
    );
    static_assert(!std::is_constructible<variant<int&>, std::in_place_type_t<int&>, int>::value);
    static_assert(!std::is_constructible<variant<int&>, std::in_place_type_t<int>, int&>::value);
    static_assert(
        !std::is_constructible<variant<int&, int&>, std::in_place_type_t<int&>, int&>::value
    );
    static_assert(
        std::is_constructible<variant<long&, int&>, std::in_place_type_t<long&>, long&>::value
    );
    static_assert(
        !std::is_constructible<variant<long&, int&>, std::in_place_type_t<int&>, long&>::value
    );
    static_assert(
        !std::is_constructible<variant<long&, int&>, std::in_place_type_t<long&>, int&>::value
    );
    static_assert(std::is_constructible<variant<int, int&>, std::in_place_type_t<int>, int&>::value
    );
    static_assert(
        std::is_constructible<variant<int, int&>, std::in_place_type_t<int&>, int&>::value
    );
    static_assert(
        std::is_constructible<variant<long&, int>, std::in_place_type_t<long&>, long&>::value
    );
    static_assert(
        std::is_constructible<variant<long&, int>, std::in_place_type_t<int>, long&>::value
    );
    static_assert(!std::is_constructible<
                  variant<int&, std::string&>,
                  std::in_place_type_t<std::string&>,
                  const char*>::value);
    static_assert(!std::is_constructible<
                  variant<int&, std::string&>,
                  std::in_place_type_t<std::string&>,
                  const char(&)[3]>::value);
    static_assert(
        !std::is_constructible<variant<int&, const int&>, std::in_place_type_t<int&>, const int&>::
            value
    );
    static_assert(std::is_constructible<
                  variant<int&, const int&>,
                  std::in_place_type_t<const int&>,
                  const int&>::value);

    {
        struct more_arguments {
            more_arguments(int, double, float);
        };
        static_assert(std::is_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      float>::value);
        static_assert(std::is_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      double>::value);
        static_assert(std::is_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      long,
                      double,
                      float>::value);
        static_assert(!std::is_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double>::value);
        static_assert(!std::is_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      float,
                      int>::value);
        static_assert(std::is_constructible<
                      variant<int, more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      float>::value);
        static_assert(!std::is_constructible<
                      variant<int, more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      int,
                      double,
                      float>::value);
        static_assert(!std::is_constructible<
                      variant<more_arguments&>,
                      std::in_place_type_t<more_arguments&>,
                      int,
                      double,
                      double>::value);
    }
}

TEST(VariantTestInPlaceTypeConstructor, Noexcept) {
    static_assert(
        std::is_nothrow_constructible<variant<int>, std::in_place_type_t<int>, int>::value
    );
    static_assert(
        std::is_nothrow_constructible<variant<int>, std::in_place_type_t<int>, double>::value
    );
    static_assert(std::is_nothrow_constructible<
                  variant<int, long, long long>,
                  std::in_place_type_t<long>,
                  int>::value);
    static_assert(std::is_nothrow_constructible<
                  variant<int, long, long long>,
                  std::in_place_type_t<long long>,
                  float>::value);
    static_assert(
        std::is_nothrow_constructible<variant<int, std::string>, std::in_place_type_t<int>, int>::
            value
    );
    static_assert(!std::is_nothrow_constructible<
                  variant<int, std::string>,
                  std::in_place_type_t<std::string>,
                  const char*>::value);
    static_assert(
        std::is_nothrow_constructible<variant<int, long, int*>, std::in_place_type_t<int*>, int*>::
            value
    );
    static_assert(std::is_nothrow_constructible<
                  variant<int, long, int*>,
                  std::in_place_type_t<int*>,
                  std::nullptr_t>::value);

    static_assert(
        std::is_nothrow_constructible<variant<int&>, std::in_place_type_t<int&>, int&>::value
    );
    static_assert(
        !std::is_nothrow_constructible<variant<int&>, std::in_place_type_t<int>, int&>::value
    );
    static_assert(
        !std::is_nothrow_constructible<variant<int&, int&>, std::in_place_type_t<int&>, int&>::
            value
    );
    static_assert(
        std::is_nothrow_constructible<variant<long&, int&>, std::in_place_type_t<long&>, long&>::
            value
    );
    static_assert(
        !std::is_nothrow_constructible<variant<long&, int&>, std::in_place_type_t<int&>, long&>::
            value
    );
    static_assert(
        std::is_nothrow_constructible<variant<long&, int&>, std::in_place_type_t<int&>, int&>::
            value
    );
    static_assert(
        std::is_nothrow_constructible<variant<int, int&>, std::in_place_type_t<int>, int&>::value
    );
    static_assert(
        std::is_nothrow_constructible<variant<int, int&>, std::in_place_type_t<int&>, int&>::value
    );
    static_assert(
        std::is_nothrow_constructible<variant<long&, int>, std::in_place_type_t<long&>, long&>::
            value
    );
    static_assert(
        std::is_nothrow_constructible<variant<long&, int>, std::in_place_type_t<int>, long&>::value
    );
    static_assert(std::is_nothrow_constructible<
                  variant<int&, std::string&>,
                  std::in_place_type_t<std::string&>,
                  std::string&>::value);
    static_assert(std::is_nothrow_constructible<
                  variant<int&, const int&>,
                  std::in_place_type_t<const int&>,
                  const int&>::value);

    {
        struct may_throw_constructible {
            may_throw_constructible(int);
            may_throw_constructible(double) noexcept;
        };
        static_assert(!std::is_nothrow_constructible<
                      variant<may_throw_constructible>,
                      std::in_place_type_t<may_throw_constructible>,
                      int>::value);
        static_assert(std::is_constructible<
                      variant<may_throw_constructible>,
                      std::in_place_type_t<may_throw_constructible>,
                      int>::value);
        static_assert(std::is_nothrow_constructible<
                      variant<may_throw_constructible>,
                      std::in_place_type_t<may_throw_constructible>,
                      double>::value);
        static_assert(!std::is_nothrow_constructible<
                      variant<may_throw_constructible, int>,
                      std::in_place_type_t<may_throw_constructible>,
                      int>::value);
        static_assert(std::is_constructible<
                      variant<may_throw_constructible, int>,
                      std::in_place_type_t<may_throw_constructible>,
                      int>::value);
        static_assert(std::is_nothrow_constructible<
                      variant<may_throw_constructible, int>,
                      std::in_place_type_t<may_throw_constructible>,
                      double>::value);
        static_assert(!std::is_nothrow_constructible<
                      variant<int, may_throw_constructible>,
                      std::in_place_type_t<may_throw_constructible>,
                      int>::value);
        static_assert(std::is_constructible<
                      variant<int, may_throw_constructible>,
                      std::in_place_type_t<may_throw_constructible>,
                      int>::value);
        static_assert(std::is_nothrow_constructible<
                      variant<int, may_throw_constructible>,
                      std::in_place_type_t<may_throw_constructible>,
                      double>::value);
    }
    {
        struct more_arguments {
            more_arguments(int, double, float) noexcept;
            more_arguments(float, double, int);
        };
        static_assert(std::is_nothrow_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      float>::value);
        static_assert(std::is_nothrow_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      double>::value);
        static_assert(!std::is_nothrow_constructible<
                      variant<more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      float,
                      double,
                      int>::value);
        static_assert(std::is_nothrow_constructible<
                      variant<int, more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      int,
                      double,
                      float>::value);
        static_assert(!std::is_nothrow_constructible<
                      variant<int, more_arguments>,
                      std::in_place_type_t<more_arguments>,
                      float,
                      double,
                      int>::value);
    }
}

TEST(VariantTestInPlaceTypeConstructor, BasicBehavior) {
    {
        using v = variant<int>;
        v x(std::in_place_type<int>, 3);
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<int>(x));
        EXPECT_EQ(get<0>(x), 3);
        EXPECT_EQ(get<int>(x), 3);
    }
    {
        using v = variant<int, long>;
        v x(std::in_place_type<long>, 3);
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x));
        EXPECT_EQ(get<1>(x), 3l);
        EXPECT_EQ(get<long>(x), 3);
    }
    {
        using v = variant<int, long>;
        v x(std::in_place_type<long>);
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x));
        EXPECT_EQ(get<1>(x), 0l);
        EXPECT_EQ(get<long>(x), 0);
    }
    {
        using v = variant<int, const int, long>;
        v x(std::in_place_type<const int>, 3);
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<const int>(x));
        EXPECT_EQ(get<1>(x), 3);
        EXPECT_EQ(get<const int>(x), 3);
    }
    {
        using v = variant<const int, volatile int, int>;
        v x(std::in_place_type<const int>, 3);
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<const int>(x));
        EXPECT_EQ(get<0>(x), 3);
        EXPECT_EQ(get<const int>(x), 3);
    }
    {
        using v = variant<const int, volatile int, int>;
        v x(std::in_place_type<volatile int>, 3);
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<volatile int>(x));
        EXPECT_EQ(get<1>(x), 3);
        EXPECT_EQ(get<volatile int>(x), 3);
    }
    {
        using v = variant<const int, volatile int, int>;
        v x(std::in_place_type<int>, 3);
        EXPECT_EQ(x.index(), 2);
        EXPECT_TRUE(holds_alternative<int>(x));
        EXPECT_EQ(get<2>(x), 3);
        EXPECT_EQ(get<int>(x), 3);
    }
    {
        using v = variant<int, counter>;
        v x(std::in_place_type<counter>);
        EXPECT_EQ(x.index(), 1);
        EXPECT_TRUE(holds_alternative<counter>(x));
        EXPECT_EQ(counter::alive_count, 1);
        counter::reset();
    }
    {
        struct more_arguments {
            int i_val;
            float f_val;
            more_arguments(int i, float f) : i_val(i), f_val(f) { }
        };

        using v = variant<int, more_arguments>;
        v x(std::in_place_type<more_arguments>, 3, 4.0f);
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(get<1>(x).i_val, 3);
        EXPECT_EQ(get<1>(x).f_val, 4.0f);
    }
    {
        struct throw_constructible {
            throw_constructible(int) {
                throw 1;
            }
        };

        using v = variant<int, throw_constructible>;
        EXPECT_THROW(v x(std::in_place_type<throw_constructible>, 3), int);
        v x(std::in_place_index<0>, 5);
        EXPECT_EQ(x.index(), 0);
        EXPECT_EQ(get<0>(x), 5);
    }
    {
        using v = variant<int&>;
        int data = 3;
        v x(std::in_place_type<int&>, data);
        EXPECT_EQ(x.index(), 0);
        EXPECT_TRUE(holds_alternative<int&>(x));
        EXPECT_EQ(&get<int&>(x), &data);
    }
    {
        using v = variant<int&, long&>;
        long data = 3;
        v x(std::in_place_type<long&>, data);
        EXPECT_EQ(x.index(), 1);
        EXPECT_EQ(&get<1>(x), &data);
    }
    {
        struct conv {
            int data = 3;
            operator int&() {
                return data;
            }
        };

        using v = variant<int&, conv&>;
        conv c;
        v x1(std::in_place_type<int&>, c);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(&get<0>(x1), &c.data);
        v x2(std::in_place_type<conv&>, c);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(&get<1>(x2), &c);
    }
}

TEST(VariantTestInPlaceTypeConstructor, Constexpr) {
    {
        using v = variant<int>;
        constexpr v x(std::in_place_type<int>, 3);
        static_assert(x.index() == 0);
        static_assert(holds_alternative<int>(x));
        static_assert(get<0>(x) == 3);
        static_assert(get<int>(x) == 3);
    }
    {
        using v = variant<int, long>;
        constexpr v x(std::in_place_type<long>, 3);
        static_assert(x.index() == 1);
        static_assert(holds_alternative<long>(x));
        static_assert(get<1>(x) == 3l);
        static_assert(get<long>(x) == 3l);
    }
    {
        using v = variant<int, long>;
        constexpr v x(std::in_place_type<long>);
        static_assert(x.index() == 1);
        static_assert(holds_alternative<long>(x));
        static_assert(get<1>(x) == 0l);
        static_assert(get<long>(x) == 0l);
    }
    {
        using v = variant<int, const int, long>;
        constexpr v x(std::in_place_type<const int>, 3);
        static_assert(x.index() == 1);
        static_assert(holds_alternative<const int>(x));
        static_assert(get<1>(x) == 3);
        static_assert(get<const int>(x) == 3);
    }
    {
        struct constexpr_ctor {
            int val;
            constexpr constexpr_ctor(int val) : val(val) { }
        };

        using v = variant<int, constexpr_ctor>;
        constexpr v x(std::in_place_type<constexpr_ctor>, 3);
        static_assert(x.index() == 1);
        static_assert(holds_alternative<constexpr_ctor>(x));
        static_assert(get<1>(x).val == 3);
        static_assert(get<constexpr_ctor>(x).val == 3);
    }
    {
        using v = variant<int&>;
        static int data = 3;
        constexpr v x(std::in_place_type<int&>, data);
        static_assert(x.index() == 0);
        static_assert(holds_alternative<int&>(x));
        static_assert(&get<int&>(x) == &data);
    }
    {
        using v = variant<long&, int&>;
        static int data = 3;
        constexpr v x(std::in_place_type<int&>, data);
        static_assert(x.index() == 1);
        static_assert(&get<1>(x) == &data);
    }
}
}  // namespace
}  // namespace rust