#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

namespace rust {
namespace {
TEST(VariantTestMoveAssignment, Deleted) {
  static_assert(std::is_move_assignable<variant<int>>::value);
  static_assert(std::is_move_assignable<variant<int, double>>::value);
  static_assert(!std::is_move_assignable<variant<const int>>::value);
  static_assert(!std::is_move_assignable<variant<int, const double>>::value);
  static_assert(std::is_move_assignable<variant<monostate>>::value);
  static_assert(std::is_move_assignable<variant<int, monostate>>::value);

  struct move_only {
    move_only(const move_only&) = delete;
    move_only(move_only&&) = default;
    move_only& operator=(const move_only&) = delete;
    move_only& operator=(move_only&&) = default;
  };
  static_assert(!std::is_copy_assignable<variant<int, move_only>>::value);
  static_assert(std::is_move_assignable<variant<int, move_only>>::value);

  struct move_construct_only {
    move_construct_only(const move_construct_only&) = delete;
    move_construct_only(move_construct_only&&) = default;
    move_construct_only& operator=(const move_construct_only&) = delete;
    move_construct_only& operator=(move_construct_only&&) = delete;
  };
  static_assert(
      !std::is_copy_assignable<variant<int, move_construct_only>>::value);
  static_assert(
      !std::is_move_assignable<variant<int, move_construct_only>>::value);

  struct move_assign_only {
    move_assign_only(const move_assign_only&) = delete;
    move_assign_only(move_assign_only&&) = delete;
    move_assign_only& operator=(const move_assign_only&) = delete;
    move_assign_only& operator=(move_assign_only&&) = default;
  };
  static_assert(
      !std::is_copy_assignable<variant<int, move_assign_only>>::value);
  static_assert(
      !std::is_move_assignable<variant<int, move_assign_only>>::value);

  struct non_moveable {
    non_moveable() = default;
    non_moveable(const non_moveable&) = default;
    non_moveable(non_moveable&&) = delete;
    non_moveable& operator=(const non_moveable&) = default;
    non_moveable& operator=(non_moveable&&) = delete;
  };
  static_assert(std::is_copy_assignable<variant<int, non_moveable>>::value);
  // use copy assignment
  static_assert(std::is_move_assignable<variant<int, non_moveable>>::value);

  {
    using v = variant<counter, non_moveable>;
    static_assert(std::is_copy_assignable<v>::value);
    static_assert(std::is_move_assignable<v>::value);

    v x1(std::in_place_index<1>);
    v x2(std::in_place_index<0>);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);

    x1 = std::move(x2);
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_construct_count, 1);
    EXPECT_EQ(counter::move_construct_count, 0);
  }
}

TEST(VariantTestMoveAssignment, Trivial) {
  static_assert(std::is_trivially_move_assignable<variant<int>>::value);
  static_assert(std::is_trivially_move_assignable<variant<int, long>>::value);
  static_assert(std::is_trivially_move_assignable<variant<monostate>>::value);
  static_assert(
      std::is_trivially_move_assignable<variant<int, monostate>>::value);

  struct trivially_moveable {
    trivially_moveable(const trivially_moveable&) = default;
    trivially_moveable(trivially_moveable&&) = default;
    trivially_moveable& operator=(const trivially_moveable&) = default;
    trivially_moveable& operator=(trivially_moveable&&) = default;
  };
  static_assert(std::is_trivially_move_assignable<
                variant<int, trivially_moveable>>::value);

  struct non_trivially_move_constructible {
    non_trivially_move_constructible(const non_trivially_move_constructible&) =
        default;
    non_trivially_move_constructible(non_trivially_move_constructible&&) { }
    non_trivially_move_constructible&
    operator=(const non_trivially_move_constructible&) = default;
    non_trivially_move_constructible&
    operator=(non_trivially_move_constructible&&) = default;
    ~non_trivially_move_constructible() = default;
  };
  static_assert(!std::is_trivially_move_assignable<
                variant<int, non_trivially_move_constructible>>::value);
  static_assert(std::is_move_assignable<
                variant<int, non_trivially_move_constructible>>::value);

  struct non_trivially_move_assignable {
    non_trivially_move_assignable(const non_trivially_move_assignable&) =
        default;
    non_trivially_move_assignable(non_trivially_move_assignable&&) = default;
    non_trivially_move_assignable&
    operator=(const non_trivially_move_assignable&) = default;
    non_trivially_move_assignable& operator=(non_trivially_move_assignable&&) {
      return *this;
    }
    ~non_trivially_move_assignable() = default;
  };
  static_assert(!std::is_trivially_move_assignable<
                variant<int, non_trivially_move_assignable>>::value);
  static_assert(std::is_move_assignable<
                variant<int, non_trivially_move_assignable>>::value);

  struct non_trivially_destructible {
    non_trivially_destructible(const non_trivially_destructible&) = default;
    non_trivially_destructible(non_trivially_destructible&&) = default;
    non_trivially_destructible&
    operator=(const non_trivially_destructible&) = default;
    non_trivially_destructible&
    operator=(non_trivially_destructible&&) = default;
    ~non_trivially_destructible() { }
  };
  static_assert(!std::is_trivially_move_assignable<
                variant<int, non_trivially_destructible>>::value);
  static_assert(
      std::is_move_assignable<variant<int, non_trivially_destructible>>::value);

  struct trivially_move_assignable_non_trivially_copyable {
    trivially_move_assignable_non_trivially_copyable(
        const trivially_move_assignable_non_trivially_copyable&) { }
    trivially_move_assignable_non_trivially_copyable(
        trivially_move_assignable_non_trivially_copyable&&) = default;
    trivially_move_assignable_non_trivially_copyable&
    operator=(const trivially_move_assignable_non_trivially_copyable&) {
      return *this;
    }
    trivially_move_assignable_non_trivially_copyable&
    operator=(trivially_move_assignable_non_trivially_copyable&&) = default;
  };
  static_assert(
      std::is_trivially_move_assignable<variant<
          int, trivially_move_assignable_non_trivially_copyable>>::value);
}

TEST(VariantTestMoveAssignment, Noexcept) {
  static_assert(std::is_nothrow_move_assignable<variant<int>>::value);
  static_assert(std::is_nothrow_move_assignable<variant<int, double>>::value);
  static_assert(std::is_nothrow_move_assignable<variant<monostate>>::value);
  static_assert(
      std::is_nothrow_move_assignable<variant<int, monostate>>::value);

  struct nothrow_moveable {
    nothrow_moveable(const nothrow_moveable&) = default;
    nothrow_moveable(nothrow_moveable&&) noexcept = default;
    nothrow_moveable& operator=(const nothrow_moveable&) = default;
    nothrow_moveable& operator=(nothrow_moveable&&) noexcept = default;
  };
  static_assert(
      std::is_nothrow_move_assignable<variant<int, nothrow_moveable>>::value);

  struct throw_move_constructible {
    throw_move_constructible(const throw_move_constructible&) = default;
    throw_move_constructible(throw_move_constructible&&);
    throw_move_constructible&
    operator=(const throw_move_constructible&) = default;
    throw_move_constructible&
    operator=(throw_move_constructible&&) noexcept = default;
  };
  static_assert(!std::is_nothrow_move_assignable<
                variant<int, throw_move_constructible>>::value);

  struct throw_move_assignable {
    throw_move_assignable(const throw_move_assignable&) = default;
    throw_move_assignable(throw_move_assignable&&) noexcept = default;
    throw_move_assignable& operator=(const throw_move_assignable&) = default;
    throw_move_assignable& operator=(throw_move_assignable&&);
  };
  static_assert(!std::is_nothrow_move_assignable<
                variant<int, throw_move_assignable>>::value);

  struct throw_moveable {
    throw_moveable(const throw_moveable&) = default;
    throw_moveable(throw_moveable&&);
    throw_moveable& operator=(const throw_moveable&) = default;
    throw_moveable& operator=(throw_moveable&&);
  };
  static_assert(
      !std::is_nothrow_move_assignable<variant<int, throw_moveable>>::value);
}

TEST(VariantTestMoveAssignment, BasicBehavior) {
  {
    using v = variant<int, valueless_t>;
    static_assert(!variant_no_valueless_state<v>::value);
    v x1, x2;
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    make_valueless(x1);
    make_valueless(x2);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
  }
  {
    using v = variant<counter, valueless_t>;
    static_assert(!variant_no_valueless_state<v>::value);
    v x1, x2;
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(counter::alive_count, 2);

    make_valueless(x2);
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
    EXPECT_EQ(counter::alive_count, 0);
    counter::reset();
  }
  {
    using v = variant<counter, valueless_t>;
    static_assert(!variant_no_valueless_state<v>::value);
    v x1, x2;
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(counter::alive_count, 2);

    make_valueless(x1);
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_TRUE(holds_alternative<counter>(x1));
    EXPECT_TRUE(holds_alternative<counter>(x2));
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::move_construct_count, 1);
    counter::reset();
  }
  {
    using v = variant<int>;
    v x1(2), x2(3);
    EXPECT_EQ(get<0>(x1), 2);
    EXPECT_EQ(get<0>(x2), 3);
    x1 = std::move(x2);
    EXPECT_EQ(get<0>(x1), 3);
    EXPECT_EQ(get<0>(x2), 3);
  }
  {
    using v = variant<counter>;
    v x1, x2;
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::move_assign_count, 0);
    x1 = std::move(x2);
    EXPECT_EQ(counter::move_assign_count, 1);
    counter::reset();
  }
  {
    using v = variant<int, long>;
    v x1(std::in_place_index<1>, 3);
    v x2(std::in_place_index<1>, 4);
    EXPECT_EQ(get<1>(x1), 3);
    EXPECT_EQ(get<1>(x2), 4);

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(get<1>(x1), 4);
    EXPECT_EQ(get<1>(x2), 4);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x1));
    EXPECT_TRUE(holds_alternative<long>(x2));
  }
  {
    using v = variant<int, std::string>;
    v x1(std::in_place_index<1>, "abc");
    v x2(std::in_place_index<1>, "bcd");
    EXPECT_EQ(get<1>(x1), "abc");
    EXPECT_EQ(get<1>(x2), "bcd");

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(get<1>(x1), "bcd");
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<std::string>(x1));
    EXPECT_TRUE(holds_alternative<std::string>(x2));
  }
  {
    using v = variant<int, counter>;
    v x1(std::in_place_index<1>);
    v x2(std::in_place_index<1>);
    EXPECT_EQ(counter::copy_assign_count, 0);
    EXPECT_EQ(counter::move_assign_count, 0);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_EQ(counter::copy_assign_count, 0);
    EXPECT_EQ(counter::move_assign_count, 1);
    counter::reset();
  }
  {
    using v = variant<int, valueless_t>;
    static_assert(!variant_no_valueless_state<v>::value);
    v x1(std::in_place_index<1>);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());

    EXPECT_THROW((x1 = std::move(x2)), int);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
  }
  {
    using v = variant<int, counter>;
    v x1(std::in_place_index<1>);
    v x2(std::in_place_index<0>, 4);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_EQ(counter::alive_count, 1);

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_EQ(get<0>(x1), 4);
    EXPECT_EQ(get<0>(x2), 4);
    EXPECT_EQ(counter::alive_count, 0);
    counter::reset();
  }
  {
    struct nothrow_move_constructible {
      bool copy_ctor = false;
      bool move_ctor = false;

      nothrow_move_constructible() = default;
      nothrow_move_constructible(const nothrow_move_constructible&) {
        copy_ctor = true;
      }
      nothrow_move_constructible(nothrow_move_constructible&&) noexcept {
        move_ctor = true;
      }
      nothrow_move_constructible&
      operator=(const nothrow_move_constructible&) = default;
      nothrow_move_constructible&
      operator=(nothrow_move_constructible&&) = default;
    };

    using v = variant<int, nothrow_move_constructible>;
    static_assert(variant_no_valueless_state<v>::value);
    v x1(std::in_place_index<0>);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    v& ref = (x1 = x2);
    EXPECT_EQ(&ref, &x1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_FALSE(get<1>(x1).copy_ctor);
    EXPECT_TRUE(get<1>(x1).move_ctor);
  }
}

template <class V1, class V2, class Ty>
constexpr bool constexpr_move_assign_impl(V1&& lhs, V2&& rhs,
                                          std::size_t expected_index,
                                          Ty expected_value) {
  lhs = std::move(rhs);
  return lhs.index() == expected_index && get<Ty>(lhs) == expected_value;
}

TEST(VariantTestMoveAssignment, Constexpr) {
  using v = variant<long, void*, int>;
  static_assert(std::is_move_assignable<v>::value);
  static_assert(constexpr_move_assign_impl(v(3), v(4), 2, 4));
  static_assert(constexpr_move_assign_impl(v(3l), v(4l), 0, 4l));
  static_assert(constexpr_move_assign_impl(v(nullptr), v(4l), 0, 4l));
  static_assert(constexpr_move_assign_impl(v(3l), v(nullptr), 1,
                                           static_cast<void*>(nullptr)));
  static_assert(constexpr_move_assign_impl(v(3l), v(4), 2, 4));
  static_assert(constexpr_move_assign_impl(v(3), v(4l), 0, 4l));
}

TEST(VariantTestMoveAssignment, ValuelessStrategy) {
  {
    using v = variant<int, may_throw_move_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_FALSE(get<1>(x1).copy_ctor);
    EXPECT_TRUE(get<1>(x1).move_ctor);
  }
  {
    using v = variant<int, throw_move_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = std::move(x2), int);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x2.index(), 1);
  }
  {
    using v = variant<int, may_throw_move_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_FALSE(get<1>(x1).copy_ctor);
    EXPECT_TRUE(get<1>(x1).move_ctor);
  }
  {
    using v = variant<int, throw_move_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(get<0>(x1), 4);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = std::move(x2), int);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_EQ(get<0>(x1), 0);
  }
  {
    using v = variant<int, may_throw_move_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    v& ref = (x1 = std::move(x2));
    EXPECT_EQ(&ref, &x1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_FALSE(get<1>(x1).copy_ctor);
    EXPECT_TRUE(get<1>(x1).move_ctor);
  }
  {
    using v = variant<int, throw_move_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = std::move(x2), int);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x2.index(), 1);
  }
}
}  // namespace
}  // namespace rust
