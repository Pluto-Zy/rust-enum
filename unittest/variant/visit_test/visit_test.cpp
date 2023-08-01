#include "visit_test_helper.hpp"
#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
template <class T>
struct incomplete_holder { };

TEST(VariantTestVisit, RobustAgainstADL) {
  struct incomplete;

  variant<incomplete_holder<incomplete>*, int> x = nullptr;
  rust::visit([](auto) {}, x);
  rust::visit([](auto) -> incomplete_holder<incomplete>* { return nullptr; },
              x);
  rust::visit<void>([](auto) {}, x);
  rust::visit<void*>(
      [](auto) -> incomplete_holder<incomplete>* { return nullptr; }, x);
}

TEST(VariantTestVisit, ForwardCallOperator) {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    rust::visit(obj);
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_LVALUE));
    rust::visit(cobj);
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_LVALUE));
    rust::visit(std::move(obj));
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_RVALUE));
    rust::visit(std::move(cobj));
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int>;
    v x(3);
    rust::visit(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_LVALUE));
    rust::visit(cobj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_LVALUE));
    rust::visit(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_RVALUE));
    rust::visit(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    rust::visit(obj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_LVALUE));
    rust::visit(cobj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_LVALUE));
    rust::visit(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_RVALUE));
    rust::visit(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_RVALUE));
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    rust::visit(obj, x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_LVALUE)));
    rust::visit(cobj, x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_LVALUE)));
    rust::visit(std::move(obj), x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_RVALUE)));
    rust::visit(std::move(cobj), x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(4), x4(1.1);
    rust::visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    rust::visit(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_LVALUE)));
    rust::visit(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    rust::visit(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    rust::visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    rust::visit(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_LVALUE)));
    rust::visit(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    rust::visit(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int&>;
    int data = 3;
    v x(data);
    rust::visit(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_LVALUE));
    rust::visit(cobj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_LVALUE));
    rust::visit(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_RVALUE));
    rust::visit(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int&, const long&, double&>;
    long data = 3;
    v x(data);
    rust::visit(obj, x);
    EXPECT_TRUE(fn::check_call<const long&>(CT_NON_CONST | CT_LVALUE));
    rust::visit(cobj, x);
    EXPECT_TRUE(fn::check_call<const long&>(CT_CONST | CT_LVALUE));
    rust::visit(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<const long&>(CT_NON_CONST | CT_RVALUE));
    rust::visit(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<const long&>(CT_CONST | CT_RVALUE));
  }
  {
    using v1 = variant<int&, double&>;
    using v2 = variant<float&, std::string&>;
    double data1 = 3.0;
    float data2 = 4.0f;
    v1 x1(data1);
    v2 x2(data2);
    rust::visit(obj, x1, x2);
    EXPECT_TRUE((fn::check_call<double&, float&>(CT_NON_CONST | CT_LVALUE)));
    rust::visit(cobj, x1, x2);
    EXPECT_TRUE((fn::check_call<double&, float&>(CT_CONST | CT_LVALUE)));
    rust::visit(std::move(obj), x1, x2);
    EXPECT_TRUE((fn::check_call<double&, float&>(CT_NON_CONST | CT_RVALUE)));
    rust::visit(std::move(cobj), x1, x2);
    EXPECT_TRUE((fn::check_call<double&, float&>(CT_CONST | CT_RVALUE)));
  }
}

TEST(VariantTestVisit, ForwardArgument) {
  using fn = forwarding_call_object;
  fn obj {};
  const auto expected = CT_LVALUE | CT_NON_CONST;
  {
    using v = variant<int>;
    v x(3);
    const v& cx = x;
    rust::visit(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(expected));
    rust::visit(obj, cx);
    EXPECT_TRUE(fn::check_call<const int&>(expected));
    rust::visit(obj, std::move(x));
    EXPECT_TRUE(fn::check_call<int&&>(expected));
    rust::visit(obj, std::move(cx));
    EXPECT_TRUE(fn::check_call<const int&&>(expected));
  }
  {
    using v = variant<int, std::string, long>;
    v x1(3), x2("hello"), x3(3l);
    rust::visit(obj, x1, x2, x3);
    EXPECT_TRUE((fn::check_call<int&, std::string&, long&>(expected)));
    rust::visit(obj, std::as_const(x1), std::as_const(x2), std::move(x3));
    EXPECT_TRUE(
        (fn::check_call<const int&, const std::string&, long&&>(expected)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    rust::visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(expected)));
    rust::visit(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
                std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int&&, double&&>(
            expected)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    rust::visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&, int*&, double&>(expected)));
    rust::visit(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
                std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int*&&, double&&>(
            expected)));
  }
  {
    using v = variant<int&>;
    int data = 3;
    v x(data);
    const v& cx(x);
    rust::visit(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(expected));
    rust::visit(obj, cx);
    EXPECT_TRUE(fn::check_call<int&>(expected));
    rust::visit(obj, std::move(x));
    EXPECT_TRUE(fn::check_call<int&>(expected));
    rust::visit(obj, std::move(cx));
    EXPECT_TRUE(fn::check_call<int&>(expected));
  }
  {
    using v = variant<int&, const long&, float>;
    int data1 = 3;
    long data2 = 4;
    float data3 = 5.f;
    v x1(data1), x2(data2), x3(data3);
    rust::visit(obj, x1, x2, x3);
    EXPECT_TRUE((fn::check_call<int&, const long&, float&>(expected)));
    rust::visit(obj, std::as_const(x1), std::as_const(x2), std::as_const(x3));
    EXPECT_TRUE((fn::check_call<int&, const long&, const float&>(expected)));
    rust::visit(obj, std::move(x1), std::move(x2), std::move(x3));
    EXPECT_TRUE((fn::check_call<int&, const long&, float&&>(expected)));
    rust::visit(obj, static_cast<v const&&>(x1), static_cast<v const&&>(x2),
                static_cast<v const&&>(x3));
    EXPECT_TRUE((fn::check_call<int&, const long&, const float&&>(expected)));
  }
}

TEST(VariantTestVisit, ReturnType) {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    static_assert(std::is_same<decltype(rust::visit(obj)), fn&>::value);
    static_assert(std::is_same<decltype(rust::visit(cobj)), const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj))), fn&&>::value);
    static_assert(std::is_same<decltype(rust::visit(std::move(cobj))),
                               const fn&&>::value);
  }
  {
    using v = variant<int>;
    v x(3);
    static_assert(std::is_same<decltype(rust::visit(obj, x)), fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(cobj, x)), const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj), x)), fn&&>::value);
    static_assert(std::is_same<decltype(rust::visit(std::move(cobj), x)),
                               const fn&&>::value);
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    static_assert(std::is_same<decltype(rust::visit(obj, x)), fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(cobj, x)), const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj), x)), fn&&>::value);
    static_assert(std::is_same<decltype(rust::visit(std::move(cobj), x)),
                               const fn&&>::value);
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    static_assert(std::is_same<decltype(rust::visit(obj, x1, x2)), fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(cobj, x1, x2)), const fn&>::value);
    static_assert(std::is_same<decltype(rust::visit(std::move(obj), x1, x2)),
                               fn&&>::value);
    static_assert(std::is_same<decltype(rust::visit(std::move(cobj), x1, x2)),
                               const fn&&>::value);
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    static_assert(
        std::is_same<decltype(rust::visit(obj, x1, x2, x3, x4)), fn&>::value);
    static_assert(std::is_same<decltype(rust::visit(cobj, x1, x2, x3, x4)),
                               const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj), x1, x2, x3, x4)),
                     fn&&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(cobj), x1, x2, x3, x4)),
                     const fn&&>::value);
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    static_assert(
        std::is_same<decltype(rust::visit(obj, x1, x2, x3, x4)), fn&>::value);
    static_assert(std::is_same<decltype(rust::visit(cobj, x1, x2, x3, x4)),
                               const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj), x1, x2, x3, x4)),
                     fn&&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(cobj), x1, x2, x3, x4)),
                     const fn&&>::value);
  }
  {
    using v = variant<int&>;
    int data = 3;
    v x(data);
    static_assert(std::is_same<decltype(rust::visit(obj, x)), fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(cobj, x)), const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj), x)), fn&&>::value);
    static_assert(std::is_same<decltype(rust::visit(std::move(cobj), x)),
                               const fn&&>::value);
  }
  {
    using v = variant<int&, const long&, float>;
    int data1 = 3;
    long data2 = 4;
    float data3 = 5.f;
    v x1(data1), x2(data2), x3(data3);
    static_assert(
        std::is_same<decltype(rust::visit(obj, x1, x2, x3)), fn&>::value);
    static_assert(std::is_same<decltype(rust::visit(cobj, x1, x2, x3)),
                               const fn&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(obj), x1, x2, x3)),
                     fn&&>::value);
    static_assert(
        std::is_same<decltype(rust::visit(std::move(cobj), x1, x2, x3)),
                     const fn&&>::value);
  }
}

TEST(VariantTestVisit, Constexpr) {
  constexpr return_first first {};
  constexpr return_first_reference first_ref {};
  constexpr return_arity arity {};

  {
    using v = variant<int>;
    constexpr v x(3);
    static_assert(rust::visit(first, x) == 3);
  }
  {
    using v = variant<short, long, char>;
    constexpr v x(3l);
    static_assert(rust::visit(first, x) == 3l);
  }
  {
    using v1 = variant<int>;
    using v2 = variant<int, char*, long long>;
    using v3 = variant<bool, int, int>;
    constexpr v1 x1;
    constexpr v2 x2(nullptr);
    constexpr v3 x3;
    static_assert(rust::visit(arity, x1, x2, x3) == 3);
  }
  {
    using v = variant<int, long, double, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(rust::visit(arity, x1, x2, x3, x4) == 4);
  }
  {
    using v = variant<int, long, double, long long, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(rust::visit(arity, x1, x2, x3, x4) == 4);
  }
  {
    using v = variant<int&>;
    static int data = 3;
    constexpr v x(data);
    static_assert(rust::visit(first_ref, x) == &data);
  }
  {
    using v = variant<int&, const long&, float>;
    static int data1 = 3;
    static long data2 = 4;
    constexpr float data3 = 5.f;
    constexpr v x1(data1), x2(data2), x3(data3);
    static_assert(rust::visit(arity, x1, x2, x3) == 3);
  }
}

TEST(VariantTestVisit, Exception) {
  return_arity obj {};

  {
    using v = variant<int, valueless_t>;
    v x;
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    EXPECT_THROW(rust::visit(obj, x), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(rust::visit(obj, x1, x2), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(rust::visit(obj, x2, x1), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*, valueless_t>;
    v1 x1;
    v2 x2;
    make_valueless(x1);
    make_valueless(x2);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
    EXPECT_THROW(rust::visit(obj, x1, x2), bad_variant_access);
  }
}

struct derived_variant : variant<short, long, float> { };

template <std::size_t Idx>
void get(const derived_variant&) {
  EXPECT_TRUE(false);
}

TEST(VariantTestVisit, DerivedFromVariant) {
  {
    auto x1 = derived_variant {3};
    const auto cx1 = derived_variant {4};
    rust::visit([](auto val) { EXPECT_EQ(val, 3); }, x1);
    rust::visit([](auto val) { EXPECT_EQ(val, 4); }, cx1);
    rust::visit([](auto val) { EXPECT_EQ(val, -1.25f); },
                derived_variant {-1.25f});
    rust::visit([](auto val) { EXPECT_EQ(val, 3); }, std::move(x1));
    rust::visit([](auto val) { EXPECT_EQ(val, 4); }, std::move(cx1));
  }

  {
    struct evil_variant_base {
      int index;
      char valueless_by_exception;
    };

    struct evil_variant : variant<int, long, double>,
                          std::tuple<int>,
                          evil_variant_base {
      using variant<int, long, double>::variant;
    };

    rust::visit([](auto val) { EXPECT_EQ(val, 12); }, evil_variant(12));
    rust::visit([](auto val) { EXPECT_DOUBLE_EQ(val, 12.3); },
                evil_variant(12.3));
  }

  {
    struct my_variant : variant<int, char, double> {
      using variant::variant;
    };

    my_variant x1(42), x2(3.14);

    auto visitor1 = [](auto&& x) { return static_cast<double>(x); };
    EXPECT_DOUBLE_EQ(rust::visit(visitor1, x1), 42.0);
    EXPECT_DOUBLE_EQ(rust::visit(visitor1, x2), 3.14);

    auto visitor2 = [](auto&& x, auto&& y) {
      return static_cast<double>(x + y);
    };
    EXPECT_DOUBLE_EQ(rust::visit(visitor2, x1, x2), 45.14);
  }

  {
    struct my_variant : variant<int, valueless_t> {
      using variant::variant;
    };

    my_variant x(3);
    EXPECT_FALSE(x.valueless_by_exception());
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());

    auto very_useful_visitor = [](auto&&...) { ADD_FAILURE(); };
    EXPECT_THROW(rust::visit(very_useful_visitor, x), bad_variant_access);
    EXPECT_THROW(rust::visit(very_useful_visitor, my_variant(3), x),
                 bad_variant_access);
    EXPECT_THROW(rust::visit(very_useful_visitor, x, my_variant(3)),
                 bad_variant_access);
  }
}

struct any_visitor {
  template <class Ty>
  void operator()(const Ty&) const { }
};

template <class Ty, class = decltype(rust::visit(std::declval<any_visitor&>(),
                                                 std::declval<Ty>()))>
constexpr bool has_visit(int) {
  return true;
}

template <class Ty>
constexpr bool has_visit(...) {
  return false;
}

TEST(VariantTestVisit, Deleted) {
  struct bad_variant1 : variant<short>, variant<long, float> { };
  struct bad_variant2 : private variant<long, float> { };
  struct good_variant1 : variant<long, float> { };
  struct good_variant2 : good_variant1 { };

  static_assert(!has_visit<int>(0));
#ifndef _MSC_VER
  /*
   * MSVC considers that bad_variant1 can be used for rust::visit because it can
   * be legally used as the argument type of as_variant(). In fact, bad_variant1
   * cannot be used to call as_variant() because the variant base class is
   * ambiguous. In fact, both gcc and Clang give correct results. This seems
   * another bug of MSVC.
   */
  static_assert(!has_visit<bad_variant1>(0));
#else
  bad_variant1 _unused [[maybe_unused]];
#endif
  static_assert(!has_visit<bad_variant2>(0));
  static_assert(has_visit<variant<int>>(0));
  static_assert(has_visit<good_variant1>(0));
  static_assert(has_visit<good_variant2>(0));
}

template <class T>
struct convert_to_visitor {
  template <class U, std::enable_if_t<std::is_convertible_v<U, T>, int> = 0>
  constexpr T operator()(U&& u) const {
    return std::forward<U>(u);
  }
};

TEST(VariantTestVisit, ImmobileFunctionVisit) {
  {
    using v = variant<int, double>;
    immobile_visitor<convert_to_visitor<double>> visitor {};
    EXPECT_DOUBLE_EQ(rust::visit(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(rust::visit(std::as_const(visitor), v {3.14}), 3.14);
    EXPECT_DOUBLE_EQ(rust::visit(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(rust::visit(std::move(std::as_const(visitor)), v {1.414}),
                     1.414);
  }
  {
    using v = variant<int, double>;
    mobile_visitor<convert_to_visitor<double>> visitor {};
    EXPECT_DOUBLE_EQ(rust::visit(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(rust::visit(std::as_const(visitor), v {3.14}), 3.14);
    EXPECT_DOUBLE_EQ(rust::visit(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(rust::visit(std::move(std::as_const(visitor)), v {1.414}),
                     1.414);
  }
}

TEST(VariantTestVisit, VisitPointerToMember) {
  struct base {
    int x;
    int f() const { return x; }
  };

  struct derived : base {
    int y;
  };

  using v = variant<base, derived>;

  EXPECT_EQ(rust::visit(&base::x, v(base {13})), 13);
  EXPECT_EQ(rust::visit(&base::x, v(derived {{42}, 29})), 42);

  EXPECT_EQ(rust::visit(&base::f, v(base {13})), 13);
  EXPECT_EQ(rust::visit(&base::f, v(derived {{42}, 29})), 42);
}

TEST(VariantTestVisit, VisitReference) {
  {
    using v = variant<int&>;
    int data = 3;
    v x(data);
    rust::visit([](auto& ref) { ref = 4; }, x);
    EXPECT_EQ(data, 4);
  }
  {
    using v = variant<float&, double&, int>;
    struct Visitor {
      void operator()(float& arg) const { arg = 4.f; }
      void operator()(double& arg) const { arg = 5.0; }
      void operator()(int& arg) const { arg = 6; }
    };

    float data1 = 1.f;
    double data2 = 2.0;
    int data3 = 3;
    v x1(data1), x2(data2), x3(data3);
    rust::visit(Visitor(), x1);
    rust::visit(Visitor(), x2);
    rust::visit(Visitor(), x3);
    EXPECT_FLOAT_EQ(data1, 4.f);
    EXPECT_DOUBLE_EQ(data2, 5.0);
    EXPECT_EQ(data3, 3);
  }
  {
    using v = variant<int&, double&>;
    int data = 3;
    v x(data);
    EXPECT_EQ(rust::visit([](auto& ref) -> void* { return &ref; }, x), &data);
  }
}

}  // namespace
}  // namespace rust
