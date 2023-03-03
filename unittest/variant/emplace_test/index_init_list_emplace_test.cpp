#include "emplace_test_helper.hpp"
#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

namespace rust {
namespace {
TEST(VariantTestIndexInitListEmplace, Deleted) {
  using list = std::initializer_list<int>;

  static_assert(has_index_emplace<variant<list>, 0, list>::value);
  static_assert(has_index_emplace<variant<list>, 0, list&>::value);
  {
    struct constructible_from_list {
      constructible_from_list(list);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(has_index_emplace<v, 0, list>::value);
      static_assert(has_index_emplace<v, 0, list&>::value);
      static_assert(
          !has_index_emplace<v, 0, std::initializer_list<double>>::value);
      static_assert(!has_index_emplace<v, 0, list, int>::value);
      static_assert(!has_index_emplace<v, 0, int>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(has_index_emplace<v, 1, list>::value);
      static_assert(has_index_emplace<v, 1, list&>::value);
    }
  }
  {
    struct constructible_from_list {
      constructible_from_list(list&);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(has_index_emplace<v, 0, list>::value);
      static_assert(has_index_emplace<v, 0, list&>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(has_index_emplace<v, 1, list>::value);
      static_assert(has_index_emplace<v, 1, list&>::value);
    }
  }
  {
    struct constructible_from_list {
      constructible_from_list(list&&);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(has_index_emplace<v, 0, list>::value);
      static_assert(!has_index_emplace<v, 0, list&>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(has_index_emplace<v, 1, list>::value);
      static_assert(!has_index_emplace<v, 1, list&>::value);
    }
  }
  {
    struct constructible_from_list_args {
      constructible_from_list_args(list, int);
    };

    {
      using v = variant<constructible_from_list_args>;

      static_assert(has_index_emplace<v, 0, list, int>::value);
      static_assert(has_index_emplace<v, 0, list, double>::value);
      static_assert(!has_index_emplace<v, 0, list>::value);
      static_assert(!has_index_emplace<v, 0, list, int, int>::value);
    }
    {
      using v = variant<int, constructible_from_list_args>;

      static_assert(has_index_emplace<v, 1, list, int>::value);
      static_assert(has_index_emplace<v, 1, list, double>::value);
      static_assert(!has_index_emplace<v, 1, list>::value);
      static_assert(!has_index_emplace<v, 1, list, int, int>::value);
    }
  }
}

template <class Variant, std::size_t Index, class... Args>
constexpr bool has_noexcept_emplace_v = noexcept(
    std::declval<Variant>().template emplace<Index>(std::declval<Args>()...));

TEST(VariantTestIndexInitListEmplace, Noexcept) {
  using list = std::initializer_list<int>;
  {
    struct may_throw_constructible {
      may_throw_constructible(list, int);
      may_throw_constructible(list, double) noexcept;
    };

    {
      using v = variant<may_throw_constructible>;

      static_assert(!has_noexcept_emplace_v<v, 0, list, int>);
      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list, int>::value);
      static_assert(has_noexcept_emplace_v<v, 0, list, double>);
    }
  }
}

TEST(VariantTestIndexInitListEmplace, BasicBehavior) {
  using list_t = std::initializer_list<int>;

  struct constructible_from_init_list {
    std::size_t list_size;
    constexpr constructible_from_init_list(list_t list) :
        list_size(list.size()) { }
  };

  struct constructible_from_init_list_with_arg {
    std::size_t list_size;
    int value;
    constexpr constructible_from_init_list_with_arg(list_t list, int val) :
        list_size(list.size()), value(val) { }
  };

  {
    using v = variant<int, constructible_from_init_list,
                      constructible_from_init_list_with_arg>;
    v x;

    auto& ref1 = x.emplace<1>({1, 2, 3});
    static_assert(
        std::is_same<constructible_from_init_list&, decltype(ref1)>::value);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).list_size, 3);
    EXPECT_EQ(&ref1, &get<1>(x));

    auto& ref2 = x.emplace<2>({1, 2, 3, 4}, 42);
    static_assert(std::is_same<constructible_from_init_list_with_arg&,
                               decltype(ref2)>::value);
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x).list_size, 4);
    EXPECT_EQ(get<2>(x).value, 42);
    EXPECT_EQ(&ref2, &get<2>(x));

    auto& ref3 = x.emplace<1>({2});
    static_assert(
        std::is_same<constructible_from_init_list&, decltype(ref3)>::value);
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).list_size, 1);
    EXPECT_EQ(&ref3, &get<1>(x));
  }
  {
    using v = variant<int, constructible_from_init_list,
                      constructible_from_init_list_with_arg, valueless_t>;
    v x;
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    x.emplace<1>({1, 2, 3});
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).list_size, 3);

    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    x.emplace<2>({1, 2, 3}, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x).list_size, 3);
    EXPECT_EQ(get<2>(x).value, 4);
  }
  {
    struct nothrow_constructible {
      int value;
      bool move_ctor = false;

      nothrow_constructible(list_t list) noexcept :
          value(static_cast<int>(list.size())) { }
      nothrow_constructible(list_t, int val) noexcept : value(val) { }
      nothrow_constructible(nothrow_constructible&&) noexcept(false) {
        move_ctor = true;
      }
    };

    using v = variant<int, nothrow_constructible, nothrow_constructible>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x.emplace<1>({1, 2, 3});
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).value, 3);
    EXPECT_FALSE(get<1>(x).move_ctor);
    x.emplace<2>({1, 2, 3}, 4);
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x).value, 4);
    EXPECT_FALSE(get<2>(x).move_ctor);
  }
  {
    struct nothrow_move_constructible {
      int value;
      bool move_ctor = false;

      nothrow_move_constructible(list_t list) noexcept(false) :
          value(static_cast<int>(list.size())) { }
      nothrow_move_constructible(list_t, int val) noexcept(false) :
          value(val) { }
      nothrow_move_constructible(nothrow_move_constructible&& other) noexcept :
          value(other.value) {
        move_ctor = true;
      }
    };

    using v =
        variant<int, nothrow_move_constructible, nothrow_move_constructible>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x.emplace<1>({1, 2, 3});
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x).value, 3);
    EXPECT_TRUE(get<1>(x).move_ctor);
    x.emplace<2>({1, 2, 3}, 4);
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x).value, 4);
    EXPECT_TRUE(get<2>(x).move_ctor);
  }
  {
    struct throw_constructible {
      bool dtor = false;

      throw_constructible() = default;
      throw_constructible(list_t) { throw 1; }
      ~throw_constructible() { dtor = true; }
    };

    using v = variant<throw_constructible, throw_constructible>;
    v x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_THROW(x.emplace<1>({1, 2, 3}), int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_FALSE(get<0>(x).dtor);
  }
}

// TODO: constexpr

TEST(VariantTestIndexInitListEmplace, ValuelessStrategy) {
  {
    using v = variant<dummy, may_throw_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 4);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>({1, 2, 3});
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);

    x.emplace<0>();
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>({1, 2, 3}, 4);
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

    EXPECT_THROW(x.emplace<1>({1, 2, 3}), int);
    EXPECT_TRUE(x.valueless_by_exception());

    x.emplace<0>();
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    EXPECT_THROW(x.emplace<1>({1, 2, 3}, 4), int);
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

    x.emplace<1>({1, 2, 3});
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);

    x.emplace<0>();
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>({1, 2, 3}, 4);
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

    EXPECT_THROW(x.emplace<1>({1, 2, 3}), int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 2);

    x.emplace<0>(4);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 4);

    EXPECT_THROW(x.emplace<1>({1, 2, 3}, 4), int);
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

    x.emplace<1>({1, 2, 3});
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x).move_ctor);

    x.emplace<0>();
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);

    x.emplace<1>({1, 2, 3}, 4);
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

    EXPECT_THROW(x.emplace<1>({1, 2, 3}), int);
    EXPECT_FALSE(x.valueless_by_exception());
  }
}
}  // namespace
}  // namespace rust
