#include "visit_test_helper.hpp"
#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
template <class Ret>
void ret_visit_forward_call_operator_test_impl() {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    rust::visit<Ret>(obj);
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_LVALUE));
    rust::visit<Ret>(cobj);
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_LVALUE));
    rust::visit<Ret>(std::move(obj));
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_RVALUE));
    rust::visit<Ret>(std::move(cobj));
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int>;
    v x(3);
    rust::visit<Ret>(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_LVALUE));
    rust::visit<Ret>(cobj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_LVALUE));
    rust::visit<Ret>(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_RVALUE));
    rust::visit<Ret>(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    rust::visit<Ret>(obj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_LVALUE));
    rust::visit<Ret>(cobj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_LVALUE));
    rust::visit<Ret>(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_RVALUE));
    rust::visit<Ret>(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_RVALUE));
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    rust::visit<Ret>(obj, x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_LVALUE)));
    rust::visit<Ret>(cobj, x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_LVALUE)));
    rust::visit<Ret>(std::move(obj), x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_RVALUE)));
    rust::visit<Ret>(std::move(cobj), x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(4), x4(1.1);
    rust::visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    rust::visit<Ret>(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_LVALUE)));
    rust::visit<Ret>(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    rust::visit<Ret>(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    rust::visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    rust::visit<Ret>(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_LVALUE)));
    rust::visit<Ret>(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    rust::visit<Ret>(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_RVALUE)));
  }
}

TEST(VariantTestReturnVisit, ForwardCallOperator) {
  ret_visit_forward_call_operator_test_impl<void>();
  ret_visit_forward_call_operator_test_impl<const void>();
  ret_visit_forward_call_operator_test_impl<int>();
  ret_visit_forward_call_operator_test_impl<long long>();
}

template <class Ret>
void ret_visit_forward_argument_test_impl() {
  using fn = forwarding_call_object;
  fn obj {};
  const auto expected = CT_LVALUE | CT_NON_CONST;
  {
    using v = variant<int>;
    v x(3);
    const v& cx = x;
    rust::visit<Ret>(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(expected));
    rust::visit<Ret>(obj, cx);
    EXPECT_TRUE(fn::check_call<const int&>(expected));
    rust::visit<Ret>(obj, std::move(x));
    EXPECT_TRUE(fn::check_call<int&&>(expected));
    rust::visit<Ret>(obj, std::move(cx));
    EXPECT_TRUE(fn::check_call<const int&&>(expected));
  }
  {
    using v = variant<int, std::string, long>;
    v x1(3), x2("hello"), x3(3l);
    rust::visit<Ret>(obj, x1, x2, x3);
    EXPECT_TRUE((fn::check_call<int&, std::string&, long&>(expected)));
    rust::visit<Ret>(obj, std::as_const(x1), std::as_const(x2), std::move(x3));
    EXPECT_TRUE(
        (fn::check_call<const int&, const std::string&, long&&>(expected)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    rust::visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(expected)));
    rust::visit<Ret>(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
                     std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int&&, double&&>(
            expected)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    rust::visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&, int*&, double&>(expected)));
    rust::visit<Ret>(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
                     std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int*&&, double&&>(
            expected)));
  }
}

TEST(VariantTestReturnVisit, ForwardArgument) {
  ret_visit_forward_argument_test_impl<void>();
  ret_visit_forward_argument_test_impl<const void>();
  ret_visit_forward_argument_test_impl<int>();
  ret_visit_forward_argument_test_impl<long long>();
}

template <class Ret>
void ret_visit_return_type_test_impl() {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    static_assert(std::is_same<decltype(rust::visit<Ret>(obj)), Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(cobj)), Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(std::move(obj))), Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(std::move(cobj))), Ret>::value);
  }
  {
    using v = variant<int>;
    v x(3);
    static_assert(std::is_same<decltype(rust::visit<Ret>(obj, x)), Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(cobj, x)), Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(std::move(obj), x)),
                               Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(std::move(cobj), x)),
                               Ret>::value);
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    static_assert(std::is_same<decltype(rust::visit<Ret>(obj, x)), Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(cobj, x)), Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(std::move(obj), x)),
                               Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(std::move(cobj), x)),
                               Ret>::value);
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(obj, x1, x2)), Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(cobj, x1, x2)), Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(std::move(obj), x1, x2)),
                     Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(std::move(cobj), x1, x2)),
                     Ret>::value);
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    static_assert(std::is_same<decltype(rust::visit<Ret>(obj, x1, x2, x3, x4)),
                               Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(cobj, x1, x2, x3, x4)),
                               Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(std::move(obj), x1, x2, x3, x4)),
                     Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(std::move(cobj), x1,
                                                         x2, x3, x4)),
                               Ret>::value);
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    static_assert(std::is_same<decltype(rust::visit<Ret>(obj, x1, x2, x3, x4)),
                               Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(cobj, x1, x2, x3, x4)),
                               Ret>::value);
    static_assert(
        std::is_same<decltype(rust::visit<Ret>(std::move(obj), x1, x2, x3, x4)),
                     Ret>::value);
    static_assert(std::is_same<decltype(rust::visit<Ret>(std::move(cobj), x1,
                                                         x2, x3, x4)),
                               Ret>::value);
  }
}

struct simple_base {
  int x;
  constexpr explicit simple_base(int i) noexcept : x {i} { }
};

template <int>
struct simple_derived : simple_base {
  using simple_base::simple_base;
};

#ifdef USE_CXX20
using std::identity;
#else
struct identity {
  template <class Ty>
  constexpr Ty&& operator()(Ty&& t) const noexcept {
    return std::forward<Ty>(t);
  }
};
#endif

TEST(VariantTestReturnVisit, ReturnType) {
  ret_visit_return_type_test_impl<void>();
  ret_visit_return_type_test_impl<int>();

  {
    using v = variant<simple_derived<0>, simple_derived<1>, simple_derived<2>>;
    v x(simple_derived<1>(3));

    auto&& r1 = rust::visit<simple_base&>(identity {}, x);
    static_assert(std::is_same<decltype(r1), simple_base&>::value);
    EXPECT_EQ(r1.x, 3);

    auto&& r2 = rust::visit<const simple_base&>(identity {}, std::as_const(x));
    static_assert(std::is_same<decltype(r2), const simple_base&>::value);
    EXPECT_EQ(r2.x, 3);

    auto&& r3 = rust::visit<simple_base&&>(identity {}, std::move(x));
    static_assert(std::is_same<decltype(r3), simple_base&&>::value);
    EXPECT_EQ(r3.x, 3);

    auto&& r4 = rust::visit<const simple_base&&>(identity {},
                                                 std::move(std::as_const(x)));
    static_assert(std::is_same<decltype(r4), const simple_base&&>::value);
    EXPECT_EQ(r4.x, 3);
  }
}

TEST(VariantTestReturnVisit, ConstexprVoid) {
  constexpr return_first first {};
  constexpr return_arity arity {};

  {
    using v = variant<int>;
    constexpr v x(3);
    static_assert((rust::visit<void>(first, x), 3) == 3);
  }
  {
    using v = variant<short, long, char>;
    constexpr v x(3l);
    static_assert((rust::visit<void>(first, x), 3l) == 3l);
  }
  {
    using v1 = variant<int>;
    using v2 = variant<int, char*, long long>;
    using v3 = variant<bool, int, int>;
    constexpr v1 x1;
    constexpr v2 x2(nullptr);
    constexpr v3 x3;
    static_assert((rust::visit<void>(arity, x1, x2, x3), 3) == 3);
  }
  {
    using v = variant<int, long, double, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert((rust::visit<void>(arity, x1, x2, x3, x4), 4) == 4);
  }
  {
    using v = variant<int, long, double, long long, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert((rust::visit<void>(arity, x1, x2, x3, x4), 4) == 4);
  }
}

TEST(VariantTestReturnVisit, ConstexprOther) {
  constexpr return_first first {};
  constexpr return_arity arity {};

  {
    using v = variant<int>;
    constexpr v x(3);
    static_assert(rust::visit<int>(first, x) == 3);
  }
  {
    using v = variant<short, long, char>;
    constexpr v x(3l);
    static_assert(rust::visit<int>(first, x) == 3);
    static_assert(rust::visit<long>(first, x) == 3l);
  }
  {
    using v1 = variant<int>;
    using v2 = variant<int, char*, long long>;
    using v3 = variant<bool, int, int>;
    constexpr v1 x1;
    constexpr v2 x2(nullptr);
    constexpr v3 x3;
    static_assert(rust::visit<std::size_t>(arity, x1, x2, x3) == 3);
    static_assert(rust::visit<double>(arity, x1, x2, x3) == 3.0);
    static_assert(rust::visit<long long>(arity, x1, x2, x3) == 3ll);
  }
  {
    using v = variant<int, long, double, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(rust::visit<int>(arity, x1, x2, x3, x4) == 4);
  }
  {
    using v = variant<int, long, double, long long, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(rust::visit<int>(arity, x1, x2, x3, x4) == 4);
  }
}

template <class Ret>
void ret_visit_exception_test_impl() {
  return_arity obj {};

  {
    using v = variant<int, valueless_t>;
    v x;
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    EXPECT_THROW(rust::visit<Ret>(obj, x), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(rust::visit<Ret>(obj, x1, x2), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(rust::visit<Ret>(obj, x2, x1), bad_variant_access);
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
    EXPECT_THROW(rust::visit<Ret>(obj, x1, x2), bad_variant_access);
  }
}

TEST(VariantTestReturnVisit, Exception) {
  ret_visit_exception_test_impl<void>();
  ret_visit_exception_test_impl<const void>();
  ret_visit_exception_test_impl<int>();
  ret_visit_exception_test_impl<long long>();
}

TEST(VariantTestReturnVisit, CallerAcceptNonConst) {
  struct test { };
  struct visitor {
    void operator()(test&) { }
  };

  variant<test> x;
  rust::visit<void>(visitor {}, x);
}

TEST(VariantTestReturnVisit, ConstexprVisitExplicitSideEffect) {
  auto test_lambda = [](int arg) constexpr {
    variant<int> x = 101;
    rust::visit<void>([arg](int& x) constexpr { x = arg; }, x);
    return get<int>(x);
  };

  static_assert(test_lambda(202) == 202);
}

TEST(VariantTestReturnVisit, DerivedFromVariant) {
  {
    struct my_variant : variant<short, long, float> { };

    rust::visit<bool>(
        [](auto x) {
          EXPECT_EQ(x, 3);
          return true;
        },
        my_variant {3});
    rust::visit<bool>(
        [](auto x) {
          EXPECT_EQ(x, -1.3f);
          return true;
        },
        my_variant {-1.3f});
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

    rust::visit<bool>(
        [](auto x) {
          EXPECT_EQ(x, 12);
          return true;
        },
        evil_variant(12));
    rust::visit<bool>(
        [](auto x) {
          EXPECT_EQ(x, 12.3);
          return true;
        },
        evil_variant(12.3));
  }

  {
    struct my_variant : variant<int, char, double> {
      using variant::variant;
    };

    my_variant x1(42), x2(3.14);

    auto visitor1 = [](auto&& x) { return x; };
    EXPECT_DOUBLE_EQ(rust::visit<double>(visitor1, x1), 42.0);
    EXPECT_DOUBLE_EQ(rust::visit<double>(visitor1, x2), 3.14);

    auto visitor2 = [](auto&& x, auto&& y) { return x + y; };
    EXPECT_DOUBLE_EQ(rust::visit<double>(visitor2, x1, x2), 45.14);
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
    EXPECT_THROW(rust::visit<void>(very_useful_visitor, x), bad_variant_access);
    EXPECT_THROW(rust::visit<void>(very_useful_visitor, my_variant(3), x),
                 bad_variant_access);
    EXPECT_THROW(rust::visit<void>(very_useful_visitor, x, my_variant(3)),
                 bad_variant_access);
  }
}

struct any_visitor_for_ret_visit {
  template <class Ty>
  bool operator()(const Ty&) {
    return true;
  }
};

template <class Ty,
          class = decltype(rust::visit<bool>(
              std::declval<any_visitor_for_ret_visit&>(), std::declval<Ty>()))>
constexpr bool has_ret_visit(int) {
  return true;
}

template <class Ty>
constexpr bool has_ret_visit(...) {
  return false;
}

TEST(VariantTestReturnVisit, Deleted) {
  struct bad_variant1 : variant<short>, variant<long, float> { };
  struct bad_variant2 : private variant<long, float> { };
  struct good_variant1 : variant<long, float> { };
  struct good_variant2 : good_variant1 { };

  static_assert(!has_ret_visit<int>(0));
#ifndef _MSC_VER
  /*
   * MSVC considers that bad_variant1 can be used for rust::visit because it can
   * be legally used as the argument type of as_variant(). In fact, bad_variant1
   * cannot be used to call as_variant() because the variant base class is
   * ambiguous. In fact, both gcc and Clang give correct results. This seems
   * another bug of MSVC.
   */
  static_assert(!has_ret_visit<bad_variant1>(0));
#else
  bad_variant1 _unused [[maybe_unused]];
#endif
  static_assert(!has_ret_visit<bad_variant2>(0));
  static_assert(has_ret_visit<variant<int>>(0));
  static_assert(has_ret_visit<good_variant1>(0));
  static_assert(has_ret_visit<good_variant2>(0));
}

struct mobile_data {
  int x;

  /* implicit */ mobile_data(int i) : x {i} { }
  mobile_data(const mobile_data&) { ADD_FAILURE(); }
  mobile_data(mobile_data&&) { ADD_FAILURE(); }
  mobile_data& operator=(const mobile_data&) {
    ADD_FAILURE();
    return *this;
  }
  mobile_data& operator=(mobile_data&&) {
    ADD_FAILURE();
    return *this;
  }
};

struct immobile_data : mobile_data {
  using mobile_data::mobile_data;
  immobile_data(const immobile_data&) = delete;
  immobile_data& operator=(const immobile_data&) = delete;
};

TEST(VariantTestReturnVisit, PerfectReturn) {
  {
    using ret = immobile_data;
    EXPECT_EQ(rust::visit<ret>(identity {}, variant<int, short>(13)).x, 13);
    EXPECT_EQ(rust::visit<ret>(identity {}, variant<int, short>((short) 13)).x,
              13);

    struct convertible_to_immobile_one {
      operator immobile_data() const { return immobile_data {1729}; }
    };

    struct convertible_to_immobile_other {
      operator immobile_data() const { return immobile_data {1138}; }
    };

    using v =
        variant<convertible_to_immobile_one, convertible_to_immobile_other>;
    EXPECT_EQ(rust::visit<ret>(identity {}, v(convertible_to_immobile_one())).x,
              1729);
    EXPECT_EQ(
        rust::visit<ret>(identity {}, v(convertible_to_immobile_other())).x,
        1138);
  }
  {
    using ret = mobile_data;
    EXPECT_EQ(rust::visit<ret>(identity {}, variant<int, short>(13)).x, 13);
    EXPECT_EQ(rust::visit<ret>(identity {}, variant<int, short>((short) 13)).x,
              13);

    ret r1(1729), r2(1138);
    auto visitor1 = [&r1](auto) mutable -> ret& { return r1; };
    EXPECT_EQ(rust::visit<ret&>(visitor1, variant<int, short>(13)).x, 1729);
    auto visitor2 = [&r2](auto) mutable -> ret&& { return std::move(r2); };
    EXPECT_EQ(rust::visit<ret&&>(visitor2, variant<int, short>(13)).x, 1138);
  }
}

TEST(VariantTestReturnVisit, ImmobileFunction) {
  {
    using v = variant<int, double>;
    immobile_visitor<identity> visitor {};
    EXPECT_DOUBLE_EQ(rust::visit<double>(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(rust::visit<double>(std::as_const(visitor), v {3.14}),
                     3.14);
    EXPECT_DOUBLE_EQ(rust::visit<double>(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(
        rust::visit<double>(std::move(std::as_const(visitor)), v {1.414}),
        1.414);
  }
  {
    using v = variant<int, double>;
    mobile_visitor<identity> visitor {};
    EXPECT_DOUBLE_EQ(rust::visit<double>(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(rust::visit<double>(std::as_const(visitor), v {3.14}),
                     3.14);
    EXPECT_DOUBLE_EQ(rust::visit<double>(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(
        rust::visit<double>(std::move(std::as_const(visitor)), v {1.414}),
        1.414);
  }
}

}  // namespace
}  // namespace rust
