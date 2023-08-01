#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestInPlaceIndexConstructor, Deleted) {
  static_assert(std::is_constructible<variant<int>, std::in_place_index_t<0>,
                                      int>::value);
  static_assert(
      std::is_constructible<variant<int>, std::in_place_index_t<0>>::value);
  static_assert(!std::is_constructible<variant<int>, std::in_place_index_t<0>,
                                       int, int>::value);
  static_assert(
      !std::is_convertible<std::in_place_index_t<0>, variant<int>>::value);
  static_assert(!std::is_constructible<variant<int>, std::in_place_index_t<1>,
                                       int>::value);
  static_assert(std::is_constructible<variant<int>, std::in_place_index_t<0>,
                                      double>::value);
  static_assert(std::is_constructible<variant<int, long, long long>,
                                      std::in_place_index_t<1>, int>::value);
  static_assert(std::is_constructible<variant<int, long, long long>,
                                      std::in_place_index_t<1>>::value);
  static_assert(!std::is_convertible<std::in_place_index_t<1>,
                                     variant<int, long, long long>>::value);
  static_assert(std::is_constructible<variant<int, long, long long>,
                                      std::in_place_index_t<2>, float>::value);
  static_assert(!std::is_convertible<std::in_place_index_t<3>,
                                     variant<int, long, long long>>::value);
  static_assert(!std::is_constructible<variant<int, long, long long>,
                                       std::in_place_index_t<3>, int>::value);
  static_assert(std::is_constructible<variant<int, std::string>,
                                      std::in_place_index_t<0>, int>::value);
  static_assert(
      std::is_constructible<variant<int, std::string>, std::in_place_index_t<1>,
                            const char*>::value);
  static_assert(std::is_constructible<variant<int, std::string>,
                                      std::in_place_index_t<1>>::value);
  static_assert(!std::is_convertible<std::in_place_index_t<1>,
                                     variant<int, std::string>>::value);
  static_assert(std::is_constructible<variant<int, int>,
                                      std::in_place_index_t<1>, int>::value);
  static_assert(std::is_constructible<variant<int, long, int*>,
                                      std::in_place_index_t<2>, int*>::value);
  static_assert(!std::is_constructible<variant<int, long, int*>,
                                       std::in_place_index_t<0>, int*>::value);
  static_assert(
      std::is_constructible<variant<int, long, int*>, std::in_place_index_t<2>,
                            std::nullptr_t>::value);

  static_assert(std::is_constructible<variant<int&>, std::in_place_index_t<0>,
                                      int&>::value);
  static_assert(!std::is_constructible<variant<int&>, std::in_place_index_t<0>,
                                       short&>::value);
  static_assert(!std::is_constructible<variant<int&>, std::in_place_index_t<0>,
                                       int>::value);
  static_assert(!std::is_constructible<variant<int&>, std::in_place_index_t<1>,
                                       int&>::value);
  static_assert(std::is_constructible<variant<int&, int&>,
                                      std::in_place_index_t<0>, int&>::value);
  static_assert(std::is_constructible<variant<int&, int&>,
                                      std::in_place_index_t<1>, int&>::value);
  static_assert(!std::is_constructible<variant<int&, int&>,
                                       std::in_place_index_t<2>, int&>::value);
  static_assert(std::is_constructible<variant<long&, int&>,
                                      std::in_place_index_t<0>, long&>::value);
  static_assert(!std::is_constructible<variant<long&, int&>,
                                       std::in_place_index_t<1>, long&>::value);
  static_assert(!std::is_constructible<variant<long&, int&>,
                                       std::in_place_index_t<2>, long&>::value);
  static_assert(std::is_constructible<variant<int, int&>,
                                      std::in_place_index_t<0>, int&>::value);
  static_assert(std::is_constructible<variant<int, int&>,
                                      std::in_place_index_t<1>, int&>::value);
  static_assert(!std::is_constructible<variant<int, int&>,
                                       std::in_place_index_t<2>, int&>::value);
  static_assert(std::is_constructible<variant<long&, int>,
                                      std::in_place_index_t<0>, long&>::value);
  static_assert(std::is_constructible<variant<long&, int>,
                                      std::in_place_index_t<1>, long&>::value);
  static_assert(!std::is_constructible<variant<long&, int>,
                                       std::in_place_index_t<2>, long&>::value);
  static_assert(
      !std::is_constructible<variant<int&, std::string&>,
                             std::in_place_index_t<1>, const char*>::value);
  static_assert(!std::is_constructible<variant<int&, std::string&>,
                                       std::in_place_index_t<1>,
                                       const char(&)[3]>::value);
  static_assert(
      !std::is_constructible<variant<int&, const int&>,
                             std::in_place_index_t<0>, const int&>::value);
  static_assert(
      std::is_constructible<variant<int&, const int&>, std::in_place_index_t<1>,
                            const int&>::value);
  static_assert(
      !std::is_constructible<variant<int&, const int&>,
                             std::in_place_index_t<2>, const int&>::value);

  {
    struct more_arguments {
      more_arguments(int, double, float);
    };
    static_assert(
        std::is_constructible<variant<more_arguments>, std::in_place_index_t<0>,
                              int, double, float>::value);
    static_assert(
        std::is_constructible<variant<more_arguments>, std::in_place_index_t<0>,
                              int, double, double>::value);
    static_assert(
        std::is_constructible<variant<more_arguments>, std::in_place_index_t<0>,
                              long, double, float>::value);
    static_assert(
        !std::is_constructible<variant<more_arguments>,
                               std::in_place_index_t<0>, int, double>::value);
    static_assert(!std::is_constructible<variant<more_arguments>,
                                         std::in_place_index_t<0>, int, double,
                                         float, int>::value);
    static_assert(std::is_constructible<variant<int, more_arguments>,
                                        std::in_place_index_t<1>, int, double,
                                        float>::value);
    static_assert(!std::is_constructible<variant<int, more_arguments>,
                                         std::in_place_index_t<1>, int, int,
                                         double, float>::value);
    static_assert(!std::is_constructible<variant<more_arguments&>,
                                         std::in_place_index_t<0>, int, double,
                                         double>::value);
  }
}

TEST(VariantTestInPlaceIndexConstructor, Noexcept) {
  static_assert(
      std::is_nothrow_constructible<variant<int>, std::in_place_index_t<0>,
                                    int>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int>, std::in_place_index_t<0>,
                                    double>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, long, long long>,
                                    std::in_place_index_t<1>, int>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, long, long long>,
                                    std::in_place_index_t<2>, float>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, std::string>,
                                    std::in_place_index_t<0>, int>::value);
  static_assert(!std::is_nothrow_constructible<variant<int, std::string>,
                                               std::in_place_index_t<1>,
                                               const char*>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, int>, std::in_place_index_t<1>,
                                    int>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, long, int*>,
                                    std::in_place_index_t<2>, int*>::value);
  static_assert(std::is_nothrow_constructible<variant<int, long, int*>,
                                              std::in_place_index_t<2>,
                                              std::nullptr_t>::value);

  static_assert(
      std::is_nothrow_constructible<variant<int&>, std::in_place_index_t<0>,
                                    int&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int&, int&>,
                                    std::in_place_index_t<0>, int&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int&, int&>,
                                    std::in_place_index_t<1>, int&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<long&, int&>,
                                    std::in_place_index_t<0>, long&>::value);
  static_assert(
      !std::is_nothrow_constructible<variant<long&, int&>,
                                     std::in_place_index_t<1>, long&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, int&>,
                                    std::in_place_index_t<0>, int&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, int&>,
                                    std::in_place_index_t<1>, int&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<long&, int>,
                                    std::in_place_index_t<0>, long&>::value);
  static_assert(
      std::is_nothrow_constructible<variant<long&, int>,
                                    std::in_place_index_t<1>, long&>::value);
  static_assert(std::is_nothrow_constructible<variant<int&, std::string const&>,
                                              std::in_place_index_t<1>,
                                              std::string&>::value);
  static_assert(std::is_nothrow_constructible<variant<int&, const int&>,
                                              std::in_place_index_t<1>,
                                              const int&>::value);

  {
    struct may_throw_constructible {
      may_throw_constructible(int);
      may_throw_constructible(double) noexcept;
    };
    static_assert(
        !std::is_nothrow_constructible<variant<may_throw_constructible>,
                                       std::in_place_index_t<0>, int>::value);
    static_assert(std::is_constructible<variant<may_throw_constructible>,
                                        std::in_place_index_t<0>, int>::value);
    static_assert(
        std::is_nothrow_constructible<variant<may_throw_constructible>,
                                      std::in_place_index_t<0>, double>::value);
    static_assert(
        !std::is_nothrow_constructible<variant<may_throw_constructible, int>,
                                       std::in_place_index_t<0>, int>::value);
    static_assert(std::is_constructible<variant<may_throw_constructible, int>,
                                        std::in_place_index_t<0>, int>::value);
    static_assert(
        std::is_nothrow_constructible<variant<may_throw_constructible, int>,
                                      std::in_place_index_t<0>, double>::value);
    static_assert(
        !std::is_nothrow_constructible<variant<int, may_throw_constructible>,
                                       std::in_place_index_t<1>, int>::value);
    static_assert(std::is_constructible<variant<int, may_throw_constructible>,
                                        std::in_place_index_t<1>, int>::value);
    static_assert(
        std::is_nothrow_constructible<variant<int, may_throw_constructible>,
                                      std::in_place_index_t<1>, double>::value);
  }
  {
    struct more_arguments {
      more_arguments(int, double, float) noexcept;
      more_arguments(float, double, int);
    };
    static_assert(std::is_nothrow_constructible<variant<more_arguments>,
                                                std::in_place_index_t<0>, int,
                                                double, float>::value);
    static_assert(std::is_nothrow_constructible<variant<more_arguments>,
                                                std::in_place_index_t<0>, int,
                                                double, double>::value);
    static_assert(!std::is_nothrow_constructible<variant<more_arguments>,
                                                 std::in_place_index_t<0>,
                                                 float, double, int>::value);
    static_assert(std::is_nothrow_constructible<variant<int, more_arguments>,
                                                std::in_place_index_t<1>, int,
                                                double, float>::value);
    static_assert(!std::is_nothrow_constructible<variant<int, more_arguments>,
                                                 std::in_place_index_t<1>,
                                                 float, double, int>::value);
  }
}

TEST(VariantTestInPlaceIndexConstructor, BasicBehavior) {
  {
    using v = variant<int>;
    v x(std::in_place_index<0>, 3);
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(x));
    EXPECT_EQ(get<0>(x), 3);
    EXPECT_EQ(get<int>(x), 3);
  }
  {
    using v = variant<int, long, long>;
    v x(std::in_place_index<1>, 3);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x), 3l);
    EXPECT_THROW(get<2>(x), bad_variant_access);
  }
  {
    using v = variant<int, long, long>;
    v x(std::in_place_index<1>);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x), 0l);
    EXPECT_THROW(get<2>(x), bad_variant_access);
  }
  {
    using v = variant<int, const int, long>;
    v x(std::in_place_index<1>, 3);
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<const int>(x));
    EXPECT_EQ(get<1>(x), 3);
    EXPECT_EQ(get<const int>(x), 3);
  }
  {
    using v = variant<const int, volatile int, int>;
    v x(std::in_place_index<0>, 3);
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<const int>(x));
    EXPECT_EQ(get<0>(x), 3);
    EXPECT_EQ(get<const int>(x), 3);
  }
  {
    using v = variant<const int, volatile int, int>;
    v x(std::in_place_index<1>, 3);
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<volatile int>(x));
    EXPECT_EQ(get<1>(x), 3);
    EXPECT_EQ(get<volatile int>(x), 3);
  }
  {
    using v = variant<const int, volatile int, int>;
    v x(std::in_place_index<2>, 3);
    EXPECT_EQ(x.index(), 2);
    EXPECT_TRUE(holds_alternative<int>(x));
    EXPECT_EQ(get<2>(x), 3);
    EXPECT_EQ(get<int>(x), 3);
  }
  {
    using v = variant<int, counter>;
    v x(std::in_place_index<1>);
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
    v x(std::in_place_index<1>, 3, 4.0f);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).i_val, 3);
    EXPECT_EQ(get<1>(x).f_val, 4.0f);
  }
  {
    struct throw_constructible {
      throw_constructible(int) { throw 1; }
    };

    using v = variant<int, throw_constructible>;
    EXPECT_THROW(v x(std::in_place_index<1>, 3), int);
    v x(std::in_place_index<0>, 5);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 5);
  }
  {
    using v = variant<int&>;
    int data = 3;
    v x(std::in_place_index<0>, data);
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<int&>(x));
    EXPECT_EQ(&get<int&>(x), &data);
  }
  {
    using v = variant<int&, int&>;
    int data = 3;
    v x(std::in_place_index<1>, data);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(&get<1>(x), &data);
  }
  {
    struct conv {
      int data = 3;
      operator int&() { return data; }
    };

    using v = variant<int&, conv&>;
    conv c;
    v x1(std::in_place_index<0>, c);
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(&get<0>(x1), &c.data);
    v x2(std::in_place_index<1>, c);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_EQ(&get<1>(x2), &c);
  }
}

TEST(VariantTestInPlaceIndexConstructor, Constexpr) {
  {
    using v = variant<int>;
    constexpr v x(std::in_place_index<0>, 3);
    static_assert(x.index() == 0);
    static_assert(holds_alternative<int>(x));
    static_assert(get<0>(x) == 3);
    static_assert(get<int>(x) == 3);
  }
  {
    using v = variant<int, long, long>;
    constexpr v x(std::in_place_index<1>, 3);
    static_assert(x.index() == 1);
    static_assert(get<1>(x) == 3l);
  }
  {
    using v = variant<int, long, long>;
    constexpr v x(std::in_place_index<1>);
    static_assert(x.index() == 1);
    static_assert(get<1>(x) == 0l);
  }
  {
    using v = variant<int, const int, long>;
    constexpr v x(std::in_place_index<1>, 3);
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

    using v = variant<int, constexpr_ctor, constexpr_ctor>;
    constexpr v x(std::in_place_index<1>, 3);
    static_assert(x.index() == 1);
    static_assert(get<1>(x).val == 3);
  }
  {
    using v = variant<int&>;
    static int data = 3;
    constexpr v x(std::in_place_index<0>, data);
    static_assert(x.index() == 0);
    static_assert(holds_alternative<int&>(x));
    static_assert(&get<int&>(x) == &data);
  }
  {
    using v = variant<int&, int&>;
    static int data = 3;
    constexpr v x(std::in_place_index<1>, data);
    static_assert(x.index() == 1);
    static_assert(&get<1>(x) == &data);
  }
}
}  // namespace
}  // namespace rust