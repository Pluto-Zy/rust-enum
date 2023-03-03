#include "emplace_test_helper.hpp"
#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

namespace rust {
namespace {
TEST(VariantTestIndexEmplace, Deleted) {
  struct non_default_constructible {
    non_default_constructible() = delete;
  };

  struct non_convertible {
    explicit non_convertible(int);
  };

  using v = variant<int, void*, const void*, non_default_constructible>;
  static_assert(has_index_emplace<v, 0>::value);
  static_assert(has_index_emplace<v, 0, int>::value);
  static_assert(!has_index_emplace<v, 0, decltype(nullptr)>::value);
  static_assert(has_index_emplace<v, 1, decltype(nullptr)>::value);
  static_assert(has_index_emplace<v, 1, int*>::value);
  static_assert(!has_index_emplace<v, 1, const int*>::value);
  static_assert(!has_index_emplace<v, 1, int>::value);
  static_assert(has_index_emplace<v, 2, const int*>::value);
  static_assert(has_index_emplace<v, 2, int*>::value);
  static_assert(!has_index_emplace<v, 3>::value);

  static_assert(has_index_emplace<variant<non_convertible>, 0, int>::value);
}

TEST(VariantTestIndexEmplace, BasicBehavior) {
  // Test cases from MSVC STL
  {
    using v = variant<int>;
    v x(3);

    auto& ref1 = x.emplace<0>();
    static_assert(std::is_same<int&, decltype(ref1)>::value);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 0);
    EXPECT_EQ(get<int>(x), 0);
    EXPECT_EQ(&ref1, &get<0>(x));

    auto& ref2 = x.emplace<0>(4);
    static_assert(std::is_same<int&, decltype(ref2)>::value);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 4);
    EXPECT_EQ(get<int>(x), 4);
    EXPECT_EQ(&ref2, &get<0>(x));
  }
  {
    using v = variant<int, long, const void*, std::string>;
    v x(std::in_place_index<0>, -1);

    auto& ref1 = x.emplace<1>();
    static_assert(std::is_same<long&, decltype(ref1)>::value);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x), 0l);
    EXPECT_EQ(get<long>(x), 0l);
    EXPECT_EQ(&ref1, &get<1>(x));

    const int data = 3;
    auto& ref2 = x.emplace<2>(&data);
    static_assert(std::is_same<const void*&, decltype(ref2)>::value);
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x), &data);
    EXPECT_EQ(get<const void*>(x), &data);
    EXPECT_EQ(&ref2, &get<2>(x));

    auto& ref3 = x.emplace<3>(3u, 'a');
    static_assert(std::is_same<std::string&, decltype(ref3)>::value);
    EXPECT_EQ(x.index(), 3);
    EXPECT_EQ(get<3>(x), "aaa");
    EXPECT_EQ(get<std::string>(x), "aaa");
    EXPECT_EQ(&ref3, &get<3>(x));
  }
  {
    using v = variant<int, counter>;
    {
      v x(std::in_place_index<0>);
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(counter::alive_count, 0);
      x.emplace<1>();
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      counter::reset();
    }
    {
      v x(std::in_place_index<1>);
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      x.emplace<0>();
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(counter::alive_count, 0);
      counter::reset();
    }
    {
      v x(std::in_place_index<1>);
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      x.emplace<1>();
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(counter::alive_count, 1);
      EXPECT_EQ(counter::copy_construct_count, 0);
      EXPECT_EQ(counter::move_construct_count, 0);
      EXPECT_EQ(counter::copy_assign_count, 0);
      EXPECT_EQ(counter::move_assign_count, 0);
      counter::reset();
    }
  }
  {
    using v = variant<counter, valueless_t>;
    v x(std::in_place_index<0>);
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    EXPECT_EQ(counter::alive_count, 0);
    x.emplace<0>();
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
    x.emplace<1>(3);
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
    x.emplace<1>(3);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).value, 3);
    EXPECT_TRUE(get<1>(x).move_ctor);
  }
  {
    struct throw_constructible {
      bool dtor = false;

      throw_constructible() = default;
      throw_constructible(int) { throw 1; }
      ~throw_constructible() { dtor = true; }
    };

    using v = variant<throw_constructible, throw_constructible>;
    v x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_THROW(x.emplace<1>(3), int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_FALSE(get<0>(x).dtor);
  }
}

#ifdef USE_CXX20
template <std::size_t Index, class V, class Ty, class... Args>
constexpr bool constexpr_emplace_impl(V&& v, Ty expected_value,
                                      Args&&... args) {
  v.template emplace<Index>(std::forward<Args>(args)...);
  return v.index() == Index && get<Ty>(v) == expected_value;
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

TEST(VariantTestIndexEmplace, Constexpr) {
  static int i = 0;
  {
    using v = variant<long, int*, int>;
    static_assert(constexpr_emplace_impl<0>(v(3l), 4l, 4l));
    static_assert(constexpr_emplace_impl<0>(v(3), 4l, 4l));
    static_assert(constexpr_emplace_impl<2>(v(3l), 4, 4));
    static_assert(constexpr_emplace_impl<2>(v(3), 4, 4));
    static_assert(constexpr_emplace_impl<0>(v(nullptr), 4l, 4l));
    static_assert(constexpr_emplace_impl<1>(v(3l), &i, &i));
  }
  {
    using v = variant<int, multiple_args>;
    static_assert(constexpr_emplace_impl<0>(v(multiple_args(1, 2, 3)), 4, 4));
    static_assert(
        constexpr_emplace_impl<1>(v(3), multiple_args(1, 2, 3), 1, 2, 3));
  }
}
#endif

template <class Variant, std::size_t Index, class... Args>
constexpr bool has_noexcept_emplace_v = noexcept(
    std::declval<Variant>().template emplace<Index>(std::declval<Args>()...));

TEST(VariantTestIndexEmplace, Noexcept) {
  static_assert(has_noexcept_emplace_v<variant<int>, 0, int>);
  static_assert(has_noexcept_emplace_v<variant<int>, 0, double>);
  static_assert(has_noexcept_emplace_v<variant<int, long, long long>, 1, int>);
  static_assert(
      has_noexcept_emplace_v<variant<int, long, long long>, 2, float>);
  static_assert(has_noexcept_emplace_v<variant<int, std::string>, 0, int>);
  static_assert(
      !has_noexcept_emplace_v<variant<int, std::string>, 1, const char*>);
  static_assert(has_noexcept_emplace_v<variant<int, int>, 1, int>);
  static_assert(has_noexcept_emplace_v<variant<int, long, int*>, 2, int*>);
  static_assert(
      has_noexcept_emplace_v<variant<int, long, int*>, 2, std::nullptr_t>);

  {
    struct may_throw_constructible {
      may_throw_constructible(int);
      may_throw_constructible(double) noexcept;
    };
    static_assert(
        !has_noexcept_emplace_v<variant<may_throw_constructible>, 0, int>);
    static_assert(
        has_index_emplace<variant<may_throw_constructible>, 0, int>::value);
    static_assert(
        has_noexcept_emplace_v<variant<may_throw_constructible>, 0, double>);
    static_assert(
        !has_noexcept_emplace_v<variant<may_throw_constructible, int>, 0, int>);
    static_assert(has_index_emplace<variant<may_throw_constructible, int>, 0,
                                    int>::value);
    static_assert(has_noexcept_emplace_v<variant<may_throw_constructible, int>,
                                         0, double>);
    static_assert(
        !has_noexcept_emplace_v<variant<int, may_throw_constructible>, 1, int>);
    static_assert(has_index_emplace<variant<int, may_throw_constructible>, 1,
                                    int>::value);
    static_assert(has_noexcept_emplace_v<variant<int, may_throw_constructible>,
                                         1, double>);
  }
  {
    struct more_arguments {
      more_arguments(int, double, float) noexcept;
      more_arguments(float, double, int);
    };
    static_assert(
        has_noexcept_emplace_v<variant<more_arguments>, 0, int, double, float>);
    static_assert(has_noexcept_emplace_v<variant<more_arguments>, 0, int,
                                         double, double>);
    static_assert(!has_noexcept_emplace_v<variant<more_arguments>, 0, float,
                                          double, int>);
    static_assert(has_noexcept_emplace_v<variant<int, more_arguments>, 1, int,
                                         double, float>);
    static_assert(!has_noexcept_emplace_v<variant<int, more_arguments>, 1,
                                          float, double, int>);
  }
}

TEST(VariantTestIndexEmplace, ValuelessStrategy) {
  {
    using v = variant<dummy, may_throw_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>(3);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    using v = variant<dummy, throw_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    EXPECT_THROW(x.emplace<1>(3), int);
    EXPECT_TRUE(x.valueless_by_exception());
  }
  {
    using v = variant<dummy, may_throw_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>(3);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    using v = variant<dummy, throw_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 4);

    EXPECT_THROW(x.emplace<1>(3), int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 2);
  }
  {
    using v = variant<dummy, may_throw_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>(3);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);
  }
  {
    using v = variant<dummy, throw_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    EXPECT_THROW(x.emplace<1>(3), int);
    EXPECT_FALSE(x.valueless_by_exception());
  }
}
}  // namespace
}  // namespace rust
