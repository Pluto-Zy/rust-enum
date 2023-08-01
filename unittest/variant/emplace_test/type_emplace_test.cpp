#include "emplace_test_helper.hpp"
#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

namespace rust {
namespace {
TEST(VariantTestTypeEmplace, Deleted) {
  struct non_default_constructible {
    non_default_constructible() = delete;
  };

  struct non_convertible {
    explicit non_convertible(int);
  };

  using v = variant<int, void*, const void*, non_default_constructible>;
  static_assert(has_type_emplace<v, int>::value);
  static_assert(has_type_emplace<v, int, int>::value);
  static_assert(!has_type_emplace<v, int, decltype(nullptr)>::value);
  static_assert(has_type_emplace<v, void*, decltype(nullptr)>::value);
  static_assert(has_type_emplace<v, void*, int*>::value);
  static_assert(!has_type_emplace<v, void*, const int*>::value);
  static_assert(!has_type_emplace<v, void*, int>::value);
  static_assert(has_type_emplace<v, const void*, const int*>::value);
  static_assert(has_type_emplace<v, const void*, int*>::value);
  static_assert(!has_type_emplace<v, non_default_constructible>::value);

  static_assert(
      has_type_emplace<variant<non_convertible>, non_convertible, int>::value);
  static_assert(!has_type_emplace<variant<int, int>, int, int>::value);
  static_assert(!has_type_emplace<variant<int, int, long>, int, int>::value);
  static_assert(has_type_emplace<variant<int, int, long>, long, int>::value);

  static_assert(has_type_emplace<variant<int&>, int&, int&>::value);
  static_assert(!has_type_emplace<variant<int&>, int&, int>::value);
  static_assert(!has_type_emplace<variant<int&>, int&, short&>::value);
  static_assert(!has_type_emplace<variant<int&>, int, int&>::value);
  static_assert(!has_type_emplace<variant<int&, int&>, int&, int&>::value);
  static_assert(has_type_emplace<variant<long&, int&>, long&, long&>::value);
  static_assert(has_type_emplace<variant<long&, int&>, int&, int&>::value);
  static_assert(!has_type_emplace<variant<long&, int&>, long&, int&>::value);
  static_assert(has_type_emplace<variant<int, int&>, int, int&>::value);
  static_assert(has_type_emplace<variant<int, int&>, int&, int&>::value);
  static_assert(has_type_emplace<variant<long&, int>, long&, long&>::value);
  static_assert(has_type_emplace<variant<long&, int>, int, long&>::value);
  static_assert(!has_type_emplace<variant<int&, std::string&>, std::string&,
                                  const char*>::value);
  static_assert(!has_type_emplace<variant<int&, std::string&>, std::string&,
                                  const char(&)[3]>::value);
  static_assert(
      !has_type_emplace<variant<int&, const int&>, int&, const int&>::value);
  static_assert(has_type_emplace<variant<int&, const int&>, const int&,
                                 const int&>::value);

  {
    struct more_arguments {
      more_arguments(int, double, float);
    };
    static_assert(has_type_emplace<variant<more_arguments>, more_arguments, int,
                                   double, float>::value);
    static_assert(has_type_emplace<variant<more_arguments>, more_arguments, int,
                                   double, double>::value);
    static_assert(has_type_emplace<variant<more_arguments>, more_arguments,
                                   long, double, float>::value);
    static_assert(!has_type_emplace<variant<more_arguments>, more_arguments,
                                    int, double>::value);
    static_assert(!has_type_emplace<variant<more_arguments>, more_arguments,
                                    int, double, float, int>::value);
    static_assert(has_type_emplace<variant<int, more_arguments>, more_arguments,
                                   int, double, float>::value);
    static_assert(
        !has_type_emplace<variant<int, more_arguments>, more_arguments, int,
                          int, double, float>::value);
    static_assert(!has_type_emplace<variant<more_arguments&>, more_arguments&,
                                    int, double, double>::value);
  }
}

TEST(VariantTestTypeEmplace, BasicBehavior) {
  // Test cases from MSVC STL
  {
    using v = variant<int>;
    v x(3);

    auto& ref1 = x.emplace<int>();
    static_assert(std::is_same<int&, decltype(ref1)>::value);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 0);
    EXPECT_EQ(get<int>(x), 0);
    EXPECT_EQ(&ref1, &get<0>(x));

    auto& ref2 = x.emplace<int>(4);
    static_assert(std::is_same<int&, decltype(ref2)>::value);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 4);
    EXPECT_EQ(get<int>(x), 4);
    EXPECT_EQ(&ref2, &get<0>(x));
  }
  {
    using v = variant<int, long, const void*, std::string>;
    v x(std::in_place_type<int>, -1);

    auto& ref1 = x.emplace<long>();
    static_assert(std::is_same<long&, decltype(ref1)>::value);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x), 0l);
    EXPECT_EQ(get<long>(x), 0l);
    EXPECT_EQ(&ref1, &get<1>(x));

    const int data = 3;
    auto& ref2 = x.emplace<const void*>(&data);
    static_assert(std::is_same<const void*&, decltype(ref2)>::value);
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x), &data);
    EXPECT_EQ(get<const void*>(x), &data);
    EXPECT_EQ(&ref2, &get<2>(x));

    auto& ref3 = x.emplace<std::string>(3u, 'a');
    static_assert(std::is_same<std::string&, decltype(ref3)>::value);
    EXPECT_EQ(x.index(), 3);
    EXPECT_EQ(get<3>(x), "aaa");
    EXPECT_EQ(get<std::string>(x), "aaa");
    EXPECT_EQ(&ref3, &get<3>(x));
  }
  {
    using v = variant<int, counter>;
    {
      v x(std::in_place_type<int>);
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(counter::alive_count, 0);
      x.emplace<counter>();
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      counter::reset();
    }
    {
      v x(std::in_place_type<counter>);
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      x.emplace<int>();
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(counter::alive_count, 0);
      x.emplace<counter>();
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      EXPECT_EQ(counter::copy_construct_count, 0);
      EXPECT_EQ(counter::move_construct_count, 0);
      EXPECT_EQ(counter::copy_assign_count, 0);
      EXPECT_EQ(counter::move_assign_count, 0);
      counter::reset();
    }
    {
      v x(std::in_place_type<counter>);
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      x.emplace<counter>();
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      EXPECT_EQ(counter::copy_construct_count, 0);
      EXPECT_EQ(counter::move_construct_count, 0);
      EXPECT_EQ(counter::copy_assign_count, 0);
      EXPECT_EQ(counter::move_assign_count, 0);
      x.emplace<counter>();
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      EXPECT_EQ(counter::copy_construct_count, 0);
      EXPECT_EQ(counter::move_construct_count, 0);
      EXPECT_EQ(counter::copy_assign_count, 0);
      EXPECT_EQ(counter::move_assign_count, 0);
      x.emplace<int>();
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(counter::alive_count, 0);
      EXPECT_EQ(counter::copy_construct_count, 0);
      EXPECT_EQ(counter::move_construct_count, 0);
      EXPECT_EQ(counter::copy_assign_count, 0);
      EXPECT_EQ(counter::move_assign_count, 0);
      counter::reset();
    }
  }
  {
    using v = variant<counter, valueless_t>;
    v x(std::in_place_type<counter>);
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    EXPECT_EQ(counter::alive_count, 0);
    x.emplace<counter>();
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(counter::alive_count, 1);
    counter::reset();
  }
  {
    struct nothrow_constructible {
      int value;
      bool move_ctor = false;

      nothrow_constructible(int val) noexcept : value(val) { }
      nothrow_constructible(nothrow_constructible&&) noexcept(false) {
        move_ctor = true;
      }
    };

    using v = variant<int, nothrow_constructible>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x.emplace<nothrow_constructible>(3);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).value, 3);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    struct nothrow_move_constructible {
      int value;
      bool move_ctor = false;

      nothrow_move_constructible(int val) noexcept(false) : value(val) { }
      nothrow_move_constructible(nothrow_move_constructible&& other) noexcept :
          value(other.value) {
        move_ctor = true;
      }
    };

    using v = variant<int, nothrow_move_constructible>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x.emplace<nothrow_move_constructible>(3);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).value, 3);
    EXPECT_TRUE(get<1>(x).move_ctor);
  }
  {
    struct throw_constructible1 {
      bool dtor = false;

      throw_constructible1() = default;
      throw_constructible1(int) { throw 1; }
      ~throw_constructible1() { dtor = true; }
    };

    struct throw_constructible2 : throw_constructible1 {
      using throw_constructible1::throw_constructible1;
    };

    using v = variant<throw_constructible1, throw_constructible2>;
    v x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_THROW(x.emplace<throw_constructible2>(3), int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_FALSE(get<0>(x).dtor);
  }
  {
    using v = variant<int&>;
    int data1 = 3, data2 = 4;
    v x = data1;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(&get<0>(x), &data1);
    get<0>(x) = 5;
    EXPECT_EQ(data1, 5);
    x.emplace<int&>(data2);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(&get<0>(x), &data2);
    get<0>(x) = 6;
    EXPECT_EQ(data1, 5);
    EXPECT_EQ(data2, 6);
  }
  {
    using v = variant<const int&, int&>;
    int data1 = 3, data2 = 4;
    v x(std::in_place_index<0>, data1);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(&get<0>(x), &data1);
    x.emplace<int&>(data2);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(&get<1>(x), &data2);
    x.emplace<const int&>(data2);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(&get<0>(x), &data2);
  }
  {
    using v = variant<int&, int>;
    int data1 = 3, data2 = 4;
    v x(std::in_place_index<1>, data1);
    EXPECT_EQ(x.index(), 1);
    EXPECT_NE(&get<1>(x), &data1);
    x.emplace<int&>(data2);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(&get<0>(x), &data2);
    x.emplace<int>(data2);
    EXPECT_EQ(x.index(), 1);
    EXPECT_NE(&get<1>(x), &data2);
  }
  {
    using v = variant<int&, long&, const int>;
    int data1 = 3;
    long data2 = 4;
    v x(std::in_place_type<const int>, data1);
    EXPECT_EQ(x.index(), 2);
    EXPECT_NE(&get<2>(x), &data1);
    x.emplace<int&>(data1);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(&get<0>(x), &data1);
    get<0>(x) = 5;
    EXPECT_EQ(data1, 5);
    x.emplace<long&>(data2);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(&get<1>(x), &data2);
    get<1>(x) = 6;
    EXPECT_EQ(data1, 5);
    EXPECT_EQ(data2, 6);
  }
}

#ifdef USE_CXX20
template <class V, class Ty, class... Args>
constexpr bool constexpr_emplace_impl(V&& v, std::size_t expected_index,
                                      Ty expected_value, Args&&... args) {
  v.template emplace<Ty>(std::forward<Args>(args)...);
  return v.index() == expected_index && get<Ty>(v) == expected_value;
}

struct multiple_args {
  int x, y, z;
  constexpr multiple_args(int x, int y, int z) noexcept : x(x), y(y), z(z) { }
};

constexpr bool operator==(const multiple_args& lhs, const multiple_args& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

constexpr bool operator!=(const multiple_args& lhs, const multiple_args& rhs) {
  return !(lhs == rhs);
}

TEST(VariantTestTypeEmplace, Constexpr) {
  static int i = 0;
  {
    using v = variant<long, int*, int>;
    static_assert(constexpr_emplace_impl(v(3l), 0, 4l, 4l));
    static_assert(constexpr_emplace_impl(v(3), 0, 4l, 4l));
    static_assert(constexpr_emplace_impl(v(3l), 2, 4, 4));
    static_assert(constexpr_emplace_impl(v(3), 2, 4, 4));
    static_assert(constexpr_emplace_impl(v(nullptr), 0, 4l, 4l));
    static_assert(constexpr_emplace_impl(v(3l), 1, &i, &i));
  }
  {
    using v = variant<int, multiple_args>;
    static_assert(constexpr_emplace_impl(v(multiple_args(1, 2, 3)), 0, 4, 4));
    static_assert(
        constexpr_emplace_impl(v(3), 1, multiple_args(1, 2, 3), 1, 2, 3));
  }
}
#endif

template <class Variant, class Type, class... Args>
constexpr bool has_noexcept_emplace_v = noexcept(
    std::declval<Variant>().template emplace<Type>(std::declval<Args>()...));

TEST(VariantTestTypeEmplace, Noexcept) {
  static_assert(has_noexcept_emplace_v<variant<int>, int, int>);
  static_assert(has_noexcept_emplace_v<variant<int>, int, double>);
  static_assert(
      has_noexcept_emplace_v<variant<int, long, long long>, long, int>);
  static_assert(
      has_noexcept_emplace_v<variant<int, long, long long>, long long, float>);
  static_assert(has_noexcept_emplace_v<variant<int, std::string>, int, int>);
  static_assert(!has_noexcept_emplace_v<variant<int, std::string>, std::string,
                                        const char*>);
  static_assert(has_type_emplace<variant<int, std::string>, std::string,
                                 const char*>::value);
  static_assert(has_noexcept_emplace_v<variant<int, long, int*>, int*, int*>);
  static_assert(
      has_noexcept_emplace_v<variant<int, long, int*>, int*, std::nullptr_t>);

  static_assert(has_noexcept_emplace_v<variant<int&>, int&, int&>);
  static_assert(has_noexcept_emplace_v<variant<long&, int&>, long&, long&>);
  static_assert(has_noexcept_emplace_v<variant<long&, int&>, int&, int&>);
  static_assert(has_noexcept_emplace_v<variant<int, int&>, int, int&>);
  static_assert(has_noexcept_emplace_v<variant<int, int&>, int&, int&>);
  static_assert(has_noexcept_emplace_v<variant<long&, int>, long&, long&>);
  static_assert(has_noexcept_emplace_v<variant<long&, int>, int, long&>);
  static_assert(has_noexcept_emplace_v<variant<int&, std::string&>,
                                       std::string&, std::string&>);
  static_assert(has_noexcept_emplace_v<variant<int&, const int&>, const int&,
                                       const int&>);

  {
    struct may_throw_constructible {
      may_throw_constructible(int);
      may_throw_constructible(double) noexcept;
    };
    static_assert(!has_noexcept_emplace_v<variant<may_throw_constructible>,
                                          may_throw_constructible, int>);
    static_assert(has_type_emplace<variant<may_throw_constructible>,
                                   may_throw_constructible, int>::value);
    static_assert(has_noexcept_emplace_v<variant<may_throw_constructible>,
                                         may_throw_constructible, double>);
    static_assert(!has_noexcept_emplace_v<variant<may_throw_constructible, int>,
                                          may_throw_constructible, int>);
    static_assert(has_type_emplace<variant<may_throw_constructible, int>,
                                   may_throw_constructible, int>::value);
    static_assert(has_noexcept_emplace_v<variant<may_throw_constructible, int>,
                                         may_throw_constructible, double>);
    static_assert(!has_noexcept_emplace_v<variant<int, may_throw_constructible>,
                                          may_throw_constructible, int>);
    static_assert(has_type_emplace<variant<int, may_throw_constructible>,
                                   may_throw_constructible, int>::value);
    static_assert(has_noexcept_emplace_v<variant<int, may_throw_constructible>,
                                         may_throw_constructible, double>);
  }
  {
    struct more_arguments {
      more_arguments(int, double, float) noexcept;
      more_arguments(float, double, int);
    };
    static_assert(has_noexcept_emplace_v<variant<more_arguments>,
                                         more_arguments, int, double, float>);
    static_assert(has_noexcept_emplace_v<variant<more_arguments>,
                                         more_arguments, int, double, double>);
    static_assert(!has_noexcept_emplace_v<variant<more_arguments>,
                                          more_arguments, float, double, int>);
    static_assert(has_noexcept_emplace_v<variant<int, more_arguments>,
                                         more_arguments, int, double, float>);
    static_assert(!has_noexcept_emplace_v<variant<int, more_arguments>,
                                          more_arguments, float, double, int>);
  }
}

TEST(VariantTestTypeEmplace, ValuelessStrategy) {
  {
    using v = variant<dummy, may_throw_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x(std::in_place_type<dummy>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<may_throw_constructible_strategy_default>(3);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    using v = variant<dummy, throw_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x(std::in_place_type<dummy>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    EXPECT_THROW(x.emplace<throw_constructible_strategy_default>(3), int);
    EXPECT_TRUE(x.valueless_by_exception());
  }
  {
    using v = variant<dummy, may_throw_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_type<dummy>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<may_throw_constructible_strategy_fallback>(3);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    using v = variant<dummy, throw_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_type<dummy>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 4);

    EXPECT_THROW(x.emplace<throw_constructible_strategy_fallback>(3), int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 2);
  }
  {
    using v = variant<dummy, may_throw_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_type<dummy>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<may_throw_constructible_strategy_novalueless>(3);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    using v = variant<dummy, throw_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_type<dummy>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    EXPECT_THROW(x.emplace<throw_constructible_strategy_novalueless>(3), int);
    EXPECT_FALSE(x.valueless_by_exception());
  }
}
}  // namespace
}  // namespace rust