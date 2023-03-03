#include <bitset>
#include <functional>
#include <gtest/gtest.h>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <rust_enum/variant.hpp>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace rust {
TEST(VariantPublicInterfaceTest, BadVariantAccessException) {
  static_assert(
      std::is_nothrow_default_constructible<bad_variant_access>::value);
  static_assert(std::is_base_of<std::exception, bad_variant_access>::value);
  static_assert(std::is_nothrow_copy_constructible<bad_variant_access>::value);
  static_assert(std::is_nothrow_copy_assignable<bad_variant_access>::value);
  static_assert(noexcept(bad_variant_access {}.what()));

  const bad_variant_access e;
  EXPECT_TRUE(e.what());
}

template <class Variant, std::size_t Size>
void variant_size_test_helper() {
  static_assert(variant_size<Variant>::value == Size);
  static_assert(variant_size<const Variant>::value == Size);
  static_assert(variant_size<volatile Variant>::value == Size);
  static_assert(variant_size<const volatile Variant>::value == Size);
  static_assert(variant_size_v<Variant> == Size);
  static_assert(variant_size_v<const Variant> == Size);
  static_assert(variant_size_v<volatile Variant> == Size);
  static_assert(variant_size_v<const volatile Variant> == Size);
}

TEST(VariantPublicInterfaceTest, VariantSize) {
  variant_size_test_helper<variant<>, 0>();
  variant_size_test_helper<variant<int>, 1>();
  variant_size_test_helper<variant<float, long, double*, const int>, 4>();
}

TEST(VariantPublicInterfaceTest, VariantAlternative) {
  using v = variant<int, const int, const float*, volatile double>;

  static_assert(
      std::is_same<typename variant_alternative<0, v>::type, int>::value);
  static_assert(std::is_same<typename variant_alternative<0, const v>::type,
                             const int>::value);
  static_assert(std::is_same<typename variant_alternative<0, volatile v>::type,
                             volatile int>::value);
  static_assert(
      std::is_same<typename variant_alternative<0, const volatile v>::type,
                   const volatile int>::value);

  static_assert(
      std::is_same<typename variant_alternative<1, v>::type, const int>::value);
  static_assert(std::is_same<typename variant_alternative<1, const v>::type,
                             const int>::value);
  static_assert(std::is_same<typename variant_alternative<1, volatile v>::type,
                             const volatile int>::value);
  static_assert(
      std::is_same<typename variant_alternative<1, const volatile v>::type,
                   const volatile int>::value);

  static_assert(std::is_same<typename variant_alternative<2, v>::type,
                             const float*>::value);
  static_assert(std::is_same<typename variant_alternative<2, const v>::type,
                             const float* const>::value);
  static_assert(std::is_same<typename variant_alternative<2, volatile v>::type,
                             const float* volatile>::value);
  static_assert(
      std::is_same<typename variant_alternative<2, const volatile v>::type,
                   const float* const volatile>::value);

  static_assert(std::is_same<typename variant_alternative<3, v>::type,
                             volatile double>::value);
  static_assert(std::is_same<typename variant_alternative<3, const v>::type,
                             const volatile double>::value);
  static_assert(std::is_same<typename variant_alternative<3, volatile v>::type,
                             volatile double>::value);
  static_assert(
      std::is_same<typename variant_alternative<3, const volatile v>::type,
                   const volatile double>::value);
}

TEST(VariantPublicInterfaceTest, Monostate) {
  using m = monostate;

  static_assert(std::is_trivially_default_constructible<m>::value);
  static_assert(std::is_trivially_copy_constructible<m>::value);
  static_assert(std::is_trivially_copy_assignable<m>::value);
  static_assert(std::is_trivially_destructible<m>::value);

  constexpr m _m [[maybe_unused]] {};
}

TEST(VariantPublicInterfaceTest, CompareMonostate) {
  // constexpr
  {
    constexpr monostate m1, m2;

    static_assert(m1 == m2);
    static_assert(!(m1 != m2));
    static_assert(!(m1 < m2));
    static_assert(!(m1 > m2));
    static_assert(m1 <= m2);
    static_assert(m1 >= m2);

    static_assert(noexcept(m1 == m2));
    static_assert(noexcept(m1 != m2));
    static_assert(noexcept(m1 < m2));
    static_assert(noexcept(m1 > m2));
    static_assert(noexcept(m1 <= m2));
    static_assert(noexcept(m1 >= m2));
  }

  // non-constexpr
  {
    monostate m1, m2;

    EXPECT_EQ(m1, m2);
    EXPECT_FALSE(m1 != m2);
    EXPECT_FALSE(m1 < m2);
    EXPECT_FALSE(m1 > m2);
    EXPECT_LE(m1, m2);
    EXPECT_GE(m1, m2);

    static_assert(noexcept(m1 == m2));
    static_assert(noexcept(m1 != m2));
    static_assert(noexcept(m1 < m2));
    static_assert(noexcept(m1 > m2));
    static_assert(noexcept(m1 <= m2));
    static_assert(noexcept(m1 >= m2));
  }
}

TEST(VariantPublicInterfaceTest, IndexGetIf) {
  // constexpr
  {
    constexpr variant<int>* v = nullptr;
    static_assert(get_if<0>(v) == nullptr);
    static_assert(noexcept(get_if<0>(v)));
    static_assert(std::is_same<decltype(get_if<0>(v)), int*>::value);
  }
  {
    constexpr variant<int, const long> v(3);
    static_assert(*get_if<0>(&v) == 3);
    static_assert(noexcept(get_if<0>(&v)));
    static_assert(std::is_same<decltype(get_if<0>(&v)), const int*>::value);
    static_assert(get_if<1>(&v) == nullptr);
    static_assert(noexcept(get_if<1>(&v)));
    static_assert(std::is_same<decltype(get_if<1>(&v)), const long*>::value);
  }
  {
    constexpr variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(*get_if<1>(&v) == 3);
    static_assert(noexcept(get_if<1>(&v)));
    static_assert(std::is_same<decltype(get_if<1>(&v)), const long*>::value);
    static_assert(get_if<0>(&v) == nullptr);
    static_assert(noexcept(get_if<0>(&v)));
    static_assert(std::is_same<decltype(get_if<0>(&v)), const int*>::value);
  }

  // non-const
  {
    variant<int>* v = nullptr;
    EXPECT_EQ(get_if<0>(v), nullptr);
    static_assert(noexcept(get_if<0>(v)));
    static_assert(std::is_same<decltype(get_if<0>(v)), int*>::value);
  }
  {
    variant<int, const long> v(3);
    EXPECT_EQ(*get_if<0>(&v), 3);
    static_assert(noexcept(get_if<0>(&v)));
    static_assert(std::is_same<decltype(get_if<0>(&v)), int*>::value);
    EXPECT_EQ(get_if<1>(&v), nullptr);
    static_assert(noexcept(get_if<1>(&v)));
    static_assert(std::is_same<decltype(get_if<1>(&v)), const long*>::value);
  }
  {
    variant<int, const long> v(std::in_place_index<1>, 3);
    EXPECT_EQ(*get_if<1>(&v), 3);
    static_assert(noexcept(get_if<1>(&v)));
    static_assert(std::is_same<decltype(get_if<1>(&v)), const long*>::value);
    EXPECT_EQ(get_if<0>(&v), nullptr);
    static_assert(noexcept(get_if<0>(&v)));
    static_assert(std::is_same<decltype(get_if<0>(&v)), int*>::value);
  }
}

TEST(VariantPublicInterfaceTest, TypeGetIf) {
  // constexpr
  {
    constexpr variant<int>* v = nullptr;
    static_assert(get_if<int>(v) == nullptr);
    static_assert(noexcept(get_if<int>(v)));
    static_assert(std::is_same<decltype(get_if<int>(v)), int*>::value);
  }
  {
    constexpr variant<int, const long> v(3);
    static_assert(*get_if<int>(&v) == 3);
    static_assert(noexcept(get_if<int>(&v)));
    static_assert(std::is_same<decltype(get_if<int>(&v)), const int*>::value);
    static_assert(get_if<const long>(&v) == nullptr);
    static_assert(noexcept(get_if<const long>(&v)));
    static_assert(
        std::is_same<decltype(get_if<const long>(&v)), const long*>::value);
  }
  {
    constexpr variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(*get_if<const long>(&v) == 3);
    static_assert(noexcept(get_if<const long>(&v)));
    static_assert(
        std::is_same<decltype(get_if<const long>(&v)), const long*>::value);
    static_assert(get_if<int>(&v) == nullptr);
    static_assert(noexcept(get_if<int>(&v)));
    static_assert(std::is_same<decltype(get_if<int>(&v)), const int*>::value);
  }

  // non-const
  {
    variant<int>* v = nullptr;
    EXPECT_EQ(get_if<int>(v), nullptr);
    static_assert(noexcept(get_if<int>(v)));
    static_assert(std::is_same<decltype(get_if<int>(v)), int*>::value);
  }
  {
    variant<int, const long> v(3);
    EXPECT_EQ(*get_if<int>(&v), 3);
    static_assert(noexcept(get_if<int>(&v)));
    static_assert(std::is_same<decltype(get_if<int>(&v)), int*>::value);
    EXPECT_EQ(get_if<const long>(&v), nullptr);
    static_assert(noexcept(get_if<const long>(&v)));
    static_assert(
        std::is_same<decltype(get_if<const long>(&v)), const long*>::value);
  }
  {
    variant<int, const long> v(std::in_place_index<1>, 3);
    EXPECT_EQ(*get_if<const long>(&v), 3);
    static_assert(noexcept(get_if<const long>(&v)));
    static_assert(
        std::is_same<decltype(get_if<const long>(&v)), const long*>::value);
    EXPECT_EQ(get_if<int>(&v), nullptr);
    static_assert(noexcept(get_if<int>(&v)));
    static_assert(std::is_same<decltype(get_if<int>(&v)), int*>::value);
  }
}

TEST(VariantPublicInterfaceTest, IndexGet) {
  // constexpr
  {
    constexpr variant<int, const long> v(3);
    static_assert(std::is_same<decltype(get<0>(v)), const int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    static_assert(get<0>(v) == 3);
  }
  {
    constexpr variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<0>(v)), const int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    static_assert(get<1>(v) == 3);
  }

  // const lvalue
  {
    const variant<int, const long, int*> v(3);
    static_assert(std::is_same<decltype(get<0>(v)), const int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    static_assert(std::is_same<decltype(get<2>(v)), int* const&>::value);
    EXPECT_EQ(get<0>(v), 3);
    EXPECT_THROW(get<1>(v), bad_variant_access);
    EXPECT_THROW(get<2>(v), bad_variant_access);
  }
  {
    const variant<int, const long, int*> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<0>(v)), const int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    static_assert(std::is_same<decltype(get<2>(v)), int* const&>::value);
    EXPECT_EQ(get<1>(v), 3);
    EXPECT_THROW(get<0>(v), bad_variant_access);
    EXPECT_THROW(get<2>(v), bad_variant_access);
  }
  {
    const variant<int, const long, int*> v(std::in_place_index<2>, nullptr);
    static_assert(std::is_same<decltype(get<0>(v)), const int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    static_assert(std::is_same<decltype(get<2>(v)), int* const&>::value);
    EXPECT_EQ(get<2>(v), nullptr);
    EXPECT_THROW(get<0>(v), bad_variant_access);
    EXPECT_THROW(get<1>(v), bad_variant_access);
  }

  // non-const lvalue
  {
    variant<int, const long> v(3);
    static_assert(std::is_same<decltype(get<0>(v)), int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    EXPECT_EQ(get<0>(v), 3);
    EXPECT_THROW(get<1>(v), bad_variant_access);
    get<0>(v) = 5;
    EXPECT_EQ(get<0>(v), 5);
    EXPECT_THROW(get<1>(v), bad_variant_access);
  }
  {
    variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<0>(v)), int&>::value);
    static_assert(std::is_same<decltype(get<1>(v)), const long&>::value);
    EXPECT_EQ(get<1>(v), 3);
    EXPECT_THROW(get<0>(v), bad_variant_access);
  }

  // non-const rvalue
  {
    variant<int, const long> v(3);
    static_assert(std::is_same<decltype(get<0>(std::move(v))), int&&>::value);
    static_assert(
        std::is_same<decltype(get<1>(std::move(v))), const long&&>::value);
    EXPECT_EQ(get<0>(std::move(v)), 3);
    EXPECT_THROW(get<1>(std::move(v)), bad_variant_access);
  }
  {
    variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<0>(std::move(v))), int&&>::value);
    static_assert(
        std::is_same<decltype(get<1>(std::move(v))), const long&&>::value);
    EXPECT_EQ(get<1>(std::move(v)), 3);
    EXPECT_THROW(get<0>(std::move(v)), bad_variant_access);
  }

  // const rvalue
  {
    const variant<int, const long> v(3);
    static_assert(
        std::is_same<decltype(get<0>(std::move(v))), const int&&>::value);
    static_assert(
        std::is_same<decltype(get<1>(std::move(v))), const long&&>::value);
    EXPECT_EQ(get<0>(std::move(v)), 3);
    EXPECT_THROW(get<1>(std::move(v)), bad_variant_access);
  }
  {
    const variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(
        std::is_same<decltype(get<0>(std::move(v))), const int&&>::value);
    static_assert(
        std::is_same<decltype(get<1>(std::move(v))), const long&&>::value);
    EXPECT_EQ(get<1>(std::move(v)), 3);
    EXPECT_THROW(get<0>(std::move(v)), bad_variant_access);
  }
}

TEST(VariantPublicInterfaceTest, TypeGet) {
  // constexpr
  {
    constexpr variant<int, const long> v(3);
    static_assert(std::is_same<decltype(get<int>(v)), const int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    static_assert(get<int>(v) == 3);
  }
  {
    constexpr variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<int>(v)), const int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    static_assert(get<const long>(v) == 3);
  }

  // const lvalue
  {
    const variant<int, const long, int*> v(3);
    static_assert(std::is_same<decltype(get<int>(v)), const int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    static_assert(std::is_same<decltype(get<int*>(v)), int* const&>::value);
    EXPECT_EQ(get<int>(v), 3);
    EXPECT_THROW(get<const long>(v), bad_variant_access);
    EXPECT_THROW(get<int*>(v), bad_variant_access);
  }
  {
    const variant<int, const long, int*> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<int>(v)), const int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    static_assert(std::is_same<decltype(get<int*>(v)), int* const&>::value);
    EXPECT_EQ(get<const long>(v), 3);
    EXPECT_THROW(get<int>(v), bad_variant_access);
    EXPECT_THROW(get<int*>(v), bad_variant_access);
  }
  {
    const variant<int, const long, int*> v(std::in_place_index<2>, nullptr);
    static_assert(std::is_same<decltype(get<int>(v)), const int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    static_assert(std::is_same<decltype(get<int*>(v)), int* const&>::value);
    EXPECT_EQ(get<int*>(v), nullptr);
    EXPECT_THROW(get<int>(v), bad_variant_access);
    EXPECT_THROW(get<const long>(v), bad_variant_access);
  }

  // non-const lvalue
  {
    variant<int, const long> v(3);
    static_assert(std::is_same<decltype(get<int>(v)), int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    EXPECT_EQ(get<int>(v), 3);
    EXPECT_THROW(get<const long>(v), bad_variant_access);
    get<int>(v) = 5;
    EXPECT_EQ(get<int>(v), 5);
    EXPECT_THROW(get<const long>(v), bad_variant_access);
  }
  {
    variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<int>(v)), int&>::value);
    static_assert(
        std::is_same<decltype(get<const long>(v)), const long&>::value);
    EXPECT_EQ(get<const long>(v), 3);
    EXPECT_THROW(get<int>(v), bad_variant_access);
  }

  // non-const rvalue
  {
    variant<int, const long> v(3);
    static_assert(std::is_same<decltype(get<int>(std::move(v))), int&&>::value);
    static_assert(std::is_same<decltype(get<const long>(std::move(v))),
                               const long&&>::value);
    EXPECT_EQ(get<int>(std::move(v)), 3);
    EXPECT_THROW(get<const long>(std::move(v)), bad_variant_access);
  }
  {
    variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(std::is_same<decltype(get<int>(std::move(v))), int&&>::value);
    static_assert(std::is_same<decltype(get<const long>(std::move(v))),
                               const long&&>::value);
    EXPECT_EQ(get<const long>(std::move(v)), 3);
    EXPECT_THROW(get<int>(std::move(v)), bad_variant_access);
  }

  // const rvalue
  {
    const variant<int, const long> v(3);
    static_assert(
        std::is_same<decltype(get<int>(std::move(v))), const int&&>::value);
    static_assert(std::is_same<decltype(get<const long>(std::move(v))),
                               const long&&>::value);
    EXPECT_EQ(get<int>(std::move(v)), 3);
    EXPECT_THROW(get<const long>(std::move(v)), bad_variant_access);
  }
  {
    const variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(
        std::is_same<decltype(get<int>(std::move(v))), const int&&>::value);
    static_assert(std::is_same<decltype(get<const long>(std::move(v))),
                               const long&&>::value);
    EXPECT_EQ(get<const long>(std::move(v)), 3);
    EXPECT_THROW(get<int>(std::move(v)), bad_variant_access);
  }
}

TEST(VariantPublicInterfaceTest, HoldsAlternative) {
  {
    constexpr variant<int, const long> v;
    static_assert(holds_alternative<int>(v));
    static_assert(noexcept(holds_alternative<int>(v)));
    static_assert(!holds_alternative<const long>(v));
    static_assert(noexcept(holds_alternative<const long>(v)));
  }
  {
    constexpr variant<int, const long> v(std::in_place_index<1>, 3);
    static_assert(holds_alternative<const long>(v));
    static_assert(noexcept(holds_alternative<const long>(v)));
    static_assert(!holds_alternative<int>(v));
    static_assert(noexcept(holds_alternative<int>(v)));
  }
}

struct valueless_t {
  valueless_t() = default;
  valueless_t(const valueless_t&) { throw 1; }
  valueless_t(valueless_t&&) { throw 2; }
  valueless_t& operator=(const valueless_t&) { throw 3; }
  valueless_t& operator=(valueless_t&&) { throw 4; }
};

bool operator==(const valueless_t&, const valueless_t&) {
  ADD_FAILURE();
  return false;
}

bool operator!=(const valueless_t&, const valueless_t&) {
  ADD_FAILURE();
  return false;
}

bool operator<(const valueless_t&, const valueless_t&) {
  ADD_FAILURE();
  return false;
}

bool operator<=(const valueless_t&, const valueless_t&) {
  ADD_FAILURE();
  return false;
}

bool operator>(const valueless_t&, const valueless_t&) {
  ADD_FAILURE();
  return false;
}

bool operator>=(const valueless_t&, const valueless_t&) {
  ADD_FAILURE();
  return false;
}

template <class Variant>
void make_valueless(Variant& v) {
  Variant valueless(std::in_place_type<valueless_t>);
  EXPECT_THROW((v = valueless), int);
}

struct my_bool {
  bool value;

  constexpr explicit my_bool(bool v) : value(v) { }
  constexpr operator bool() const noexcept { return value; }
};

struct compares_to_my_bool {
  int value = 0;
};

constexpr my_bool operator==(const compares_to_my_bool& lhs,
                             const compares_to_my_bool& rhs) noexcept {
  return my_bool(lhs.value == rhs.value);
}

constexpr my_bool operator!=(const compares_to_my_bool& lhs,
                             const compares_to_my_bool& rhs) noexcept {
  return my_bool(lhs.value != rhs.value);
}

constexpr my_bool operator<(const compares_to_my_bool& lhs,
                            const compares_to_my_bool& rhs) noexcept {
  return my_bool(lhs.value < rhs.value);
}

constexpr my_bool operator<=(const compares_to_my_bool& lhs,
                             const compares_to_my_bool& rhs) noexcept {
  return my_bool(lhs.value <= rhs.value);
}

constexpr my_bool operator>(const compares_to_my_bool& lhs,
                            const compares_to_my_bool& rhs) noexcept {
  return my_bool(lhs.value > rhs.value);
}

constexpr my_bool operator>=(const compares_to_my_bool& lhs,
                             const compares_to_my_bool& rhs) noexcept {
  return my_bool(lhs.value >= rhs.value);
}

template <class T1, class T2>
void test_equality_basic() {
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {42});
    constexpr v x2(std::in_place_index<0>, T1 {42});
    static_assert(x1 == x2);
    static_assert(x2 == x1);
    static_assert(!(x1 != x2));
    static_assert(!(x2 != x1));
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {42});
    constexpr v x2(std::in_place_index<0>, T1 {43});
    static_assert(!(x1 == x2));
    static_assert(!(x2 == x1));
    static_assert(x1 != x2);
    static_assert(x2 != x1);
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {42});
    constexpr v x2(std::in_place_index<1>, T2 {42});
    static_assert(!(x1 == x2));
    static_assert(!(x2 == x1));
    static_assert(x1 != x2);
    static_assert(x2 != x1);
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<1>, T2 {42});
    constexpr v x2(std::in_place_index<1>, T2 {42});
    static_assert(x1 == x2);
    static_assert(x2 == x1);
    static_assert(!(x1 != x2));
    static_assert(!(x2 != x1));
  }
}

TEST(VariantPublicInterfaceTest, Equality) {
  test_equality_basic<int, long>();
  test_equality_basic<compares_to_my_bool, int>();
  test_equality_basic<int, compares_to_my_bool>();
  test_equality_basic<compares_to_my_bool, compares_to_my_bool>();

  {
    using v = variant<int, valueless_t>;
    v x1, x2;
    make_valueless(x2);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
    EXPECT_FALSE(x1 == x2);
    EXPECT_FALSE(x2 == x1);
    EXPECT_TRUE(x1 != x2);
    EXPECT_TRUE(x2 != x1);
  }
  {
    using v = variant<int, valueless_t>;
    v x1, x2;
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_FALSE(x1 == x2);
    EXPECT_FALSE(x2 == x1);
    EXPECT_TRUE(x1 != x2);
    EXPECT_TRUE(x2 != x1);
  }
  using v = variant<int, valueless_t>;
  v x1, x2;
  make_valueless(x1);
  make_valueless(x2);
  EXPECT_TRUE(x1.valueless_by_exception());
  EXPECT_TRUE(x2.valueless_by_exception());
  EXPECT_TRUE(x1 == x2);
  EXPECT_TRUE(x2 == x1);
  EXPECT_FALSE(x1 != x2);
  EXPECT_FALSE(x2 != x1);
}

template <class V>
constexpr bool test_less(const V& l, const V& r, bool expect_less,
                         bool expect_greater) {
  static_assert(std::is_same<decltype(l < r), bool>::value);
  static_assert(std::is_same<decltype(l <= r), bool>::value);
  static_assert(std::is_same<decltype(l > r), bool>::value);
  static_assert(std::is_same<decltype(l >= r), bool>::value);

  return ((l < r) == expect_less) && (!(l >= r) == expect_less) &&
         ((l > r) == expect_greater) && (!(l <= r) == expect_greater);
}

template <class T1, class T2>
void test_relational_basic() {
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {1});
    constexpr v x2(std::in_place_index<0>, T1 {1});
    static_assert(test_less(x1, x2, false, false));
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {0});
    constexpr v x2(std::in_place_index<0>, T1 {1});
    static_assert(test_less(x1, x2, true, false));
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {1});
    constexpr v x2(std::in_place_index<0>, T1 {0});
    static_assert(test_less(x1, x2, false, true));
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<0>, T1 {0});
    constexpr v x2(std::in_place_index<1>, T2 {0});
    static_assert(test_less(x1, x2, true, false));
  }
  {
    using v = variant<T1, T2>;
    constexpr v x1(std::in_place_index<1>, T2 {0});
    constexpr v x2(std::in_place_index<0>, T1 {0});
    static_assert(test_less(x1, x2, false, true));
  }
}

TEST(VariantPublicInterfaceTest, Relational) {
  test_relational_basic<int, long>();
  test_relational_basic<compares_to_my_bool, int>();
  test_relational_basic<int, compares_to_my_bool>();
  test_relational_basic<compares_to_my_bool, compares_to_my_bool>();

  {
    using v = variant<int, valueless_t>;
    v x1, x2;
    make_valueless(x2);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
    EXPECT_TRUE(test_less(x1, x2, false, true));
  }
  {
    using v = variant<int, valueless_t>;
    v x1, x2;
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_TRUE(test_less(x1, x2, true, false));
  }
  {
    using v = variant<int, valueless_t>;
    v x1, x2;
    make_valueless(x1);
    make_valueless(x2);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_TRUE(x2.valueless_by_exception());
    EXPECT_TRUE(test_less(x1, x2, false, false));
  }
}

TEST(VariantPublicInterfaceTest, ConvertingAssignment) {
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

  static_assert(
      !std::is_assignable<variant<int, bool>, decltype("meow")>::value);
  static_assert(
      !std::is_assignable<variant<int, const bool>, decltype("meow")>::value);
  static_assert(!std::is_assignable<variant<int, const volatile bool>,
                                    decltype("meow")>::value);

  static_assert(std::is_assignable<variant<bool>, std::true_type>::value);
  static_assert(
      !std::is_assignable<variant<bool>, std::unique_ptr<char>>::value);

  static_assert(!std::is_assignable<variant<bool>, decltype(nullptr)>::value);
#endif
}

TEST(VariantPublicInterfaceTest, ConvertingConstructor) {
  // Test cases from MSVC STL.
  static_assert(std::is_constructible<variant<int>, int>::value);
  static_assert(!std::is_constructible<variant<int, int>, int>::value);
  static_assert(!std::is_constructible<variant<long, long long>, int>::value);
  static_assert(
      std::is_constructible<variant<float, long, double>, int>::value);

#ifdef USE_CXX20
  static_assert(!std::is_constructible<variant<char>, int>::value);

  static_assert(
      !std::is_constructible<variant<std::string, float>, int>::value);
  static_assert(
      !std::is_constructible<variant<std::string, double>, int>::value);
  static_assert(!std::is_constructible<variant<std::string, bool>, int>::value);

  static_assert(
      !std::is_constructible<variant<int, bool>, decltype("meow")>::value);
  static_assert(!std::is_constructible<variant<int, const bool>,
                                       decltype("meow")>::value);
  static_assert(!std::is_constructible<variant<int, const volatile bool>,
                                       decltype("meow")>::value);

  static_assert(std::is_constructible<variant<bool>, std::true_type>::value);
  static_assert(
      !std::is_constructible<variant<bool>, std::unique_ptr<char>>::value);

  static_assert(
      !std::is_constructible<variant<bool>, decltype(nullptr)>::value);
#endif
}

TEST(VariantPublicInterfaceTest, ConvertingP0608R3) {
  // Test cases from P0608R3
  {
    using v = variant<std::string, bool>;
    static_assert(std::is_constructible<v, const char*>::value);
    static_assert(std::is_assignable<v, const char*>::value);
    static_assert(std::is_constructible<v, const char[4]>::value);
    static_assert(std::is_assignable<v, const char[4]>::value);
    static_assert(std::is_constructible<v, std::string>::value);
    static_assert(std::is_assignable<v, std::string>::value);

    v x1 = "abc";
    EXPECT_EQ(x1.index(), 0);
    EXPECT_TRUE(holds_alternative<std::string>(x1));
    EXPECT_FALSE(holds_alternative<bool>(x1));
    EXPECT_EQ(get<0>(x1), "abc");
    EXPECT_EQ(get<std::string>(x1), "abc");
    EXPECT_THROW(get<1>(x1), bad_variant_access);
    EXPECT_THROW(get<bool>(x1), bad_variant_access);

    v x2(std::in_place_index<1>);
    EXPECT_EQ(x2.index(), 1);
    x2 = "abc";
    EXPECT_EQ(x2.index(), 0);
    EXPECT_TRUE(holds_alternative<std::string>(x2));
    EXPECT_FALSE(holds_alternative<bool>(x2));
    EXPECT_EQ(get<0>(x2), "abc");
    EXPECT_EQ(get<std::string>(x2), "abc");
    EXPECT_THROW(get<1>(x2), bad_variant_access);
    EXPECT_THROW(get<bool>(x2), bad_variant_access);
  }
  {
    using v = variant<char, std::optional<char16_t>>;
    static_assert(std::is_constructible<v, char16_t>::value);
    static_assert(std::is_assignable<v, char16_t>::value);

    v x1 = u'\u2043';
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<std::optional<char16_t>>(x1));
    EXPECT_THROW(get<0>(x1), bad_variant_access);
    EXPECT_THROW(get<char>(x1), bad_variant_access);
    EXPECT_EQ(get<1>(x1), u'\u2043');
    EXPECT_EQ(get<std::optional<char16_t>>(x1), u'\u2043');
  }
  {
    using v = variant<char, std::optional<char16_t>>;
    static_assert(std::is_constructible<v, char16_t>::value);
    static_assert(std::is_assignable<v, char16_t>::value);

    v x1 = u'\u2043';
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<std::optional<char16_t>>(x1));
    EXPECT_THROW(get<0>(x1), bad_variant_access);
    EXPECT_THROW(get<char>(x1), bad_variant_access);
    EXPECT_EQ(get<1>(x1), u'\u2043');
    EXPECT_EQ(get<std::optional<char16_t>>(x1), u'\u2043');

    v x2(std::in_place_index<0>);
    EXPECT_EQ(x2.index(), 0);
    x2 = u'\u2043';
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<std::optional<char16_t>>(x2));
    EXPECT_THROW(get<0>(x2), bad_variant_access);
    EXPECT_THROW(get<char>(x2), bad_variant_access);
    EXPECT_EQ(get<1>(x2), u'\u2043');
    EXPECT_EQ(get<std::optional<char16_t>>(x2), u'\u2043');
  }
  {
    using v = variant<int, std::reference_wrapper<double>>;
    static_assert(std::is_constructible<v, double&>::value);
    static_assert(std::is_assignable<v, double&>::value);

    double d = 3.14;
    v y1 = d;
    EXPECT_EQ(y1.index(), 1);
    EXPECT_TRUE(holds_alternative<std::reference_wrapper<double>>(y1));
    EXPECT_EQ(get<1>(y1), d);
    EXPECT_EQ(get<std::reference_wrapper<double>>(y1), d);

    v y2;
    EXPECT_EQ(y2.index(), 0);
    y2 = d;
    EXPECT_EQ(y2.index(), 1);
    EXPECT_TRUE(holds_alternative<std::reference_wrapper<double>>(y2));
    EXPECT_EQ(get<1>(y2), d);
    EXPECT_EQ(get<std::reference_wrapper<double>>(y2), d);
  }
  {
    using v = variant<float, int>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = 0;
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<int>(x));
  }
  {
    using v = variant<float, long>;
    static_assert(std::is_constructible<v, int>::value);
    static_assert(std::is_assignable<v, int>::value);

    v x;
    EXPECT_EQ(x.index(), 0);
    x = 0;
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x));
  }
  {
    static_assert(!std::is_constructible<variant<float>, int>::value);
    static_assert(!std::is_assignable<variant<float>, int>::value);
    static_assert(
        !std::is_constructible<variant<float, std::vector<int>>, int>::value);
    static_assert(
        !std::is_assignable<variant<float, std::vector<int>>, int>::value);
    static_assert(!std::is_constructible<variant<float, char>, int>::value);
    static_assert(!std::is_assignable<variant<float, char>, int>::value);
  }
  {
    using v = variant<float, int>;
    static_assert(std::is_constructible<v, char>::value);
    static_assert(std::is_assignable<v, char>::value);

    v x1 = 'a';
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<int>(x1));
    EXPECT_EQ(get<1>(x1), int('a'));
    EXPECT_EQ(get<int>(x1), int('a'));

    v x2;
    EXPECT_EQ(x2.index(), 0);
    x2 = 'a';
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<int>(x2));
    EXPECT_EQ(get<1>(x2), int('a'));
    EXPECT_EQ(get<int>(x2), int('a'));
  }
  {
    using v = variant<float, long>;
    static_assert(std::is_constructible<v, int>::value);
    static_assert(std::is_assignable<v, int>::value);

    v x1 = 0;
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x1));

    v x2;
    EXPECT_EQ(x2.index(), 0);
    x2 = 0;
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x2));
  }
  {
    using v = variant<float, long, double>;
    static_assert(std::is_constructible<v, int>::value);
    static_assert(std::is_assignable<v, int>::value);

    v x1 = 0;
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x1));

    v x2;
    EXPECT_EQ(x2.index(), 0);
    x2 = 0;
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x2));
  }
}

TEST(VariantPublicInterfaceTest, ConvertingP1957R2) {
  // Test cases from P1957R2 and MSVC STL
  static_assert(std::is_constructible<variant<bool, int>, bool>::value);
  static_assert(std::is_constructible<variant<bool, int>,
                                      std::bitset<4>::reference>::value);
  static_assert(
      std::is_constructible<variant<bool>, std::bitset<4>::reference>::value);

  static_assert(std::is_assignable<variant<bool, int>, bool>::value);
  static_assert(
      std::is_assignable<variant<bool, int>, std::bitset<4>::reference>::value);
  static_assert(
      std::is_assignable<variant<bool>, std::bitset<4>::reference>::value);

  std::bitset<4> a_bitset("0101");
  bool a_data = a_bitset[2];
  variant<bool, int> a = a_data;  // bool
  EXPECT_EQ(a.index(), 0);
  EXPECT_TRUE(get<0>(a));

  std::bitset<4> b_bitset("0101");
  variant<bool, int> b = b_bitset[2];   // bool
  variant<bool, int> b2 = b_bitset[1];  // bool
  EXPECT_EQ(b.index(), 0);
  EXPECT_TRUE(get<0>(b));
  EXPECT_EQ(b2.index(), 0);
  EXPECT_FALSE(get<0>(b2));
}

struct double_double {
  double x;

  double_double(double x) : x(x) { }
};

struct convertible_bool {
  convertible_bool(bool x) : x(x) { }
  ~convertible_bool() = default;

  operator bool() const noexcept { return x; }

  bool x;
};

struct default_struct { };

TEST(VariantPublicInterfaceTest, ConvertingMoreCases) {
  // More cases from MSVC STL.
  static_assert(std::is_constructible<variant<double_double>, double>::value);
  static_assert(std::is_constructible<
                variant<std::vector<std::vector<int>>, std::optional<int>, int>,
                int>::value);
  static_assert(std::is_constructible<
                variant<std::vector<std::vector<int>>, std::optional<int>>,
                int>::value);
  static_assert(
      std::is_constructible<
          variant<std::vector<int>, std::optional<int>, float>, int>::value);
  static_assert(std::is_constructible<variant<bool>, convertible_bool>::value);
  static_assert(
      std::is_constructible<variant<bool, int>, convertible_bool>::value);
  static_assert(std::is_constructible<variant<convertible_bool>, bool>::value);
  static_assert(std::is_constructible<variant<float, bool, convertible_bool>,
                                      convertible_bool>::value);
  static_assert(std::is_constructible<variant<float, bool, convertible_bool>,
                                      bool>::value);
  static_assert(std::is_constructible<variant<char, int>, bool>::value);
  static_assert(std::is_constructible<variant<double_double>, int>::value);
  static_assert(!std::is_constructible<variant<float>, unsigned int>::value);
  static_assert(
      !std::is_constructible<variant<char, default_struct>, int>::value);
  static_assert(
      !std::is_constructible<variant<float, long, long long>, int>::value);

  static_assert(std::is_assignable<variant<double_double>, double>::value);
  static_assert(std::is_assignable<
                variant<std::vector<std::vector<int>>, std::optional<int>, int>,
                int>::value);
  static_assert(std::is_assignable<
                variant<std::vector<std::vector<int>>, std::optional<int>>,
                int>::value);
  static_assert(
      std::is_assignable<variant<std::vector<int>, std::optional<int>, float>,
                         int>::value);
  static_assert(std::is_assignable<variant<bool>, convertible_bool>::value);
  static_assert(
      std::is_assignable<variant<bool, int>, convertible_bool>::value);
  static_assert(std::is_assignable<variant<convertible_bool>, bool>::value);
  static_assert(std::is_assignable<variant<float, bool, convertible_bool>,
                                   convertible_bool>::value);
  static_assert(
      std::is_assignable<variant<float, bool, convertible_bool>, bool>::value);
  static_assert(std::is_assignable<variant<char, int>, bool>::value);
  static_assert(std::is_assignable<variant<double_double>, int>::value);
  static_assert(!std::is_assignable<variant<float>, unsigned int>::value);
  static_assert(!std::is_assignable<variant<char, default_struct>, int>::value);
  static_assert(
      !std::is_assignable<variant<float, long, long long>, int>::value);

  {
    using v = variant<char, int, float, bool, std::vector<bool>>;
    v x = true;
    EXPECT_EQ(x.index(), 3);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<3>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<char, int, float, bool, std::vector<bool>>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = true;
    EXPECT_EQ(x.index(), 3);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<3>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<bool, int>;
    v x = convertible_bool {true};
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<0>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<int, bool>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = convertible_bool {true};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<1>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<char, int, bool>;
    v x = true;
    EXPECT_EQ(x.index(), 2);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<2>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<char, int, bool>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = true;
    EXPECT_EQ(x.index(), 2);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<2>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<char, int, convertible_bool>;
    v x = convertible_bool {true};
    EXPECT_EQ(x.index(), 2);
    EXPECT_TRUE(holds_alternative<convertible_bool>(x));
    EXPECT_TRUE(get<2>(x));
    EXPECT_TRUE(get<convertible_bool>(x));
  }
  {
    using v = variant<char, int, convertible_bool>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = convertible_bool {true};
    EXPECT_EQ(x.index(), 2);
    EXPECT_TRUE(holds_alternative<convertible_bool>(x));
    EXPECT_TRUE(get<2>(x));
    EXPECT_TRUE(get<convertible_bool>(x));
  }
  {
    using v = variant<float, bool, convertible_bool>;
    v x = bool {};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_FALSE(get<1>(x));
    EXPECT_FALSE(get<bool>(x));
  }
  {
    using v = variant<float, bool, convertible_bool>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = bool {};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_FALSE(get<1>(x));
    EXPECT_FALSE(get<bool>(x));
  }
  {
    using v = variant<float, bool>;
    v x = convertible_bool {false};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_FALSE(get<1>(x));
    EXPECT_FALSE(get<bool>(x));
  }
  {
    using v = variant<float, bool>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = convertible_bool {false};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_FALSE(get<1>(x));
    EXPECT_FALSE(get<bool>(x));
  }
  {
    using v = variant<int, bool>;
    v x = std::true_type {};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<1>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    using v = variant<int, bool>;
    v x;
    EXPECT_EQ(x.index(), 0);
    x = std::true_type {};
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(holds_alternative<bool>(x));
    EXPECT_TRUE(get<1>(x));
    EXPECT_TRUE(get<bool>(x));
  }
  {
    variant<std::string, bool, int> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<std::string>(x), "");
    x = 3;
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<int>(x), 3);
    x = true;
    EXPECT_EQ(x.index(), 1);
    EXPECT_TRUE(get<bool>(x));
  }
  {
    bool data = true;
    variant<bool, int, double_double> x = data;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), data);
    x = 12;
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x), 12);
    x = 12.5;
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x).x, 12.5);
  }
  {
    variant<void*, bool, double_double> x;
    EXPECT_EQ(x.index(), 0);
    x = false;
    EXPECT_EQ(x.index(), 1);
    EXPECT_FALSE(get<1>(x));
    x = 5.12;
    EXPECT_EQ(x.index(), 2);
    EXPECT_EQ(get<2>(x).x, 5.12);
    double_double data = 1.2;
    x = &data;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(static_cast<double_double*>(get<0>(x))->x, 1.2);
  }
}

TEST(VariantPublicInterfaceTest, DeletedCopyAssignment) {
  static_assert(std::is_copy_assignable<variant<int>>::value);
  static_assert(std::is_copy_assignable<variant<int, double>>::value);
  static_assert(!std::is_copy_assignable<variant<const int>>::value);
  static_assert(!std::is_copy_assignable<variant<int, const double>>::value);
  static_assert(std::is_copy_assignable<variant<monostate>>::value);
  static_assert(std::is_copy_assignable<variant<int, monostate>>::value);

  struct copy_only {
    copy_only(const copy_only&) = default;
    copy_only(copy_only&&) = delete;
    copy_only& operator=(const copy_only&) = default;
    copy_only& operator=(copy_only&&) = delete;
  };
  static_assert(std::is_copy_assignable<variant<int, copy_only>>::value);

  struct copy_construct_only {
    copy_construct_only(const copy_construct_only&) = default;
    copy_construct_only(copy_construct_only&&) = delete;
    copy_construct_only& operator=(const copy_construct_only&) = delete;
    copy_construct_only& operator=(copy_construct_only&&) = delete;
  };
  static_assert(
      !std::is_copy_assignable<variant<int, copy_construct_only>>::value);

  struct copy_assign_only {
    copy_assign_only(const copy_assign_only&) = delete;
    copy_assign_only(copy_assign_only&&) = delete;
    copy_assign_only& operator=(const copy_assign_only&) = default;
    copy_assign_only& operator=(copy_assign_only&&) = delete;
  };
  static_assert(
      !std::is_copy_assignable<variant<int, copy_assign_only>>::value);

  struct non_copyable {
    non_copyable(const non_copyable&) = delete;
    non_copyable(non_copyable&&) = default;
    non_copyable& operator=(const non_copyable&) = delete;
    non_copyable& operator=(non_copyable&&) = default;
  };
  static_assert(!std::is_copy_assignable<variant<int, non_copyable>>::value);
}

TEST(VariantPublicInterfaceTest, TriviallyCopyAssignment) {
  static_assert(std::is_trivially_copy_assignable<variant<int>>::value);
  static_assert(std::is_trivially_copy_assignable<variant<int, long>>::value);
  static_assert(std::is_trivially_copy_assignable<variant<monostate>>::value);
  static_assert(
      std::is_trivially_copy_assignable<variant<int, monostate>>::value);

  struct trivially_copyable {
    trivially_copyable(const trivially_copyable&) = default;
    trivially_copyable& operator=(const trivially_copyable&) = default;
  };
  static_assert(std::is_trivially_copy_assignable<
                variant<int, trivially_copyable>>::value);

  struct non_trivially_copy_constructible {
    non_trivially_copy_constructible(const non_trivially_copy_constructible&) {
    }
    non_trivially_copy_constructible(non_trivially_copy_constructible&&) =
        default;
    non_trivially_copy_constructible&
    operator=(const non_trivially_copy_constructible&) = default;
    non_trivially_copy_constructible&
    operator=(non_trivially_copy_constructible&&) = default;
    ~non_trivially_copy_constructible() = default;
  };
  static_assert(!std::is_trivially_copy_assignable<
                variant<int, non_trivially_copy_constructible>>::value);
  static_assert(std::is_copy_assignable<
                variant<int, non_trivially_copy_constructible>>::value);

  struct non_trivially_copy_assignable {
    non_trivially_copy_assignable(const non_trivially_copy_assignable&) =
        default;
    non_trivially_copy_assignable(non_trivially_copy_assignable&&) = default;
    non_trivially_copy_assignable&
    operator=(const non_trivially_copy_assignable&) {
      return *this;
    }
    non_trivially_copy_assignable&
    operator=(non_trivially_copy_assignable&&) = default;
    ~non_trivially_copy_assignable() = default;
  };
  static_assert(!std::is_trivially_copy_assignable<
                variant<int, non_trivially_copy_assignable>>::value);
  static_assert(std::is_copy_assignable<
                variant<int, non_trivially_copy_assignable>>::value);

  struct non_trivially_destructible {
    non_trivially_destructible(const non_trivially_destructible&) = default;
    non_trivially_destructible(non_trivially_destructible&&) = default;
    non_trivially_destructible&
    operator=(const non_trivially_destructible&) = default;
    non_trivially_destructible&
    operator=(non_trivially_destructible&&) = default;
    ~non_trivially_destructible() { }
  };
  static_assert(!std::is_trivially_copy_assignable<
                variant<int, non_trivially_destructible>>::value);
  static_assert(
      std::is_copy_assignable<variant<int, non_trivially_destructible>>::value);

  struct trivially_copy_assignable_non_trivially_moveable {
    trivially_copy_assignable_non_trivially_moveable(
        const trivially_copy_assignable_non_trivially_moveable&) = default;
    trivially_copy_assignable_non_trivially_moveable(
        trivially_copy_assignable_non_trivially_moveable&&) { }
    trivially_copy_assignable_non_trivially_moveable& operator=(
        const trivially_copy_assignable_non_trivially_moveable&) = default;
    trivially_copy_assignable_non_trivially_moveable&
    operator=(trivially_copy_assignable_non_trivially_moveable&&) {
      return *this;
    }
  };
  static_assert(
      std::is_trivially_copy_assignable<variant<
          int, trivially_copy_assignable_non_trivially_moveable>>::value);
}

TEST(VariantPublicInterfaceTest, NoexceptCopyAssignment) {
  static_assert(std::is_nothrow_copy_assignable<variant<int>>::value);
  static_assert(std::is_nothrow_copy_assignable<variant<int, double>>::value);
  static_assert(std::is_nothrow_copy_assignable<variant<monostate>>::value);
  static_assert(
      std::is_nothrow_copy_assignable<variant<int, monostate>>::value);

  struct nothrow_copyable {
    nothrow_copyable(const nothrow_copyable&) noexcept = default;
    nothrow_copyable(nothrow_copyable&&) = default;
    nothrow_copyable& operator=(const nothrow_copyable&) noexcept = default;
    nothrow_copyable& operator=(nothrow_copyable&&) = default;
  };
  static_assert(
      std::is_nothrow_copy_assignable<variant<int, nothrow_copyable>>::value);

  struct throw_copy_constructible {
    throw_copy_constructible(const throw_copy_constructible&);
    throw_copy_constructible&
    operator=(const throw_copy_constructible&) noexcept = default;
  };
  static_assert(!std::is_nothrow_copy_assignable<
                variant<int, throw_copy_constructible>>::value);

  struct throw_copy_assignable {
    throw_copy_assignable(const throw_copy_assignable&) noexcept = default;
    throw_copy_assignable& operator=(const throw_copy_assignable&);
  };
  static_assert(!std::is_nothrow_copy_assignable<
                variant<int, throw_copy_assignable>>::value);

  struct throw_copyable {
    throw_copyable(const throw_copyable&);
    throw_copyable& operator=(const throw_copyable&);
  };
  static_assert(
      !std::is_nothrow_copy_assignable<variant<int, throw_copyable>>::value);
}

struct counter {
  static std::size_t alive_count, copy_construct_count, move_construct_count,
      copy_assign_count, move_assign_count;

  counter() { ++alive_count; }
  counter(const counter&) : counter() { ++copy_construct_count; }
  counter(counter&&) : counter() { ++move_construct_count; }
  counter& operator=(const counter&) {
    ++copy_assign_count;
    return *this;
  }
  counter& operator=(counter&&) {
    ++move_assign_count;
    return *this;
  }
  ~counter() { --alive_count; }

  static void reset() {
    copy_construct_count = 0;
    move_construct_count = 0;
    copy_assign_count = 0;
    move_assign_count = 0;
  }
};
std::size_t counter::alive_count = 0, counter::copy_construct_count = 0,
            counter::move_construct_count = 0, counter::copy_assign_count = 0,
            counter::move_assign_count = 0;

TEST(VariantPublicInterfaceTest, CopyAssignmentBehavior) {
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
    v& ref = (x1 = x2);
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

    v& ref = (x1 = x2);
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

    v& ref = (x1 = x2);
    EXPECT_EQ(&ref, &x1);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_TRUE(holds_alternative<counter>(x1));
    EXPECT_TRUE(holds_alternative<counter>(x2));
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_construct_count, 1);
    counter::reset();
  }
  {
    using v = variant<int>;
    v x1(2), x2(3);
    EXPECT_EQ(get<0>(x1), 2);
    EXPECT_EQ(get<0>(x2), 3);
    x1 = x2;
    EXPECT_EQ(get<0>(x1), 3);
    EXPECT_EQ(get<0>(x2), 3);
  }
  {
    using v = variant<counter>;
    v x1, x2;
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_assign_count, 0);
    x1 = x2;
    EXPECT_EQ(counter::copy_assign_count, 1);
    counter::reset();
  }
  {
    using v = variant<int, long>;
    v x1(std::in_place_index<1>, 3);
    v x2(std::in_place_index<1>, 4);
    EXPECT_EQ(get<1>(x1), 3);
    EXPECT_EQ(get<1>(x2), 4);

    v& ref = (x1 = x2);
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

    v& ref = (x1 = x2);
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(get<1>(x1), "bcd");
    EXPECT_EQ(get<1>(x2), "bcd");
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

    v& ref = (x1 = x2);
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_EQ(counter::copy_assign_count, 1);
    EXPECT_EQ(counter::move_assign_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, valueless_t>;
    static_assert(!variant_no_valueless_state<v>::value);
    v x1(std::in_place_index<1>);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());

    EXPECT_THROW((x1 = x2), int);
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

    v& ref = (x1 = x2);
    EXPECT_EQ(&ref, &x1);
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_EQ(get<0>(x1), 4);
    EXPECT_EQ(get<0>(x2), 4);
    EXPECT_EQ(counter::alive_count, 0);
    counter::reset();
  }
  {
    struct nothrow_copy_constructible {
      bool copy_ctor = false;
      bool move_ctor = false;

      nothrow_copy_constructible() = default;
      nothrow_copy_constructible(const nothrow_copy_constructible&) noexcept {
        copy_ctor = true;
      }
      nothrow_copy_constructible(nothrow_copy_constructible&&) {
        move_ctor = true;
      }
      nothrow_copy_constructible&
      operator=(const nothrow_copy_constructible&) = default;
      nothrow_copy_constructible&
      operator=(nothrow_copy_constructible&&) = default;
    };

    struct throw_copy_constructible {
      throw_copy_constructible() = default;
      throw_copy_constructible(const throw_copy_constructible&) { }
    };

    using v = variant<throw_copy_constructible, nothrow_copy_constructible>;
    static_assert(!variant_no_valueless_state<v>::value);
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
    EXPECT_TRUE(get<1>(x1).copy_ctor);
    EXPECT_FALSE(get<1>(x1).move_ctor);
  }
  {
    struct may_throw_copy_constructible {
      bool copy_ctor = false;
      bool move_ctor = false;

      may_throw_copy_constructible() = default;
      may_throw_copy_constructible(const may_throw_copy_constructible&) {
        copy_ctor = true;
      }
      may_throw_copy_constructible(may_throw_copy_constructible&&) noexcept {
        move_ctor = true;
      }
      may_throw_copy_constructible&
      operator=(const may_throw_copy_constructible&) = default;
      may_throw_copy_constructible&
      operator=(may_throw_copy_constructible&&) = default;
    };

    using v = variant<int, may_throw_copy_constructible>;
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
  {
    struct throw_copy_constructible {
      bool move_ctor = false;

      throw_copy_constructible() = default;
      throw_copy_constructible(const throw_copy_constructible&) { throw 1; }
      throw_copy_constructible(throw_copy_constructible&&) noexcept {
        move_ctor = true;
      }
      throw_copy_constructible&
      operator=(const throw_copy_constructible&) = default;
      throw_copy_constructible& operator=(throw_copy_constructible&&) = default;
    };

    using v = variant<int, throw_copy_constructible>;
    static_assert(variant_no_valueless_state<v>::value);
    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = x2, int);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_EQ(get<0>(x1), 4);
  }
}

template <class V1, class V2, class Ty>
constexpr bool constexpr_copy_assign_impl(V1&& lhs, V2&& rhs,
                                          std::size_t expected_index,
                                          Ty expected_value) {
  V2 const _rhs(std::forward<V2>(rhs));
  lhs = _rhs;
  return lhs.index() == expected_index && get<Ty>(lhs) == expected_value;
}

TEST(VariantPublicInterfaceTest, ConstexprCopyAssignment) {
  using v = variant<long, void*, int>;
  static_assert(constexpr_copy_assign_impl(v(3), v(4), 2, 4));
  static_assert(constexpr_copy_assign_impl(v(3l), v(4l), 0, 4l));
  static_assert(constexpr_copy_assign_impl(v(nullptr), v(4l), 0, 4l));
  static_assert(constexpr_copy_assign_impl(v(3l), v(nullptr), 1,
                                           static_cast<void*>(nullptr)));
  static_assert(constexpr_copy_assign_impl(v(3l), v(4), 2, 4));
  static_assert(constexpr_copy_assign_impl(v(3), v(4l), 0, 4l));
}

struct may_throw_copy_constructible {
  bool copy_ctor = false;
  bool move_ctor = false;

  may_throw_copy_constructible() = default;
  may_throw_copy_constructible(const may_throw_copy_constructible&) {
    copy_ctor = true;
  }
  may_throw_copy_constructible(may_throw_copy_constructible&&) {
    move_ctor = true;
  }
  may_throw_copy_constructible&
  operator=(const may_throw_copy_constructible&) = default;
  may_throw_copy_constructible&
  operator=(may_throw_copy_constructible&&) = default;
};

struct throw_copy_constructible {
  bool move_ctor = false;

  throw_copy_constructible() = default;
  throw_copy_constructible(const throw_copy_constructible&) { throw 1; }
  throw_copy_constructible(throw_copy_constructible&&) { move_ctor = true; }
  throw_copy_constructible&
  operator=(const throw_copy_constructible&) = default;
  throw_copy_constructible& operator=(throw_copy_constructible&&) = default;
};

struct may_throw_copy_constructible_strategy_default
    : may_throw_copy_constructible { };
struct throw_copy_constructible_strategy_default : throw_copy_constructible { };

struct may_throw_copy_constructible_strategy_fallback
    : may_throw_copy_constructible { };
struct throw_copy_constructible_strategy_fallback : throw_copy_constructible {
};

template <>
struct variant_valueless_strategy<
    variant<int, may_throw_copy_constructible_strategy_fallback>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::FALLBACK;
};

template <>
struct variant_valueless_strategy<
    variant<int, throw_copy_constructible_strategy_fallback>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::FALLBACK;
};

struct may_throw_copy_constructible_strategy_novalueless
    : may_throw_copy_constructible { };
struct throw_copy_constructible_strategy_novalueless
    : throw_copy_constructible { };

template <>
struct variant_valueless_strategy<
    variant<int, may_throw_copy_constructible_strategy_novalueless>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

template <>
struct variant_valueless_strategy<
    variant<int, throw_copy_constructible_strategy_novalueless>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

TEST(VariantPublicInterfaceTest, CopyAssignmentStrategy) {
  {
    using v = variant<int, may_throw_copy_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
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
    EXPECT_TRUE(get<1>(x1).copy_ctor);
    EXPECT_FALSE(get<1>(x1).move_ctor);
  }
  {
    using v = variant<int, throw_copy_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = x2, int);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x2.index(), 1);
  }
  {
    using v = variant<int, may_throw_copy_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
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
    EXPECT_TRUE(get<1>(x1).copy_ctor);
    EXPECT_FALSE(get<1>(x1).move_ctor);
  }
  {
    using v = variant<int, throw_copy_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = x2, int);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_EQ(get<0>(x1), 0);
  }
  {
    using v = variant<int, may_throw_copy_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
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
    EXPECT_TRUE(get<1>(x1).copy_ctor);
    EXPECT_FALSE(get<1>(x1).move_ctor);
  }
  {
    using v = variant<int, throw_copy_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x1(std::in_place_index<0>, 4);
    v x2(std::in_place_index<1>);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x1.index(), 0);
    EXPECT_EQ(x2.index(), 1);

    EXPECT_THROW(x1 = x2, int);
    EXPECT_FALSE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_EQ(x2.index(), 1);
  }
}

TEST(VariantPublicInterfaceTest, DeletedMoveAssignment) {
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

TEST(VariantPublicInterfaceTest, TriviallyMoveAssignment) {
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

TEST(VariantPublicInterfaceTest, NoexceptMoveAssignment) {
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

TEST(VariantPublicInterfaceTest, MoveAssignmentBehavior) {
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

TEST(VariantPublicInterfaceTest, ConstexprMoveAssignment) {
  using v = variant<long, void*, int>;
  static_assert(constexpr_move_assign_impl(v(3), v(4), 2, 4));
  static_assert(constexpr_move_assign_impl(v(3l), v(4l), 0, 4l));
  static_assert(constexpr_move_assign_impl(v(nullptr), v(4l), 0, 4l));
  static_assert(constexpr_move_assign_impl(v(3l), v(nullptr), 1,
                                           static_cast<void*>(nullptr)));
  static_assert(constexpr_move_assign_impl(v(3l), v(4), 2, 4));
  static_assert(constexpr_move_assign_impl(v(3), v(4l), 0, 4l));
}

struct may_throw_move_constructible : may_throw_copy_constructible { };

struct throw_move_constructible {
  bool copy_ctor = false;

  throw_move_constructible() = default;
  throw_move_constructible(const throw_move_constructible&) {
    copy_ctor = true;
  }
  throw_move_constructible(throw_move_constructible&&) { throw 2; }
  throw_move_constructible&
  operator=(const throw_move_constructible&) = default;
  throw_move_constructible& operator=(throw_move_constructible&&) = default;
};

struct may_throw_move_constructible_strategy_default
    : may_throw_move_constructible { };
struct throw_move_constructible_strategy_default : throw_move_constructible { };

struct may_throw_move_constructible_strategy_fallback
    : may_throw_move_constructible { };
struct throw_move_constructible_strategy_fallback : throw_move_constructible {
};

template <>
struct variant_valueless_strategy<
    variant<int, may_throw_move_constructible_strategy_fallback>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::FALLBACK;
};

template <>
struct variant_valueless_strategy<
    variant<int, throw_move_constructible_strategy_fallback>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::FALLBACK;
};

struct may_throw_move_constructible_strategy_novalueless
    : may_throw_move_constructible { };
struct throw_move_constructible_strategy_novalueless
    : throw_move_constructible { };

template <>
struct variant_valueless_strategy<
    variant<int, may_throw_move_constructible_strategy_novalueless>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

template <>
struct variant_valueless_strategy<
    variant<int, throw_move_constructible_strategy_novalueless>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

TEST(VariantPublicInterfaceTest, MoveAssignmentStrategy) {
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

TEST(VariantPublicInterfaceTest, DeletedConvertingAssignment) {
  static_assert(std::is_assignable<variant<long>, int>::value);
  static_assert(std::is_assignable<variant<monostate>, monostate>::value);
  static_assert(std::is_assignable<variant<int, monostate>, monostate>::value);
  static_assert(!std::is_assignable<variant<const int>, int>::value);
  static_assert(std::is_assignable<variant<int, const double>, int>::value);
  static_assert(!std::is_assignable<variant<int, const double>, double>::value);
  static_assert(!std::is_assignable<variant<long, long long>, int>::value);
  static_assert(!std::is_assignable<variant<std::string, std::string>,
                                    const char*>::value);
  static_assert(!std::is_assignable<variant<std::string, void*>, int>::value);

#ifdef USE_CXX20
  static_assert(!std::is_assignable<variant<std::string, float>, int>::value);
  static_assert(!std::is_assignable<variant<std::unique_ptr<int>, bool>,
                                    std::unique_ptr<char>>::value);
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

TEST(VariantPublicInterfaceTest, NoexceptConvertingAssignment) {
  static_assert(std::is_nothrow_assignable<variant<int>, int>::value);
  static_assert(std::is_nothrow_assignable<variant<long>, int>::value);
  static_assert(
      std::is_nothrow_assignable<variant<monostate>, monostate>::value);
  static_assert(
      std::is_nothrow_assignable<variant<int, monostate>, monostate>::value);
  static_assert(
      std::is_nothrow_assignable<variant<int, const double>, int>::value);

  // Test cases from MSVC STL.
  struct dummy { };
  {
    struct nothrow {
      nothrow(int) noexcept { }
      nothrow& operator=(int) noexcept { return *this; }
    };
    static_assert(
        std::is_nothrow_assignable<variant<dummy, nothrow>, int>::value);
  }
  {
    struct throws_constructor {
      throws_constructor(int) noexcept(false) { }
      throws_constructor& operator=(int) noexcept { return *this; }
    };
    static_assert(
        !std::is_nothrow_assignable<variant<dummy, throws_constructor>,
                                    int>::value);
  }
  {
    struct throws_assignment {
      throws_assignment(int) noexcept { }
      throws_assignment& operator=(int) noexcept(false) { return *this; }
    };
    static_assert(!std::is_nothrow_assignable<variant<dummy, throws_assignment>,
                                              int>::value);
  }
}

TEST(VariantPublicInterfaceTest, ConvertingAssignmentBehavior) {
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
  {
    struct throw_assignable {
      int value;
      throw_assignable(int val) : value(val) { }
      throw_assignable(const throw_assignable&) = default;
      throw_assignable(throw_assignable&&) = default;
      throw_assignable& operator=(int) { throw 1; }
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
      nothrow_assignable(int) { throw 1; }
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
      nothrow_constructible& operator=(int) { throw 1; }
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
      throw_constructible_noexcept_move(int) { throw 1; }
      throw_constructible_noexcept_move(
          const throw_constructible_noexcept_move&) = default;
      throw_constructible_noexcept_move(
          throw_constructible_noexcept_move&&) noexcept {
        move_ctor = true;
      }
      throw_constructible_noexcept_move& operator=(int) { return *this; }
      throw_constructible_noexcept_move&
      operator=(const throw_constructible_noexcept_move&) = default;
      throw_constructible_noexcept_move&
      operator=(throw_constructible_noexcept_move&&) = default;
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
      throw_constructible_throw_move(int) { throw 1; }
      throw_constructible_throw_move(const throw_constructible_throw_move&) =
          default;
      throw_constructible_throw_move(throw_constructible_throw_move&&) {
        move_ctor = true;
      }
      throw_constructible_throw_move&
      operator=(const throw_constructible_throw_move&) = default;
      throw_constructible_throw_move&
      operator=(throw_constructible_throw_move&&) = default;
    };

    using v = variant<std::string, throw_constructible_throw_move>;
    v x = "abc";
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), "abc");
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_THROW(x = 3, int);
    EXPECT_TRUE(x.valueless_by_exception());
  }
}

struct dummy {
  int value;
  dummy() noexcept : value(2) { }
  explicit dummy(int val) : value(val) { }
};

struct may_throw_constructible {
  bool move_ctor = false;

  may_throw_constructible(int) { }
  may_throw_constructible(const may_throw_constructible&) = default;
  may_throw_constructible(may_throw_constructible&&) { move_ctor = true; }
  may_throw_constructible& operator=(const may_throw_constructible&) = default;
  may_throw_constructible& operator=(may_throw_constructible&&) = default;
};

struct throw_constructible {
  int value;
  bool move_ctor;

  throw_constructible() : value(2), move_ctor(false) { }
  throw_constructible(int) { throw 1; }
  throw_constructible(const throw_constructible&) = default;
  throw_constructible(throw_constructible&&) { move_ctor = true; }
  throw_constructible& operator=(const throw_constructible&) = default;
  throw_constructible& operator=(throw_constructible&&) = default;
};

struct may_throw_constructible_strategy_default : may_throw_constructible {
  using may_throw_constructible::may_throw_constructible;
};

struct throw_constructible_strategy_default : throw_constructible {
  using throw_constructible::throw_constructible;
};

struct throw_constructible_strategy_fallback : throw_constructible {
  using throw_constructible::throw_constructible;
};

struct may_throw_constructible_strategy_fallback : may_throw_constructible {
  using may_throw_constructible::may_throw_constructible;
};

template <>
struct variant_valueless_strategy<
    variant<dummy, throw_constructible_strategy_fallback>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::FALLBACK;
};

template <>
struct variant_valueless_strategy<
    variant<dummy, may_throw_constructible_strategy_fallback>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::FALLBACK;
};

struct throw_constructible_strategy_novalueless : throw_constructible {
  using throw_constructible::throw_constructible;
};

struct may_throw_constructible_strategy_novalueless : may_throw_constructible {
  using may_throw_constructible::may_throw_constructible;
};

template <>
struct variant_valueless_strategy<
    variant<dummy, throw_constructible_strategy_novalueless>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

template <>
struct variant_valueless_strategy<
    variant<dummy, may_throw_constructible_strategy_novalueless>> {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

TEST(VariantPublicInterfaceTest, ConvertingAssignmentValuelessStrategy) {
  {
    using v = variant<dummy, may_throw_constructible_strategy_default>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

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
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::LET_VARIANT_DECIDE);
    static_assert(!variant_no_valueless_state<v>::value);

    v x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_THROW(x = 3, int);
    EXPECT_TRUE(x.valueless_by_exception());
  }
  {
    using v = variant<dummy, may_throw_constructible_strategy_fallback>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

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
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::FALLBACK);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 3);
    EXPECT_EQ(x.index(), 0);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(get<0>(x).value, 3);
    EXPECT_THROW(x = 3, int);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x).value, 2);
  }
  {
    using v = variant<dummy, may_throw_constructible_strategy_novalueless>;
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

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
    static_assert(variant_valueless_strategy<v>::value ==
                  variant_valueless_strategy_t::DISALLOW_VALUELESS);
    static_assert(variant_no_valueless_state<v>::value);

    v x(std::in_place_index<0>, 3);
    EXPECT_EQ(x.index(), 0);
    EXPECT_FALSE(x.valueless_by_exception());
    EXPECT_EQ(get<0>(x).value, 3);
    EXPECT_THROW(x = 3, int);
    EXPECT_FALSE(x.valueless_by_exception());
  }
}

TEST(VariantPublicInterfaceTest, DeletedCopyConstructor) {
  static_assert(std::is_copy_constructible<variant<int>>::value);
  static_assert(std::is_copy_constructible<variant<int, long>>::value);
  static_assert(std::is_copy_constructible<variant<const int>>::value);
  static_assert(std::is_copy_constructible<variant<int, const double>>::value);
  static_assert(std::is_copy_constructible<variant<monostate>>::value);
  static_assert(std::is_copy_constructible<variant<int, monostate>>::value);
  {
    struct non_copyable {
      non_copyable(const non_copyable&) = delete;
      non_copyable(non_copyable&&) = default;
      non_copyable& operator=(const non_copyable&) = default;
      non_copyable& operator=(non_copyable&&) = default;
    };
    static_assert(!std::is_copy_constructible<variant<non_copyable>>::value);
    static_assert(
        !std::is_copy_constructible<variant<int, non_copyable>>::value);
  }
  {
    struct non_copy_assignable {
      non_copy_assignable(const non_copy_assignable&) = default;
      non_copy_assignable(non_copy_assignable&&) = default;
      non_copy_assignable& operator=(const non_copy_assignable&) = delete;
      non_copy_assignable& operator=(non_copy_assignable&&) = default;
    };
    static_assert(
        std::is_copy_constructible<variant<non_copy_assignable>>::value);
    static_assert(
        std::is_copy_constructible<variant<int, non_copy_assignable>>::value);
  }
  {
    struct move_only {
      move_only(const move_only&) = delete;
      move_only(move_only&&) = default;
      move_only& operator=(const move_only&) = delete;
      move_only& operator=(move_only&&) = default;
    };
    static_assert(!std::is_copy_constructible<variant<move_only>>::value);
    static_assert(!std::is_copy_constructible<variant<int, move_only>>::value);
  }
}

TEST(VariantPublicInterfaceTest, TrivialCopyConstructor) {
  static_assert(std::is_trivially_copy_constructible<variant<int>>::value);
  static_assert(
      std::is_trivially_copy_constructible<variant<int, long>>::value);
  static_assert(
      std::is_trivially_copy_constructible<variant<const int>>::value);
  static_assert(
      std::is_trivially_copy_constructible<variant<int, const double>>::value);
  static_assert(
      std::is_trivially_copy_constructible<variant<monostate>>::value);
  static_assert(
      std::is_trivially_copy_constructible<variant<int, monostate>>::value);

  {
    struct trivially_copyable {
      trivially_copyable(const trivially_copyable&) = default;
      trivially_copyable& operator=(const trivially_copyable&) = default;
    };
    static_assert(std::is_trivially_copy_constructible<
                  variant<trivially_copyable>>::value);
    static_assert(std::is_trivially_copy_constructible<
                  variant<trivially_copyable, int>>::value);
  }
  {
    struct trivially_copy_constructible {
      trivially_copy_constructible(const trivially_copy_constructible&) =
          default;
      trivially_copy_constructible&
      operator=(const trivially_copy_constructible&) {
        return *this;
      }
    };
    static_assert(std::is_trivially_copy_constructible<
                  variant<trivially_copy_constructible>>::value);
    static_assert(std::is_trivially_copy_constructible<
                  variant<trivially_copy_constructible, int>>::value);
  }
  {
    struct trivially_copy_assignable {
      trivially_copy_assignable(const trivially_copy_assignable&) { }
      trivially_copy_assignable&
      operator=(const trivially_copy_assignable&) = default;
    };
    static_assert(!std::is_trivially_copy_constructible<
                  variant<trivially_copy_assignable>>::value);
    static_assert(!std::is_trivially_copy_constructible<
                  variant<trivially_copy_assignable, int>>::value);
    static_assert(
        std::is_copy_constructible<variant<trivially_copy_assignable>>::value);
    static_assert(std::is_copy_constructible<
                  variant<trivially_copy_assignable, int>>::value);
  }
  {
    struct non_trivially_moveable {
      non_trivially_moveable(const non_trivially_moveable&) = default;
      non_trivially_moveable(non_trivially_moveable&&) { }
      non_trivially_moveable&
      operator=(const non_trivially_moveable&) = default;
      non_trivially_moveable& operator=(non_trivially_moveable&&) {
        return *this;
      }
    };
    static_assert(std::is_trivially_copy_constructible<
                  variant<non_trivially_moveable>>::value);
    static_assert(std::is_trivially_copy_constructible<
                  variant<non_trivially_moveable, int>>::value);
  }
  {
    struct non_trivially_copyable {
      non_trivially_copyable(const non_trivially_copyable&) { }
      non_trivially_copyable(non_trivially_copyable&&) = default;
      non_trivially_copyable& operator=(const non_trivially_copyable&) {
        return *this;
      }
      non_trivially_copyable& operator=(non_trivially_copyable&&) = default;
    };
    static_assert(!std::is_trivially_copy_constructible<
                  variant<non_trivially_copyable>>::value);
    static_assert(!std::is_trivially_copy_constructible<
                  variant<non_trivially_copyable, int>>::value);
    static_assert(
        std::is_copy_constructible<variant<non_trivially_copyable>>::value);
    static_assert(std::is_copy_constructible<
                  variant<non_trivially_copyable, int>>::value);
  }
}

TEST(VariantPublicInterfaceTest, NoexceptCopyConstructor) {
  static_assert(std::is_nothrow_copy_constructible<variant<int>>::value);
  static_assert(std::is_nothrow_copy_constructible<variant<int, long>>::value);
  static_assert(std::is_nothrow_copy_constructible<variant<const int>>::value);
  static_assert(
      std::is_nothrow_copy_constructible<variant<int, const double>>::value);
  static_assert(std::is_nothrow_copy_constructible<variant<monostate>>::value);
  static_assert(
      std::is_nothrow_copy_constructible<variant<int, monostate>>::value);

  struct nothrow_copyable {
    nothrow_copyable(const nothrow_copyable&) = default;
    nothrow_copyable(nothrow_copyable&&) = default;
    nothrow_copyable& operator=(const nothrow_copyable&) = default;
    nothrow_copyable& operator=(nothrow_copyable&&) = default;
  };
  static_assert(
      std::is_nothrow_copy_constructible<variant<nothrow_copyable>>::value);
  static_assert(std::is_nothrow_copy_constructible<
                variant<int, nothrow_copyable>>::value);

  struct throw_copy_constructible {
    throw_copy_constructible(const throw_copy_constructible&);
    throw_copy_constructible(throw_copy_constructible&&) = default;
    throw_copy_constructible&
    operator=(const throw_copy_constructible&) = default;
    throw_copy_constructible& operator=(throw_copy_constructible&&) = default;
  };
  static_assert(!std::is_nothrow_copy_constructible<
                variant<throw_copy_constructible>>::value);
  static_assert(!std::is_nothrow_copy_constructible<
                variant<int, throw_copy_constructible>>::value);
  static_assert(
      std::is_copy_constructible<variant<throw_copy_constructible>>::value);
  static_assert(std::is_copy_constructible<
                variant<int, throw_copy_constructible>>::value);

  struct throw_copy_assignable {
    throw_copy_assignable(const throw_copy_assignable&) = default;
    throw_copy_assignable(throw_copy_assignable&&) = default;
    throw_copy_assignable& operator=(const throw_copy_assignable&);
    throw_copy_assignable& operator=(throw_copy_assignable&&) = default;
  };
  static_assert(std::is_nothrow_copy_constructible<
                variant<throw_copy_assignable>>::value);
  static_assert(std::is_nothrow_copy_constructible<
                variant<int, throw_copy_assignable>>::value);

  struct throw_copyable {
    throw_copyable(const throw_copyable&);
    throw_copyable(throw_copyable&&) = default;
    throw_copyable& operator=(const throw_copyable&);
    throw_copyable& operator=(throw_copyable&&) = default;
  };
  static_assert(
      !std::is_nothrow_copy_constructible<variant<int, throw_copyable>>::value);
  static_assert(
      std::is_copy_constructible<variant<int, throw_copyable>>::value);
}

TEST(VariantPublicInterfaceTest, CopyConstructorBehavior) {
  {
    using v = variant<int>;
    v x1(std::in_place_index<0>, 3);
    EXPECT_EQ(x1.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(x1));
    EXPECT_EQ(get<int>(x1), 3);
    EXPECT_EQ(get<0>(x1), 3);
    v x2 = x1;
    EXPECT_EQ(x2.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(x2));
    EXPECT_EQ(get<int>(x2), 3);
    EXPECT_EQ(get<0>(x2), 3);
  }
  {
    using v = variant<int, long>;
    v x1(std::in_place_index<1>, 3);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x1));
    EXPECT_EQ(get<long>(x1), 3);
    EXPECT_EQ(get<1>(x1), 3);
    v x2 = x1;
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x2));
    EXPECT_EQ(get<long>(x2), 3);
    EXPECT_EQ(get<1>(x2), 3);
  }
  {
    using v = variant<counter>;
    v x1;
    EXPECT_EQ(x1.index(), 0);
    EXPECT_TRUE(holds_alternative<counter>(x1));
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::copy_assign_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);
    v x2 = x1;
    EXPECT_EQ(x2.index(), 0);
    EXPECT_TRUE(holds_alternative<counter>(x2));
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_construct_count, 1);
    EXPECT_EQ(counter::copy_assign_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, counter>;
    v x1(std::in_place_index<1>);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<counter>(x1));
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::copy_assign_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);
    v x2 = x1;
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<counter>(x2));
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_construct_count, 1);
    EXPECT_EQ(counter::copy_assign_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, valueless_t>;
    v x1;
    EXPECT_EQ(x1.index(), 0);
    EXPECT_FALSE(x1.valueless_by_exception());
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    v x2 = x1;
    EXPECT_TRUE(x2.valueless_by_exception());
  }
}

template <class Ty, class V>
constexpr bool constexpr_copy_construct_impl(V&& source,
                                             std::size_t expected_index,
                                             Ty expected_value) {
  V x = source;
  return x.index() == expected_index && x.index() == source.index() &&
         get<Ty>(x) == get<Ty>(source) && get<Ty>(x) == expected_value;
}

TEST(VariantPublicInterfaceTest, ConstexprCopyConstructor) {
  {
    using v = variant<int>;
    constexpr v x1(std::in_place_index<0>, 3);
    static_assert(x1.index() == 0);
    static_assert(holds_alternative<int>(x1));
    static_assert(get<int>(x1) == 3);
    static_assert(get<0>(x1) == 3);
    constexpr v x2 = x1;
    static_assert(x2.index() == 0);
    static_assert(holds_alternative<int>(x2));
    static_assert(get<int>(x2) == 3);
    static_assert(get<0>(x2) == 3);
  }
  {
    using v = variant<int, long>;
    constexpr v x1(std::in_place_index<1>, 3);
    static_assert(x1.index() == 1);
    static_assert(holds_alternative<long>(x1));
    static_assert(get<long>(x1) == 3l);
    static_assert(get<1>(x1) == 3l);
    constexpr v x2 = x1;
    static_assert(x2.index() == 1);
    static_assert(holds_alternative<long>(x2));
    static_assert(get<long>(x2) == 3l);
    static_assert(get<1>(x2) == 3l);
  }
  {
    struct copyable {
      int value;
      constexpr copyable(int val) : value(val) { }
      copyable(const copyable&) = default;
      copyable(copyable&&) = delete;
    };

    {
      using v = variant<copyable>;
      constexpr v x1(std::in_place_index<0>, 3);
      static_assert(x1.index() == 0);
      static_assert(holds_alternative<copyable>(x1));
      static_assert(get<copyable>(x1).value == 3);
      static_assert(get<0>(x1).value == 3);
      constexpr v x2 = x1;
      static_assert(x2.index() == 0);
      static_assert(holds_alternative<copyable>(x2));
      static_assert(get<copyable>(x2).value == 3);
      static_assert(get<0>(x2).value == 3);
    }
    {
      using v = variant<int, copyable>;
      constexpr v x1(std::in_place_index<1>, 3);
      static_assert(x1.index() == 1);
      static_assert(holds_alternative<copyable>(x1));
      static_assert(get<copyable>(x1).value == 3);
      static_assert(get<1>(x1).value == 3);
      constexpr v x2 = x1;
      static_assert(x2.index() == 1);
      static_assert(holds_alternative<copyable>(x2));
      static_assert(get<copyable>(x2).value == 3);
      static_assert(get<1>(x2).value == 3);
    }
  }
  {
    using v = variant<long, void*, const int>;
    static_assert(constexpr_copy_construct_impl(v(3l), 0, 3l));
    static_assert(constexpr_copy_construct_impl(v(nullptr), 1,
                                                static_cast<void*>(nullptr)));
    static_assert(constexpr_copy_construct_impl<const int>(v(3), 2, 3));
  }
}

TEST(VariantPublicInterfaceTest, DeletedMoveConstructor) {
  static_assert(std::is_move_constructible<variant<int>>::value);
  static_assert(std::is_move_constructible<variant<int, long>>::value);
  static_assert(std::is_move_constructible<variant<const int>>::value);
  static_assert(std::is_move_constructible<variant<int, const double>>::value);
  static_assert(std::is_move_constructible<variant<monostate>>::value);
  static_assert(std::is_move_constructible<variant<int, monostate>>::value);
  {
    struct non_moveable {
      non_moveable(const non_moveable&) = default;
      non_moveable(non_moveable&&) = delete;
      non_moveable& operator=(const non_moveable&) = default;
      non_moveable& operator=(non_moveable&&) = default;
    };
    static_assert(std::is_move_constructible<variant<non_moveable>>::value);
    static_assert(
        std::is_move_constructible<variant<int, non_moveable>>::value);
  }
  {
    struct non_copy_moveable {
      non_copy_moveable(const non_copy_moveable&) = delete;
      non_copy_moveable(non_copy_moveable&&) = delete;
      non_copy_moveable& operator=(const non_copy_moveable&) = default;
      non_copy_moveable& operator=(non_copy_moveable&&) = default;
    };
    static_assert(
        !std::is_move_constructible<variant<non_copy_moveable>>::value);
    static_assert(
        !std::is_move_constructible<variant<int, non_copy_moveable>>::value);
  }
  {
    struct non_move_assignable {
      non_move_assignable(const non_move_assignable&) = default;
      non_move_assignable(non_move_assignable&&) = default;
      non_move_assignable& operator=(const non_move_assignable&) = default;
      non_move_assignable& operator=(non_move_assignable&&) = delete;
    };
    static_assert(
        std::is_move_constructible<variant<non_move_assignable>>::value);
    static_assert(
        std::is_move_constructible<variant<int, non_move_assignable>>::value);
  }
  {
    struct move_only {
      move_only(const move_only&) = delete;
      move_only(move_only&&) = default;
      move_only& operator=(const move_only&) = delete;
      move_only& operator=(move_only&&) = default;
    };
    static_assert(!std::is_copy_constructible<variant<move_only>>::value);
    static_assert(!std::is_copy_constructible<variant<int, move_only>>::value);
    static_assert(std::is_move_constructible<variant<move_only>>::value);
    static_assert(std::is_move_constructible<variant<int, move_only>>::value);
  }
  {
    struct copy_only {
      copy_only(const copy_only&) = default;
      copy_only(copy_only&&) = delete;
      copy_only& operator=(const copy_only&) = default;
      copy_only& operator=(copy_only&&) = delete;
    };
    static_assert(std::is_move_constructible<variant<copy_only>>::value);
    static_assert(std::is_move_constructible<variant<int, copy_only>>::value);
  }
  {
    struct move_construct_only {
      move_construct_only(const move_construct_only&) = delete;
      move_construct_only(move_construct_only&&) = default;
      move_construct_only& operator=(const move_construct_only&) = delete;
      move_construct_only& operator=(move_construct_only&&) = delete;
    };
    static_assert(
        std::is_move_constructible<variant<move_construct_only>>::value);
    static_assert(
        std::is_move_constructible<variant<int, move_construct_only>>::value);
  }
}

TEST(VariantPublicInterfaceTest, TrivialMoveConstructor) {
  static_assert(std::is_trivially_move_constructible<variant<int>>::value);
  static_assert(
      std::is_trivially_move_constructible<variant<int, long>>::value);
  static_assert(
      std::is_trivially_move_constructible<variant<const int>>::value);
  static_assert(
      std::is_trivially_move_constructible<variant<int, const double>>::value);
  static_assert(
      std::is_trivially_move_constructible<variant<monostate>>::value);
  static_assert(
      std::is_trivially_move_constructible<variant<int, monostate>>::value);

  {
    struct trivially_moveable {
      trivially_moveable(const trivially_moveable&);
      trivially_moveable(trivially_moveable&&) = default;
      trivially_moveable& operator=(const trivially_moveable&);
      trivially_moveable& operator=(trivially_moveable&&) = default;
    };
    static_assert(std::is_trivially_move_constructible<
                  variant<trivially_moveable>>::value);
    static_assert(std::is_trivially_move_constructible<
                  variant<trivially_moveable, int>>::value);
  }
  {
    struct trivially_move_constructible {
      trivially_move_constructible(const trivially_move_constructible&);
      trivially_move_constructible(trivially_move_constructible&&) = default;
      trivially_move_constructible&
      operator=(const trivially_move_constructible&);
      trivially_move_constructible& operator=(trivially_move_constructible&&);
    };
    static_assert(std::is_trivially_move_constructible<
                  variant<trivially_move_constructible>>::value);
    static_assert(std::is_trivially_move_constructible<
                  variant<trivially_move_constructible, int>>::value);
  }
  {
    struct trivially_move_assignable {
      trivially_move_assignable(const trivially_move_assignable&);
      trivially_move_assignable(trivially_move_assignable&&);
      trivially_move_assignable& operator=(const trivially_move_assignable&);
      trivially_move_assignable&
      operator=(trivially_move_assignable&&) = default;
    };
    static_assert(!std::is_trivially_move_constructible<
                  variant<trivially_move_assignable>>::value);
    static_assert(!std::is_trivially_move_constructible<
                  variant<trivially_move_assignable, int>>::value);
    static_assert(
        std::is_move_constructible<variant<trivially_move_assignable>>::value);
    static_assert(std::is_move_constructible<
                  variant<trivially_move_assignable, int>>::value);
  }
  {
    struct non_trivially_copyable {
      non_trivially_copyable(const non_trivially_copyable&) { }
      non_trivially_copyable(non_trivially_copyable&&) = default;
      non_trivially_copyable& operator=(const non_trivially_copyable&) {
        return *this;
      }
      non_trivially_copyable& operator=(non_trivially_copyable&&) = default;
    };
    static_assert(std::is_trivially_move_constructible<
                  variant<non_trivially_copyable>>::value);
    static_assert(std::is_trivially_move_constructible<
                  variant<non_trivially_copyable, int>>::value);
  }
}

TEST(VariantPublicInterfaceTest, NoexceptMoveConstructor) {
  static_assert(std::is_nothrow_move_constructible<variant<int>>::value);
  static_assert(std::is_nothrow_move_constructible<variant<int, long>>::value);
  static_assert(std::is_nothrow_move_constructible<variant<const int>>::value);
  static_assert(
      std::is_nothrow_move_constructible<variant<int, const double>>::value);
  static_assert(std::is_nothrow_move_constructible<variant<monostate>>::value);
  static_assert(
      std::is_nothrow_move_constructible<variant<int, monostate>>::value);

  struct nothrow_moveable {
    nothrow_moveable(const nothrow_moveable&);
    nothrow_moveable(nothrow_moveable&&) = default;
    nothrow_moveable& operator=(const nothrow_moveable&);
    nothrow_moveable& operator=(nothrow_moveable&&);
  };
  static_assert(
      std::is_nothrow_move_constructible<variant<nothrow_moveable>>::value);
  static_assert(std::is_nothrow_move_constructible<
                variant<int, nothrow_moveable>>::value);

  struct throw_move_constructible {
    throw_move_constructible(const throw_move_constructible&) = default;
    throw_move_constructible(throw_move_constructible&&);
    throw_move_constructible&
    operator=(const throw_move_constructible&) = default;
    throw_move_constructible& operator=(throw_move_constructible&&) = default;
  };
  static_assert(!std::is_nothrow_move_constructible<
                variant<throw_move_constructible>>::value);
  static_assert(!std::is_nothrow_move_constructible<
                variant<int, throw_move_constructible>>::value);
  static_assert(
      std::is_move_constructible<variant<throw_move_constructible>>::value);
  static_assert(std::is_move_constructible<
                variant<int, throw_move_constructible>>::value);

  struct throw_move_assignable {
    throw_move_assignable(const throw_move_assignable&) = default;
    throw_move_assignable(throw_move_assignable&&) = default;
    throw_move_assignable& operator=(const throw_move_assignable&) = default;
    throw_move_assignable& operator=(throw_move_assignable&&);
  };
  static_assert(std::is_nothrow_move_constructible<
                variant<throw_move_assignable>>::value);
  static_assert(std::is_nothrow_move_constructible<
                variant<int, throw_move_assignable>>::value);

  struct throw_moveable {
    throw_moveable(const throw_moveable&) = default;
    throw_moveable(throw_moveable&&);
    throw_moveable& operator=(const throw_moveable&) = default;
    throw_moveable& operator=(throw_moveable&&);
  };
  static_assert(
      !std::is_nothrow_move_constructible<variant<int, throw_moveable>>::value);
  static_assert(
      std::is_move_constructible<variant<int, throw_moveable>>::value);
}

TEST(VariantPublicInterfaceTest, MoveConstructorBehavior) {
  {
    using v = variant<int>;
    v x1(std::in_place_index<0>, 3);
    EXPECT_EQ(x1.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(x1));
    EXPECT_EQ(get<int>(x1), 3);
    EXPECT_EQ(get<0>(x1), 3);
    v x2 = std::move(x1);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(x2));
    EXPECT_EQ(get<int>(x2), 3);
    EXPECT_EQ(get<0>(x2), 3);
    EXPECT_EQ(x1.index(), 0);
  }
  {
    using v = variant<int, long>;
    v x1(std::in_place_index<1>, 3);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x1));
    EXPECT_EQ(get<long>(x1), 3);
    EXPECT_EQ(get<1>(x1), 3);
    v x2 = std::move(x1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<long>(x2));
    EXPECT_EQ(get<long>(x2), 3);
    EXPECT_EQ(get<1>(x2), 3);
    EXPECT_EQ(x1.index(), 1);
  }
  {
    using v = variant<counter>;
    v x1;
    EXPECT_EQ(x1.index(), 0);
    EXPECT_TRUE(holds_alternative<counter>(x1));
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);
    EXPECT_EQ(counter::move_assign_count, 0);
    v x2 = std::move(x1);
    EXPECT_EQ(x2.index(), 0);
    EXPECT_TRUE(holds_alternative<counter>(x2));
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::move_construct_count, 1);
    EXPECT_EQ(counter::move_assign_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, counter>;
    v x1(std::in_place_index<1>);
    EXPECT_EQ(x1.index(), 1);
    EXPECT_TRUE(holds_alternative<counter>(x1));
    EXPECT_EQ(counter::alive_count, 1);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::move_construct_count, 0);
    EXPECT_EQ(counter::move_assign_count, 0);
    v x2 = std::move(x1);
    EXPECT_EQ(x2.index(), 1);
    EXPECT_TRUE(holds_alternative<counter>(x2));
    EXPECT_EQ(counter::alive_count, 2);
    EXPECT_EQ(counter::copy_construct_count, 0);
    EXPECT_EQ(counter::move_construct_count, 1);
    EXPECT_EQ(counter::move_assign_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, valueless_t>;
    v x1;
    EXPECT_EQ(x1.index(), 0);
    EXPECT_FALSE(x1.valueless_by_exception());
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    v x2 = std::move(x1);
    EXPECT_TRUE(x2.valueless_by_exception());
  }
}

template <class Ty, class V>
constexpr bool constexpr_move_construct_impl(V&& source,
                                             std::size_t expected_index,
                                             Ty expected_value) {
  V x = std::move(source);
  return x.index() == expected_index && x.index() == source.index() &&
         get<Ty>(x) == get<Ty>(source) && get<Ty>(x) == expected_value;
}

TEST(VariantPublicInterfaceTest, ConstexprMoveConstructor) {
  {
    using v = variant<int>;
    static_assert(
        constexpr_move_construct_impl(v(std::in_place_index<0>, 3), 0, 3));
  }
  {
    using v = variant<int, long>;
    static_assert(
        constexpr_move_construct_impl(v(std::in_place_index<1>, 3), 1, 3l));
  }
  {
    struct copyable {
      int value;
      constexpr copyable(int val) : value(val) { }
      copyable(const copyable&) = default;
      copyable(copyable&&) = delete;
    };

    {
      using v = variant<copyable>;
      // `static_cast` is used to convert prvalue to xvalue
      constexpr v x2 = static_cast<v&&>(v(std::in_place_index<0>, 3));
      static_assert(x2.index() == 0);
      static_assert(holds_alternative<copyable>(x2));
      static_assert(get<copyable>(x2).value == 3);
      static_assert(get<0>(x2).value == 3);
    }
    {
      using v = variant<int, copyable>;
      constexpr v x2 = static_cast<v&&>(v(std::in_place_index<1>, 3));
      static_assert(x2.index() == 1);
      static_assert(holds_alternative<copyable>(x2));
      static_assert(get<copyable>(x2).value == 3);
      static_assert(get<1>(x2).value == 3);
    }
  }
  {
    using v = variant<long, void*, const int>;
    static_assert(constexpr_move_construct_impl(v(3l), 0, 3l));
    static_assert(constexpr_move_construct_impl(v(nullptr), 1,
                                                static_cast<void*>(nullptr)));
    static_assert(constexpr_move_construct_impl<const int>(v(3), 2, 3));
  }
}

TEST(VariantPublicInterfaceTest, DeletedDefaultConstructor) {
  static_assert(std::is_default_constructible<variant<int>>::value);
  static_assert(std::is_default_constructible<variant<int, long>>::value);
  static_assert(std::is_default_constructible<variant<monostate>>::value);
  static_assert(std::is_default_constructible<variant<int, monostate>>::value);

  {
    struct non_default_constructible {
      non_default_constructible(int);
    };
    static_assert(!std::is_default_constructible<
                  variant<non_default_constructible>>::value);
    static_assert(std::is_default_constructible<
                  variant<int, non_default_constructible>>::value);
    static_assert(!std::is_default_constructible<
                  variant<non_default_constructible, int>>::value);
  }
}

TEST(VariantPublicInterfaceTest, NoexceptDefaultConstructor) {
  static_assert(std::is_nothrow_default_constructible<variant<int>>::value);
  static_assert(
      std::is_nothrow_default_constructible<variant<int, long>>::value);
  static_assert(
      std::is_nothrow_default_constructible<variant<monostate>>::value);
  static_assert(
      std::is_nothrow_default_constructible<variant<int, monostate>>::value);

  {
    struct may_throw_default_constructible {
      may_throw_default_constructible();
    };
    static_assert(!std::is_nothrow_default_constructible<
                  variant<may_throw_default_constructible>>::value);
    static_assert(!std::is_nothrow_default_constructible<
                  variant<may_throw_default_constructible, int>>::value);
    static_assert(std::is_nothrow_default_constructible<
                  variant<int, may_throw_default_constructible>>::value);
  }
  {
    struct throw_default_constructible {
      throw_default_constructible() { throw 1; }
    };

    using v1 = variant<throw_default_constructible>;
    EXPECT_THROW(v1 x, int);

    using v2 = variant<throw_default_constructible, int>;
    EXPECT_THROW(v2 x, int);

    using v3 = variant<int, throw_default_constructible>;
    v3 x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 0);
  }
}

TEST(VariantPublicInterfaceTest, DefaultConstructorBehavior) {
  {
    variant<int> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<int>(x));
    EXPECT_EQ(get<0>(x), 0);
    EXPECT_EQ(get<int>(x), 0);
  }
  {
    variant<long, int> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<long>(x));
    EXPECT_EQ(get<0>(x), 0l);
    EXPECT_EQ(get<long>(x), 0l);
  }
  {
    variant<std::string> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<std::string>(x));
    EXPECT_EQ(get<0>(x), "");
    EXPECT_EQ(get<std::string>(x), "");
  }
  {
    variant<std::string, int> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(holds_alternative<std::string>(x));
    EXPECT_EQ(get<0>(x), "");
    EXPECT_EQ(get<std::string>(x), "");
  }
  {
    variant<counter> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(counter::alive_count, 1);
    counter::reset();
  }
  {
    variant<counter, int> x;
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(counter::alive_count, 1);
    counter::reset();
  }
}

TEST(VariantPublicInterfaceTest, ConstexprDefaultConstructor) {
  {
    constexpr variant<int> x;
    static_assert(x.index() == 0);
    static_assert(holds_alternative<int>(x));
    static_assert(get<0>(x) == 0);
    static_assert(get<int>(x) == 0);
  }
  {
    constexpr variant<long, int> x;
    static_assert(x.index() == 0);
    static_assert(holds_alternative<long>(x));
    static_assert(get<0>(x) == 0l);
    static_assert(get<long>(x) == 0l);
  }
}

TEST(VariantPublicInterfaceTest, DeletedConvertingConstructor) {
  static_assert(std::is_constructible<variant<long>, int>::value);
  static_assert(std::is_constructible<variant<monostate>, monostate>::value);
  static_assert(
      std::is_constructible<variant<int, monostate>, monostate>::value);
  static_assert(std::is_constructible<variant<const int>, int>::value);
  static_assert(std::is_constructible<variant<int, const double>, int>::value);
  static_assert(
      std::is_constructible<variant<int, const double>, double>::value);
  static_assert(!std::is_constructible<variant<long, long long>, int>::value);
  static_assert(!std::is_constructible<variant<std::string, std::string>,
                                       const char*>::value);
  static_assert(
      !std::is_constructible<variant<std::string, void*>, int>::value);

#ifdef USE_CXX20
  static_assert(
      !std::is_constructible<variant<std::string, float>, int>::value);
  static_assert(!std::is_constructible<variant<std::unique_ptr<int>, bool>,
                                       std::unique_ptr<char>>::value);
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

TEST(VariantPublicInterfaceTest, NoexceptConvertingConstructor) {
  static_assert(std::is_nothrow_constructible<variant<int>, int>::value);
  static_assert(std::is_nothrow_constructible<variant<long>, int>::value);
  static_assert(
      std::is_nothrow_constructible<variant<monostate>, monostate>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, monostate>, monostate>::value);
  static_assert(std::is_nothrow_constructible<variant<const int>, int>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, const double>, int>::value);

  // Test cases from MSVC STL.
  struct dummy { };
  {
    struct nothrow {
      nothrow(int) noexcept { }
      nothrow& operator=(int) noexcept { return *this; }
    };
    static_assert(
        std::is_nothrow_constructible<variant<dummy, nothrow>, int>::value);
  }
  {
    struct throws_constructor {
      throws_constructor(int) noexcept(false) { }
      throws_constructor& operator=(int) noexcept { return *this; }
    };
    static_assert(
        !std::is_nothrow_constructible<variant<dummy, throws_constructor>,
                                       int>::value);
  }
  {
    struct throws_assignment {
      throws_assignment(int) noexcept { }
      throws_assignment& operator=(int) noexcept(false) { return *this; }
    };
    static_assert(
        std::is_nothrow_constructible<variant<dummy, throws_assignment>,
                                      int>::value);
  }
}

TEST(VariantPublicInterfaceTest, ConvertingConstructorBehavior) {
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
  {
    using v = variant<bool volatile, std::unique_ptr<int>>;
    v x = nullptr;
    EXPECT_EQ(x.index(), 1);
    EXPECT_EQ(get<1>(x), nullptr);
  }
#endif  // USE_CXX20
  {
    using v = variant<bool volatile const, int>;
    v x = true;
    EXPECT_EQ(x.index(), 0);
    EXPECT_TRUE(get<0>(x));
  }
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
}

TEST(VariantPublicInterfaceTest, DeletedInPlaceIndexConstructor) {
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
  }
}

TEST(VariantPublicInterfaceTest, NoexceptInPlaceIndexConstructor) {
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

TEST(VariantPublicInterfaceTest, InPlaceIndexConstructorBehavior) {
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
}

TEST(VariantPublicInterfaceTest, ConstexprInPlaceIndexConstructor) {
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
}

TEST(VariantPublicInterfaceTest, DeletedInPlaceIndexInitListConstructor) {
  using list = std::initializer_list<int>;

  static_assert(std::is_constructible<variant<list>, std::in_place_index_t<0>,
                                      list>::value);
  static_assert(std::is_constructible<variant<list>, std::in_place_index_t<0>,
                                      list&>::value);
  {
    struct constructible_from_list {
      constructible_from_list(list);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list>::value);
      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<1>, list>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<0>,
                                 std::initializer_list<double>>::value);
      static_assert(!std::is_constructible<v, std::in_place_index_t<0>, list,
                                           int>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<1>, list>::value);
      static_assert(
          std::is_constructible<v, std::in_place_index_t<1>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<2>, list>::value);
    }
  }
  {
    struct constructible_from_list {
      constructible_from_list(list&);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list>::value);
      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<1>, list>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<1>, list>::value);
      static_assert(
          std::is_constructible<v, std::in_place_index_t<1>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<2>, list>::value);
    }
  }
  {
    struct constructible_from_list {
      constructible_from_list(list&&);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<0>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<1>, list>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<1>, list>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<1>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<2>, list>::value);
    }
  }
  {
    struct constructible_from_list_args {
      constructible_from_list_args(list, int);
    };

    {
      using v = variant<constructible_from_list_args>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list, int>::value);
      static_assert(std::is_constructible<v, std::in_place_index_t<0>, list,
                                          double>::value);
      static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list,
                                           int>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<0>, list>::value);
      static_assert(!std::is_constructible<v, std::in_place_index_t<0>, list,
                                           int, int>::value);
    }
    {
      using v = variant<int, constructible_from_list_args>;

      static_assert(
          std::is_constructible<v, std::in_place_index_t<1>, list, int>::value);
      static_assert(std::is_constructible<v, std::in_place_index_t<1>, list,
                                          double>::value);
      static_assert(!std::is_constructible<v, std::in_place_index_t<2>, list,
                                           int>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_index_t<1>, list>::value);
      static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list,
                                           int, int>::value);
    }
  }
}

TEST(VariantPublicInterfaceTest, NoexceptInPlaceIndexInitListConstructor) {
  using list = std::initializer_list<int>;
  {
    struct may_throw_constructible {
      may_throw_constructible(list, int);
      may_throw_constructible(list, double) noexcept;
    };

    {
      using v = variant<may_throw_constructible>;

      static_assert(!std::is_nothrow_constructible<v, std::in_place_index_t<0>,
                                                   list, int>::value);
      static_assert(
          std::is_constructible<v, std::in_place_index_t<0>, list, int>::value);
      static_assert(std::is_nothrow_constructible<v, std::in_place_index_t<0>,
                                                  list, double>::value);
    }
  }
}

TEST(VariantPublicInterfaceTest, InPlaceIndexInitListConstructorBehavior) {
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
    using v = variant<constructible_from_init_list,
                      constructible_from_init_list_with_arg,
                      constructible_from_init_list>;

    {
      v x(std::in_place_index<0>, {1, 2, 3});
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(get<0>(x).list_size, 3);
    }
    {
      v x(std::in_place_index<1>, {1, 2, 3}, 4);
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(get<1>(x).list_size, 3);
      EXPECT_EQ(get<1>(x).value, 4);
    }
    {
      v x(std::in_place_index<2>, {1, 2, 3});
      EXPECT_EQ(x.index(), 2);
      EXPECT_EQ(get<2>(x).list_size, 3);
    }
    {
      constexpr v x(std::in_place_index<0>, {1, 2, 3});
      static_assert(x.index() == 0);
      static_assert(get<0>(x).list_size == 3);
    }
    {
      constexpr v x(std::in_place_index<1>, {1, 2, 3}, 4);
      static_assert(x.index() == 1);
      static_assert(get<1>(x).list_size == 3);
      static_assert(get<1>(x).value == 4);
    }
    {
      constexpr v x(std::in_place_index<2>, {1, 2, 3});
      static_assert(x.index() == 2);
      static_assert(get<2>(x).list_size == 3);
    }
  }
}

TEST(VariantPublicInterfaceTest, DeletedInPlaceTypeConstructor) {
  static_assert(std::is_constructible<variant<int>, std::in_place_type_t<int>,
                                      int>::value);
  static_assert(
      std::is_constructible<variant<int>, std::in_place_type_t<int>>::value);
  static_assert(!std::is_constructible<variant<int>, std::in_place_type_t<int>,
                                       int, int>::value);
  static_assert(
      !std::is_constructible<variant<int>, std::in_place_type_t<double>,
                             int>::value);
  static_assert(
      !std::is_convertible<std::in_place_type_t<int>, variant<int>>::value);
  static_assert(std::is_constructible<variant<int, long>,
                                      std::in_place_type_t<long>, int>::value);
  static_assert(
      !std::is_constructible<variant<int, long>, std::in_place_type_t<float>,
                             int>::value);
  static_assert(!std::is_convertible<std::in_place_type_t<long>,
                                     variant<int, long>>::value);
  static_assert(std::is_constructible<variant<int, long, long long>,
                                      std::in_place_type_t<long>, int>::value);
  static_assert(std::is_constructible<variant<int, long, int*>,
                                      std::in_place_type_t<int*>, int*>::value);
  static_assert(
      !std::is_constructible<variant<int, long, int*>,
                             std::in_place_type_t<void*>, void*>::value);
  static_assert(!std::is_convertible<std::in_place_type_t<int*>,
                                     variant<int, long, int*>>::value);
  static_assert(!std::is_constructible<variant<int, long, int>,
                                       std::in_place_type_t<int>, int>::value);
  static_assert(std::is_constructible<variant<int, long, int>,
                                      std::in_place_type_t<long>, int>::value);
  static_assert(!std::is_constructible<variant<int, long, int*>,
                                       std::in_place_type_t<int>, int*>::value);
  static_assert(
      !std::is_constructible<variant<int, long, int*>,
                             std::in_place_type_t<short>, int>::value);
  static_assert(
      std::is_constructible<variant<int, long, int*>,
                            std::in_place_type_t<int*>, std::nullptr_t>::value);

  {
    struct more_arguments {
      more_arguments(int, double, float);
    };
    static_assert(std::is_constructible<variant<more_arguments>,
                                        std::in_place_type_t<more_arguments>,
                                        int, double, float>::value);
    static_assert(std::is_constructible<variant<more_arguments>,
                                        std::in_place_type_t<more_arguments>,
                                        int, double, double>::value);
    static_assert(std::is_constructible<variant<more_arguments>,
                                        std::in_place_type_t<more_arguments>,
                                        long, double, float>::value);
    static_assert(!std::is_constructible<variant<more_arguments>,
                                         std::in_place_type_t<more_arguments>,
                                         int, double>::value);
    static_assert(!std::is_constructible<variant<more_arguments>,
                                         std::in_place_type_t<more_arguments>,
                                         int, double, float, int>::value);
    static_assert(std::is_constructible<variant<int, more_arguments>,
                                        std::in_place_type_t<more_arguments>,
                                        int, double, float>::value);
    static_assert(!std::is_constructible<variant<int, more_arguments>,
                                         std::in_place_type_t<more_arguments>,
                                         int, int, double, float>::value);
  }
}

TEST(VariantPublicInterfaceTest, NoexceptInPlaceTypeConstructor) {
  static_assert(
      std::is_nothrow_constructible<variant<int>, std::in_place_type_t<int>,
                                    int>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int>, std::in_place_type_t<int>,
                                    double>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, long, long long>,
                                    std::in_place_type_t<long>, int>::value);
  static_assert(std::is_nothrow_constructible<variant<int, long, long long>,
                                              std::in_place_type_t<long long>,
                                              float>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, std::string>,
                                    std::in_place_type_t<int>, int>::value);
  static_assert(
      !std::is_nothrow_constructible<variant<int, std::string>,
                                     std::in_place_type_t<std::string>,
                                     const char*>::value);
  static_assert(
      std::is_nothrow_constructible<variant<int, long, int*>,
                                    std::in_place_type_t<int*>, int*>::value);
  static_assert(std::is_nothrow_constructible<variant<int, long, int*>,
                                              std::in_place_type_t<int*>,
                                              std::nullptr_t>::value);

  {
    struct may_throw_constructible {
      may_throw_constructible(int);
      may_throw_constructible(double) noexcept;
    };
    static_assert(!std::is_nothrow_constructible<
                  variant<may_throw_constructible>,
                  std::in_place_type_t<may_throw_constructible>, int>::value);
    static_assert(
        std::is_constructible<variant<may_throw_constructible>,
                              std::in_place_type_t<may_throw_constructible>,
                              int>::value);
    static_assert(
        std::is_nothrow_constructible<
            variant<may_throw_constructible>,
            std::in_place_type_t<may_throw_constructible>, double>::value);
    static_assert(!std::is_nothrow_constructible<
                  variant<may_throw_constructible, int>,
                  std::in_place_type_t<may_throw_constructible>, int>::value);
    static_assert(
        std::is_constructible<variant<may_throw_constructible, int>,
                              std::in_place_type_t<may_throw_constructible>,
                              int>::value);
    static_assert(
        std::is_nothrow_constructible<
            variant<may_throw_constructible, int>,
            std::in_place_type_t<may_throw_constructible>, double>::value);
    static_assert(!std::is_nothrow_constructible<
                  variant<int, may_throw_constructible>,
                  std::in_place_type_t<may_throw_constructible>, int>::value);
    static_assert(
        std::is_constructible<variant<int, may_throw_constructible>,
                              std::in_place_type_t<may_throw_constructible>,
                              int>::value);
    static_assert(
        std::is_nothrow_constructible<
            variant<int, may_throw_constructible>,
            std::in_place_type_t<may_throw_constructible>, double>::value);
  }
  {
    struct more_arguments {
      more_arguments(int, double, float) noexcept;
      more_arguments(float, double, int);
    };
    static_assert(
        std::is_nothrow_constructible<variant<more_arguments>,
                                      std::in_place_type_t<more_arguments>, int,
                                      double, float>::value);
    static_assert(
        std::is_nothrow_constructible<variant<more_arguments>,
                                      std::in_place_type_t<more_arguments>, int,
                                      double, double>::value);
    static_assert(
        !std::is_nothrow_constructible<variant<more_arguments>,
                                       std::in_place_type_t<more_arguments>,
                                       float, double, int>::value);
    static_assert(
        std::is_nothrow_constructible<variant<int, more_arguments>,
                                      std::in_place_type_t<more_arguments>, int,
                                      double, float>::value);
    static_assert(
        !std::is_nothrow_constructible<variant<int, more_arguments>,
                                       std::in_place_type_t<more_arguments>,
                                       float, double, int>::value);
  }
}

TEST(VariantPublicInterfaceTest, InPlaceTypeConstructorBehavior) {
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
      throw_constructible(int) { throw 1; }
    };

    using v = variant<int, throw_constructible>;
    EXPECT_THROW(v x(std::in_place_type<throw_constructible>, 3), int);
    v x(std::in_place_index<0>, 5);
    EXPECT_EQ(x.index(), 0);
    EXPECT_EQ(get<0>(x), 5);
  }
}

TEST(VariantPublicInterfaceTest, ConstexprInPlaceTypeConstructor) {
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
}

TEST(VariantPublicInterfaceTest, DeletedInPlaceTypeInitListConstructor) {
  using list = std::initializer_list<int>;

  static_assert(std::is_constructible<variant<list>, std::in_place_type_t<list>,
                                      list>::value);
  static_assert(std::is_constructible<variant<list>, std::in_place_type_t<list>,
                                      list&>::value);
  {
    struct constructible_from_list {
      constructible_from_list(list);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_type_t<list>, list>::value);
      static_assert(
          !std::is_constructible<v,
                                 std::in_place_type_t<constructible_from_list>,
                                 std::initializer_list<double>>::value);
      static_assert(
          !std::is_constructible<v,
                                 std::in_place_type_t<constructible_from_list>,
                                 list, int>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_type_t<long>, list>::value);
    }
    {
      using v = variant<constructible_from_list, int, constructible_from_list>;
      static_assert(
          !std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          std::is_constructible<v, std::in_place_type_t<int>, int>::value);
    }
  }
  {
    struct constructible_from_list {
      constructible_from_list(list&);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_type_t<list>, list>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_type_t<int>, list>::value);
    }
  }
  {
    struct constructible_from_list {
      constructible_from_list(list&&);
    };

    {
      using v = variant<constructible_from_list>;

      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          !std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_type_t<list>, list>::value);
    }
    {
      using v = variant<int, constructible_from_list>;

      static_assert(
          std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list>::value);
      static_assert(
          !std::is_constructible<
              v, std::in_place_type_t<constructible_from_list>, list&>::value);
      static_assert(
          !std::is_constructible<v, std::in_place_type_t<int>, list>::value);
    }
  }
  {
    struct constructible_from_list_args {
      constructible_from_list_args(list, int);
    };

    {
      using v = variant<constructible_from_list_args>;

      static_assert(std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>, list,
                    int>::value);
      static_assert(std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>, list,
                    double>::value);
      static_assert(!std::is_constructible<v, std::in_place_type_t<list>, list,
                                           int>::value);
      static_assert(!std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>,
                    list>::value);
      static_assert(!std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>, list,
                    int, int>::value);
    }
    {
      using v = variant<int, constructible_from_list_args>;

      static_assert(std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>, list,
                    int>::value);
      static_assert(std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>, list,
                    double>::value);
      static_assert(!std::is_constructible<v, std::in_place_type_t<int>, list,
                                           int>::value);
      static_assert(!std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>,
                    list>::value);
      static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list,
                                           int, int>::value);
    }
    {
      using v = variant<constructible_from_list_args, int,
                        constructible_from_list_args>;
      static_assert(!std::is_constructible<
                    v, std::in_place_type_t<constructible_from_list_args>, list,
                    int>::value);
      static_assert(
          std::is_constructible<v, std::in_place_type_t<int>, int>::value);
    }
  }
}

TEST(VariantPublicInterfaceTest, NoexceptInPlaceTypeInitListConstructor) {
  using list = std::initializer_list<int>;
  {
    struct may_throw_constructible {
      may_throw_constructible(list, int);
      may_throw_constructible(list, double) noexcept;
    };

    {
      using v = variant<may_throw_constructible>;

      static_assert(!std::is_nothrow_constructible<
                    v, std::in_place_type_t<may_throw_constructible>, list,
                    int>::value);
      static_assert(
          std::is_constructible<v,
                                std::in_place_type_t<may_throw_constructible>,
                                list, int>::value);
      static_assert(std::is_nothrow_constructible<
                    v, std::in_place_type_t<may_throw_constructible>, list,
                    double>::value);
    }
  }
}

TEST(VariantPublicInterfaceTest, InPlaceTypeInitListConstructorBehavior) {
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
    using v = variant<constructible_from_init_list,
                      constructible_from_init_list_with_arg>;

    {
      v x(std::in_place_type<constructible_from_init_list>, {1, 2, 3});
      EXPECT_EQ(x.index(), 0);
      EXPECT_EQ(get<0>(x).list_size, 3);
      EXPECT_EQ(get<constructible_from_init_list>(x).list_size, 3);
    }
    {
      v x(std::in_place_type<constructible_from_init_list_with_arg>, {1, 2, 3},
          4);
      EXPECT_EQ(x.index(), 1);
      EXPECT_EQ(get<1>(x).list_size, 3);
      EXPECT_EQ(get<1>(x).value, 4);
      EXPECT_EQ(get<constructible_from_init_list_with_arg>(x).list_size, 3);
      EXPECT_EQ(get<constructible_from_init_list_with_arg>(x).value, 4);
    }
    {
      constexpr v x(std::in_place_type<constructible_from_init_list>,
                    {1, 2, 3});
      static_assert(x.index() == 0);
      static_assert(get<0>(x).list_size == 3);
      static_assert(get<constructible_from_init_list>(x).list_size == 3);
    }
    {
      constexpr v x(std::in_place_type<constructible_from_init_list_with_arg>,
                    {1, 2, 3}, 4);
      static_assert(x.index() == 1);
      static_assert(get<1>(x).list_size == 3);
      static_assert(get<1>(x).value == 4);
      static_assert(get<constructible_from_init_list_with_arg>(x).list_size ==
                    3);
      static_assert(get<constructible_from_init_list_with_arg>(x).value == 4);
    }
  }
}

TEST(VariantPublicInterfaceTest, TrivialDestructor) {
  static_assert(std::is_trivially_destructible<variant<int>>::value);
  static_assert(std::is_trivially_destructible<variant<int, long>>::value);
  static_assert(std::is_trivially_destructible<variant<int, int>>::value);

  {
    struct trivially_destructible {
      ~trivially_destructible() { }
    };

    static_assert(!std::is_trivially_destructible<
                  variant<trivially_destructible>>::value);
    static_assert(!std::is_trivially_destructible<
                  variant<int, trivially_destructible>>::value);
    static_assert(!std::is_trivially_destructible<
                  variant<int, int, trivially_destructible>>::value);
  }
  {
    struct non_trivially_destructible {
      ~non_trivially_destructible() = default;
    };

    static_assert(std::is_trivially_destructible<
                  variant<non_trivially_destructible>>::value);
    static_assert(std::is_trivially_destructible<
                  variant<int, non_trivially_destructible>>::value);
    static_assert(std::is_trivially_destructible<
                  variant<int, int, non_trivially_destructible>>::value);
  }
}

TEST(VariantPublicInterfaceTest, DestructorBehavior) {
  {
    using v = variant<counter>;
    EXPECT_EQ(counter::alive_count, 0);
    {
      v x;
      EXPECT_EQ(counter::alive_count, 1);
    }
    EXPECT_EQ(counter::alive_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, counter>;
    EXPECT_EQ(counter::alive_count, 0);
    {
      v x(std::in_place_index<1>);
      EXPECT_EQ(counter::alive_count, 1);
    }
    EXPECT_EQ(counter::alive_count, 0);
    counter::reset();
  }
  {
    using v = variant<int, counter>;
    EXPECT_EQ(counter::alive_count, 0);
    {
      v x(std::in_place_index<0>);
      EXPECT_EQ(counter::alive_count, 0);
    }
    EXPECT_EQ(counter::alive_count, 0);
    counter::reset();
  }
}

template <class Variant, std::size_t Index, class... Args,
          class = std::void_t<
              decltype(std::declval<Variant>().template emplace<Index>(
                  std::declval<Args>()...))>>
std::true_type has_emplace_impl(int);

template <class, std::size_t, class...>
std::false_type has_emplace_impl(...);

template <class Variant, std::size_t Index, class... Args>
struct has_emplace : decltype(has_emplace_impl<Variant, Index, Args...>(0)) { };

TEST(VariantPublicInterfaceTest, DeletedIndexEmplace) {
  struct non_default_constructible {
    non_default_constructible() = delete;
  };

  struct non_convertible {
    explicit non_convertible(int);
  };

  using v = variant<int, void*, const void*, non_default_constructible>;
  static_assert(has_emplace<v, 0>::value);
  static_assert(has_emplace<v, 0, int>::value);
  static_assert(!has_emplace<v, 0, decltype(nullptr)>::value);
  static_assert(has_emplace<v, 1, decltype(nullptr)>::value);
  static_assert(has_emplace<v, 1, int*>::value);
  static_assert(!has_emplace<v, 1, const int*>::value);
  static_assert(!has_emplace<v, 1, int>::value);
  static_assert(has_emplace<v, 2, const int*>::value);
  static_assert(has_emplace<v, 2, int*>::value);
  static_assert(!has_emplace<v, 3>::value);

  static_assert(has_emplace<variant<non_convertible>, 0, int>::value);
}

TEST(VariantPublicInterfaceTest, IndexEmplaceBehavior) {
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

// TODO: Test constexpr emplace
// TODO: Test noexcept emplace

TEST(VariantPublicInterfaceTest, IndexEmplaceValuelessStrategy) {
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

// TODO: emplace for initializer_list and type

TEST(VariantPublicInterfaceTest, Index) {
  {
    using v = variant<int, long>;
    v x1;
    static_assert(std::is_same<decltype(x1.index()), std::size_t>::value);
    static_assert(noexcept(x1.index()));
    EXPECT_EQ(x1.index(), 0);
    constexpr v x2;
    static_assert(std::is_same<decltype(x2.index()), std::size_t>::value);
    static_assert(noexcept(x2.index()));
    static_assert(x2.index() == 0);
  }
  {
    using v = variant<int, long>;
    v x1(std::in_place_index<1>);
    static_assert(std::is_same<decltype(x1.index()), std::size_t>::value);
    static_assert(noexcept(x1.index()));
    EXPECT_EQ(x1.index(), 1);
    constexpr v x2(std::in_place_index<1>);
    static_assert(std::is_same<decltype(x2.index()), std::size_t>::value);
    static_assert(noexcept(x2.index()));
    static_assert(x2.index() == 1);
  }
  {
    using v = variant<int, std::string>;
    v x("abc");
    static_assert(std::is_same<decltype(x.index()), std::size_t>::value);
    static_assert(noexcept(x.index()));
    EXPECT_EQ(x.index(), 1);
    x = 3;
    EXPECT_EQ(x.index(), 0);
  }

  // TODO: export valueless_npos as public interface and test it
}

TEST(VariantPublicInterfaceTest, ValuelessByException) {
  {
    using v = variant<int>;
    v x1;
    static_assert(
        std::is_same<decltype(x1.valueless_by_exception()), bool>::value);
    static_assert(noexcept(x1.valueless_by_exception()));
    EXPECT_FALSE(x1.valueless_by_exception());

    constexpr v x2;
    static_assert(
        std::is_same<decltype(x2.valueless_by_exception()), bool>::value);
    static_assert(noexcept(x2.valueless_by_exception()));
    static_assert(!x2.valueless_by_exception());
  }
  {
    using v = variant<int, long>;
    v x1;
    static_assert(
        std::is_same<decltype(x1.valueless_by_exception()), bool>::value);
    static_assert(noexcept(x1.valueless_by_exception()));
    EXPECT_FALSE(x1.valueless_by_exception());

    constexpr v x2;
    static_assert(
        std::is_same<decltype(x2.valueless_by_exception()), bool>::value);
    static_assert(noexcept(x2.valueless_by_exception()));
    static_assert(!x2.valueless_by_exception());
  }
  {
    using v = variant<int, long, std::string>;
    const v x1("abc");
    static_assert(
        std::is_same<decltype(x1.valueless_by_exception()), bool>::value);
    static_assert(noexcept(x1.valueless_by_exception()));
    EXPECT_FALSE(x1.valueless_by_exception());
  }
  {
    using v = variant<int, valueless_t>;
    v x(3);
    EXPECT_FALSE(x.valueless_by_exception());
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
  }
}

template <class T>
struct incomplete_holder { };

TEST(VariantPublicInterfaceTest, VisitRobustAgainstADL) {
  struct incomplete;

  variant<incomplete_holder<incomplete>*, int> x = nullptr;
  visit([](auto) {}, x);
  visit([](auto) -> incomplete_holder<incomplete>* { return nullptr; }, x);
  visit<void>([](auto) {}, x);
  visit<void*>([](auto) -> incomplete_holder<incomplete>* { return nullptr; },
               x);
}

enum call_type : unsigned {
  CT_NONE,
  CT_NON_CONST = 1,
  CT_CONST = 2,
  CT_LVALUE = 4,
  CT_RVALUE = 8,
};

constexpr call_type operator|(call_type lhs, call_type rhs) {
  return static_cast<call_type>(static_cast<unsigned>(lhs) |
                                static_cast<unsigned>(rhs));
}

struct forwarding_call_object {
  using self = forwarding_call_object;

  template <class... Args>
  self& operator()(Args&&...) & {
    set_call<Args&&...>(CT_NON_CONST | CT_LVALUE);
    return *this;
  }

  template <class... Args>
  const self& operator()(Args&&...) const& {
    set_call<Args&&...>(CT_CONST | CT_LVALUE);
    return *this;
  }

  template <class... Args>
  self&& operator()(Args&&...) && {
    set_call<Args&&...>(CT_NON_CONST | CT_RVALUE);
    return std::move(*this);
  }

  template <class... Args>
  const self&& operator()(Args&&...) const&& {
    set_call<Args&&...>(CT_CONST | CT_RVALUE);
    return std::move(*this);
  }

  template <class... Args>
  static void set_call(call_type type) {
    ASSERT_EQ(last_call_type, CT_NONE);
    ASSERT_TRUE(last_call_args.empty());
    last_call_type = type;
    last_call_args = {std::type_index(typeid(Args))...};
  }

  template <class... Args>
  static bool check_call(call_type type) {
    std::type_index expected[] = {std::type_index(typeid(int)),
                                  std::type_index(typeid(Args))...};
    bool const result =
        last_call_type == type &&
        std::equal(last_call_args.begin(), last_call_args.end(),
                   std::begin(expected) + 1, std::end(expected));

    last_call_type = CT_NONE;
    last_call_args.clear();

    return result;
  }

  constexpr operator int() const { return 0; }

  static call_type last_call_type;
  static std::vector<std::type_index> last_call_args;
};

call_type forwarding_call_object::last_call_type = CT_NONE;
std::vector<std::type_index> forwarding_call_object::last_call_args;

TEST(VariantPublicInterfaceTest, VisitForwardCallOperator) {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    visit(obj);
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_LVALUE));
    visit(cobj);
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_LVALUE));
    visit(std::move(obj));
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_RVALUE));
    visit(std::move(cobj));
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int>;
    v x(3);
    visit(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_LVALUE));
    visit(cobj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_LVALUE));
    visit(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_RVALUE));
    visit(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    visit(obj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_LVALUE));
    visit(cobj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_LVALUE));
    visit(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_RVALUE));
    visit(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_RVALUE));
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    visit(obj, x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_LVALUE)));
    visit(cobj, x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_LVALUE)));
    visit(std::move(obj), x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_RVALUE)));
    visit(std::move(cobj), x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(4), x4(1.1);
    visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    visit(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_LVALUE)));
    visit(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    visit(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    visit(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_LVALUE)));
    visit(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    visit(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_RVALUE)));
  }
}

TEST(VariantPublicInterfaceTest, VisitForwardArgument) {
  using fn = forwarding_call_object;
  fn obj {};
  const auto expected = CT_LVALUE | CT_NON_CONST;
  {
    using v = variant<int>;
    v x(3);
    const v& cx = x;
    visit(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(expected));
    visit(obj, cx);
    EXPECT_TRUE(fn::check_call<const int&>(expected));
    visit(obj, std::move(x));
    EXPECT_TRUE(fn::check_call<int&&>(expected));
    visit(obj, std::move(cx));
    EXPECT_TRUE(fn::check_call<const int&&>(expected));
  }
  {
    using v = variant<int, std::string, long>;
    v x1(3), x2("hello"), x3(3l);
    visit(obj, x1, x2, x3);
    EXPECT_TRUE((fn::check_call<int&, std::string&, long&>(expected)));
    visit(obj, std::as_const(x1), std::as_const(x2), std::move(x3));
    EXPECT_TRUE(
        (fn::check_call<const int&, const std::string&, long&&>(expected)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(expected)));
    visit(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
          std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int&&, double&&>(
            expected)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    visit(obj, x1, x2, x3, x4);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&, int*&, double&>(expected)));
    visit(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
          std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int*&&, double&&>(
            expected)));
  }
}

TEST(VariantPublicInterfaceTest, VisitReturnType) {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    static_assert(std::is_same<decltype(visit(obj)), fn&>::value);
    static_assert(std::is_same<decltype(visit(cobj)), const fn&>::value);
    static_assert(std::is_same<decltype(visit(std::move(obj))), fn&&>::value);
    static_assert(
        std::is_same<decltype(visit(std::move(cobj))), const fn&&>::value);
  }
  {
    using v = variant<int>;
    v x(3);
    static_assert(std::is_same<decltype(visit(obj, x)), fn&>::value);
    static_assert(std::is_same<decltype(visit(cobj, x)), const fn&>::value);
    static_assert(
        std::is_same<decltype(visit(std::move(obj), x)), fn&&>::value);
    static_assert(
        std::is_same<decltype(visit(std::move(cobj), x)), const fn&&>::value);
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    static_assert(std::is_same<decltype(visit(obj, x)), fn&>::value);
    static_assert(std::is_same<decltype(visit(cobj, x)), const fn&>::value);
    static_assert(
        std::is_same<decltype(visit(std::move(obj), x)), fn&&>::value);
    static_assert(
        std::is_same<decltype(visit(std::move(cobj), x)), const fn&&>::value);
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    static_assert(std::is_same<decltype(visit(obj, x1, x2)), fn&>::value);
    static_assert(
        std::is_same<decltype(visit(cobj, x1, x2)), const fn&>::value);
    static_assert(
        std::is_same<decltype(visit(std::move(obj), x1, x2)), fn&&>::value);
    static_assert(std::is_same<decltype(visit(std::move(cobj), x1, x2)),
                               const fn&&>::value);
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    static_assert(
        std::is_same<decltype(visit(obj, x1, x2, x3, x4)), fn&>::value);
    static_assert(
        std::is_same<decltype(visit(cobj, x1, x2, x3, x4)), const fn&>::value);
    static_assert(std::is_same<decltype(visit(std::move(obj), x1, x2, x3, x4)),
                               fn&&>::value);
    static_assert(std::is_same<decltype(visit(std::move(cobj), x1, x2, x3, x4)),
                               const fn&&>::value);
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    static_assert(
        std::is_same<decltype(visit(obj, x1, x2, x3, x4)), fn&>::value);
    static_assert(
        std::is_same<decltype(visit(cobj, x1, x2, x3, x4)), const fn&>::value);
    static_assert(std::is_same<decltype(visit(std::move(obj), x1, x2, x3, x4)),
                               fn&&>::value);
    static_assert(std::is_same<decltype(visit(std::move(cobj), x1, x2, x3, x4)),
                               const fn&&>::value);
  }
}

struct return_first {
  template <class... Args>
  constexpr int operator()(int val, Args&&...) const {
    return val;
  }
};

struct return_arity {
  template <class... Args>
  constexpr std::size_t operator()(Args&&...) const {
    return sizeof...(Args);
  }
};

TEST(VariantPublicInterfaceTest, ConstexprVisit) {
  constexpr return_first first {};
  constexpr return_arity arity {};

  {
    using v = variant<int>;
    constexpr v x(3);
    static_assert(visit(first, x) == 3);
  }
  {
    using v = variant<short, long, char>;
    constexpr v x(3l);
    static_assert(visit(first, x) == 3l);
  }
  {
    using v1 = variant<int>;
    using v2 = variant<int, char*, long long>;
    using v3 = variant<bool, int, int>;
    constexpr v1 x1;
    constexpr v2 x2(nullptr);
    constexpr v3 x3;
    static_assert(visit(arity, x1, x2, x3) == 3);
  }
  {
    using v = variant<int, long, double, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(visit(arity, x1, x2, x3, x4) == 4);
  }
  {
    using v = variant<int, long, double, long long, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(visit(arity, x1, x2, x3, x4) == 4);
  }
}

TEST(VariantPublicInterfaceTest, VisitException) {
  return_arity obj {};

  {
    using v = variant<int, valueless_t>;
    v x;
    make_valueless(x);
    EXPECT_TRUE(x.valueless_by_exception());
    EXPECT_THROW(visit(obj, x), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(visit(obj, x1, x2), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(visit(obj, x2, x1), bad_variant_access);
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
    EXPECT_THROW(visit(obj, x1, x2), bad_variant_access);
  }
}

struct derived_variant : variant<short, long, float> { };

template <std::size_t Idx>
void get(const derived_variant&) {
  EXPECT_TRUE(false);
}

TEST(VariantPublicInterfaceTest, DerivedFromVariantVisit) {
  {
    auto x1 = derived_variant {3};
    const auto cx1 = derived_variant {4};
    visit([](auto val) { EXPECT_EQ(val, 3); }, x1);
    visit([](auto val) { EXPECT_EQ(val, 4); }, cx1);
    visit([](auto val) { EXPECT_EQ(val, -1.25f); }, derived_variant {-1.25f});
    visit([](auto val) { EXPECT_EQ(val, 3); }, std::move(x1));
    visit([](auto val) { EXPECT_EQ(val, 4); }, std::move(cx1));
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

    visit([](auto val) { EXPECT_EQ(val, 12); }, evil_variant(12));
    visit([](auto val) { EXPECT_DOUBLE_EQ(val, 12.3); }, evil_variant(12.3));
  }

  {
    struct my_variant : variant<int, char, double> {
      using variant::variant;
    };

    my_variant x1(42), x2(3.14);

    auto visitor1 = [](auto&& x) { return static_cast<double>(x); };
    EXPECT_DOUBLE_EQ(visit(visitor1, x1), 42.0);
    EXPECT_DOUBLE_EQ(visit(visitor1, x2), 3.14);

    auto visitor2 = [](auto&& x, auto&& y) {
      return static_cast<double>(x + y);
    };
    EXPECT_DOUBLE_EQ(visit(visitor2, x1, x2), 45.14);
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
    EXPECT_THROW(visit(very_useful_visitor, x), bad_variant_access);
    EXPECT_THROW(visit(very_useful_visitor, my_variant(3), x),
                 bad_variant_access);
    EXPECT_THROW(visit(very_useful_visitor, x, my_variant(3)),
                 bad_variant_access);
  }
}

struct any_visitor {
  template <class Ty>
  void operator()(const Ty&) const { }
};

template <class Ty, class = decltype(visit(std::declval<any_visitor&>(),
                                           std::declval<Ty>()))>
constexpr bool has_visit(int) {
  return true;
}

template <class Ty>
constexpr bool has_visit(...) {
  return false;
}

TEST(VariantPublicInterfaceTest, DeletedVisit) {
  struct bad_variant1 : variant<short>, variant<long, float> { };
  struct bad_variant2 : private variant<long, float> { };
  struct good_variant1 : variant<long, float> { };
  struct good_variant2 : good_variant1 { };

  static_assert(!has_visit<int>(0));
#ifndef _MSC_VER
  /*
   * MSVC considers that bad_variant1 can be used for visit because it can be
   * legally used as the argument type of as_variant(). In fact, bad_variant1
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

template <class Fn>
struct mobile_visitor {
  mobile_visitor() = default;

  mobile_visitor(const mobile_visitor&) { ADD_FAILURE(); }
  mobile_visitor(mobile_visitor&&) { ADD_FAILURE(); }
  mobile_visitor& operator=(const mobile_visitor&) {
    ADD_FAILURE();
    return *this;
  }
  mobile_visitor& operator=(mobile_visitor&&) {
    ADD_FAILURE();
    return *this;
  }

  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    return Fn {}(std::forward<Args>(args)...);
  }
};

template <class Fn>
struct immobile_visitor : mobile_visitor<Fn> {
  immobile_visitor() = default;
  immobile_visitor(const immobile_visitor&) = delete;
  immobile_visitor& operator=(const immobile_visitor&) = delete;
};

template <class T>
struct convert_to_visitor {
  template <class U, std::enable_if_t<std::is_convertible_v<U, T>, int> = 0>
  constexpr T operator()(U&& u) const {
    return std::forward<U>(u);
  }
};

TEST(VariantPublicInterfaceTest, ImmobileFunctionVisit) {
  {
    using v = variant<int, double>;
    immobile_visitor<convert_to_visitor<double>> visitor {};
    EXPECT_DOUBLE_EQ(visit(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(visit(std::as_const(visitor), v {3.14}), 3.14);
    EXPECT_DOUBLE_EQ(visit(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(visit(std::move(std::as_const(visitor)), v {1.414}),
                     1.414);
  }
  {
    using v = variant<int, double>;
    mobile_visitor<convert_to_visitor<double>> visitor {};
    EXPECT_DOUBLE_EQ(visit(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(visit(std::as_const(visitor), v {3.14}), 3.14);
    EXPECT_DOUBLE_EQ(visit(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(visit(std::move(std::as_const(visitor)), v {1.414}),
                     1.414);
  }
}

template <class Ret>
void ret_visit_forward_call_operator_test_impl() {
  using fn = forwarding_call_object;
  fn obj {};
  const fn& cobj = obj;

  {
    visit<Ret>(obj);
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_LVALUE));
    visit<Ret>(cobj);
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_LVALUE));
    visit<Ret>(std::move(obj));
    EXPECT_TRUE(fn::check_call<>(CT_NON_CONST | CT_RVALUE));
    visit<Ret>(std::move(cobj));
    EXPECT_TRUE(fn::check_call<>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int>;
    v x(3);
    visit<Ret>(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_LVALUE));
    visit<Ret>(cobj, x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_LVALUE));
    visit<Ret>(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_NON_CONST | CT_RVALUE));
    visit<Ret>(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<int&>(CT_CONST | CT_RVALUE));
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    visit<Ret>(obj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_LVALUE));
    visit<Ret>(cobj, x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_LVALUE));
    visit<Ret>(std::move(obj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_NON_CONST | CT_RVALUE));
    visit<Ret>(std::move(cobj), x);
    EXPECT_TRUE(fn::check_call<long&>(CT_CONST | CT_RVALUE));
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    visit<Ret>(obj, x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_LVALUE)));
    visit<Ret>(cobj, x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_LVALUE)));
    visit<Ret>(std::move(obj), x1, x2);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&>(CT_NON_CONST | CT_RVALUE)));
    visit<Ret>(std::move(cobj), x1, x2);
    EXPECT_TRUE((fn::check_call<long&, std::string&>(CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(4), x4(1.1);
    visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    visit<Ret>(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_LVALUE)));
    visit<Ret>(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    visit<Ret>(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(
        CT_CONST | CT_RVALUE)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_LVALUE)));
    visit<Ret>(cobj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_LVALUE)));
    visit<Ret>(std::move(obj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_NON_CONST | CT_RVALUE)));
    visit<Ret>(std::move(cobj), x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int*&, double&>(
        CT_CONST | CT_RVALUE)));
  }
}

TEST(VariantPublicInterface, RetVisitForwardCallOperator) {
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
    visit<Ret>(obj, x);
    EXPECT_TRUE(fn::check_call<int&>(expected));
    visit<Ret>(obj, cx);
    EXPECT_TRUE(fn::check_call<const int&>(expected));
    visit<Ret>(obj, std::move(x));
    EXPECT_TRUE(fn::check_call<int&&>(expected));
    visit<Ret>(obj, std::move(cx));
    EXPECT_TRUE(fn::check_call<const int&&>(expected));
  }
  {
    using v = variant<int, std::string, long>;
    v x1(3), x2("hello"), x3(3l);
    visit<Ret>(obj, x1, x2, x3);
    EXPECT_TRUE((fn::check_call<int&, std::string&, long&>(expected)));
    visit<Ret>(obj, std::as_const(x1), std::as_const(x2), std::move(x3));
    EXPECT_TRUE(
        (fn::check_call<const int&, const std::string&, long&&>(expected)));
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE((fn::check_call<long&, std::string&, int&, double&>(expected)));
    visit<Ret>(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
               std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int&&, double&&>(
            expected)));
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    visit<Ret>(obj, x1, x2, x3, x4);
    EXPECT_TRUE(
        (fn::check_call<long&, std::string&, int*&, double&>(expected)));
    visit<Ret>(obj, std::as_const(x1), std::as_const(x2), std::move(x3),
               std::move(x4));
    EXPECT_TRUE(
        (fn::check_call<const long&, const std::string&, int*&&, double&&>(
            expected)));
  }
}

TEST(VariantPublicInterface, RetVisitForwardArgument) {
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
    static_assert(std::is_same<decltype(visit<Ret>(obj)), Ret>::value);
    static_assert(std::is_same<decltype(visit<Ret>(cobj)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(obj))), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(cobj))), Ret>::value);
  }
  {
    using v = variant<int>;
    v x(3);
    static_assert(std::is_same<decltype(visit<Ret>(obj, x)), Ret>::value);
    static_assert(std::is_same<decltype(visit<Ret>(cobj, x)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(obj), x)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(cobj), x)), Ret>::value);
  }
  {
    using v = variant<int, long, double>;
    v x(3l);
    static_assert(std::is_same<decltype(visit<Ret>(obj, x)), Ret>::value);
    static_assert(std::is_same<decltype(visit<Ret>(cobj, x)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(obj), x)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(cobj), x)), Ret>::value);
  }
  {
    using v1 = variant<int, long, double>;
    using v2 = variant<int*, std::string>;
    v1 x1(3l);
    v2 x2("hello");
    static_assert(std::is_same<decltype(visit<Ret>(obj, x1, x2)), Ret>::value);
    static_assert(std::is_same<decltype(visit<Ret>(cobj, x1, x2)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(obj), x1, x2)), Ret>::value);
    static_assert(std::is_same<decltype(visit<Ret>(std::move(cobj), x1, x2)),
                               Ret>::value);
  }
  {
    using v = variant<int, long, double, std::string>;
    v x1(3l), x2("hello"), x3(2), x4(1.1);
    static_assert(
        std::is_same<decltype(visit<Ret>(obj, x1, x2, x3, x4)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(cobj, x1, x2, x3, x4)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(obj), x1, x2, x3, x4)),
                     Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(cobj), x1, x2, x3, x4)),
                     Ret>::value);
  }
  {
    using v = variant<int, long, double, int*, std::string>;
    v x1(3l), x2("hello"), x3(nullptr), x4(1.1);
    static_assert(
        std::is_same<decltype(visit<Ret>(obj, x1, x2, x3, x4)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(cobj, x1, x2, x3, x4)), Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(obj), x1, x2, x3, x4)),
                     Ret>::value);
    static_assert(
        std::is_same<decltype(visit<Ret>(std::move(cobj), x1, x2, x3, x4)),
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

TEST(VariantPublicInterface, RetVisitReturnType) {
  ret_visit_return_type_test_impl<void>();
  ret_visit_return_type_test_impl<int>();

  {
    using v = variant<simple_derived<0>, simple_derived<1>, simple_derived<2>>;
    v x(simple_derived<1>(3));

    auto&& r1 = visit<simple_base&>(identity {}, x);
    static_assert(std::is_same<decltype(r1), simple_base&>::value);
    EXPECT_EQ(r1.x, 3);

    auto&& r2 = visit<const simple_base&>(identity {}, std::as_const(x));
    static_assert(std::is_same<decltype(r2), const simple_base&>::value);
    EXPECT_EQ(r2.x, 3);

    auto&& r3 = visit<simple_base&&>(identity {}, std::move(x));
    static_assert(std::is_same<decltype(r3), simple_base&&>::value);
    EXPECT_EQ(r3.x, 3);

    auto&& r4 =
        visit<const simple_base&&>(identity {}, std::move(std::as_const(x)));
    static_assert(std::is_same<decltype(r4), const simple_base&&>::value);
    EXPECT_EQ(r4.x, 3);
  }
}

TEST(VariantPublicInterface, ConstexprVoidVisit) {
  constexpr return_first first {};
  constexpr return_arity arity {};

  {
    using v = variant<int>;
    constexpr v x(3);
    static_assert((visit<void>(first, x), 3) == 3);
  }
  {
    using v = variant<short, long, char>;
    constexpr v x(3l);
    static_assert((visit<void>(first, x), 3l) == 3l);
  }
  {
    using v1 = variant<int>;
    using v2 = variant<int, char*, long long>;
    using v3 = variant<bool, int, int>;
    constexpr v1 x1;
    constexpr v2 x2(nullptr);
    constexpr v3 x3;
    static_assert((visit<void>(arity, x1, x2, x3), 3) == 3);
  }
  {
    using v = variant<int, long, double, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert((visit<void>(arity, x1, x2, x3, x4), 4) == 4);
  }
  {
    using v = variant<int, long, double, long long, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert((visit<void>(arity, x1, x2, x3, x4), 4) == 4);
  }
}

TEST(VariantPublicInterface, ConstexprOtherVisit) {
  constexpr return_first first {};
  constexpr return_arity arity {};

  {
    using v = variant<int>;
    constexpr v x(3);
    static_assert(visit<int>(first, x) == 3);
  }
  {
    using v = variant<short, long, char>;
    constexpr v x(3l);
    static_assert(visit<int>(first, x) == 3);
    static_assert(visit<long>(first, x) == 3l);
  }
  {
    using v1 = variant<int>;
    using v2 = variant<int, char*, long long>;
    using v3 = variant<bool, int, int>;
    constexpr v1 x1;
    constexpr v2 x2(nullptr);
    constexpr v3 x3;
    static_assert(visit<int>(arity, x1, x2, x3) == 3);
    static_assert(visit<double>(arity, x1, x2, x3) == 3.0);
    static_assert(visit<long long>(arity, x1, x2, x3) == 3ll);
  }
  {
    using v = variant<int, long, double, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(visit<int>(arity, x1, x2, x3, x4) == 4);
  }
  {
    using v = variant<int, long, double, long long, int*>;
    constexpr v x1(3l), x2(2), x3(nullptr), x4(1.1);
    static_assert(visit<int>(arity, x1, x2, x3, x4) == 4);
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
    EXPECT_THROW(visit<Ret>(obj, x), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(visit<Ret>(obj, x1, x2), bad_variant_access);
  }
  {
    using v1 = variant<int, valueless_t>;
    using v2 = variant<long, std::string, void*>;
    v1 x1;
    v2 x2("hello");
    make_valueless(x1);
    EXPECT_TRUE(x1.valueless_by_exception());
    EXPECT_FALSE(x2.valueless_by_exception());
    EXPECT_THROW(visit<Ret>(obj, x2, x1), bad_variant_access);
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
    EXPECT_THROW(visit<Ret>(obj, x1, x2), bad_variant_access);
  }
}

TEST(VariantPublicInterfaceTest, RetVisitException) {
  ret_visit_exception_test_impl<void>();
  ret_visit_exception_test_impl<const void>();
  ret_visit_exception_test_impl<int>();
  ret_visit_exception_test_impl<long long>();
}

TEST(VariantPublicInterfaceTest, RetVisitCallerAcceptNonConst) {
  struct test { };
  struct visitor {
    void operator()(test&) { }
  };

  variant<test> x;
  visit<void>(visitor {}, x);
}

TEST(VariantPublicInterfaceTest, ConstexprVisitExplicitSideEffect) {
  auto test_lambda = [](int arg) constexpr {
    variant<int> x = 101;
    visit<void>([arg](int& x) constexpr { x = arg; }, x);
    return get<int>(x);
  };

  static_assert(test_lambda(202) == 202);
}

TEST(VariantPublicInterfaceTest, DerivedFromVariantRetVisit) {
  {
    struct my_variant : variant<short, long, float> { };

    visit<bool>(
        [](auto x) {
          EXPECT_EQ(x, 3);
          return true;
        },
        my_variant {3});
    visit<bool>(
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

    visit<bool>(
        [](auto x) {
          EXPECT_EQ(x, 12);
          return true;
        },
        evil_variant(12));
    visit<bool>(
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
    EXPECT_DOUBLE_EQ(visit<double>(visitor1, x1), 42.0);
    EXPECT_DOUBLE_EQ(visit<double>(visitor1, x2), 3.14);

    auto visitor2 = [](auto&& x, auto&& y) { return x + y; };
    EXPECT_DOUBLE_EQ(visit<double>(visitor2, x1, x2), 45.14);
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
    EXPECT_THROW(visit<void>(very_useful_visitor, x), bad_variant_access);
    EXPECT_THROW(visit<void>(very_useful_visitor, my_variant(3), x),
                 bad_variant_access);
    EXPECT_THROW(visit<void>(very_useful_visitor, x, my_variant(3)),
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
          class = decltype(visit<bool>(
              std::declval<any_visitor_for_ret_visit&>(), std::declval<Ty>()))>
constexpr bool has_ret_visit(int) {
  return true;
}

template <class Ty>
constexpr bool has_ret_visit(...) {
  return false;
}

TEST(VariantPublicInterfaceTest, DeletedRetVisit) {
  struct bad_variant1 : variant<short>, variant<long, float> { };
  struct bad_variant2 : private variant<long, float> { };
  struct good_variant1 : variant<long, float> { };
  struct good_variant2 : good_variant1 { };

  static_assert(!has_ret_visit<int>(0));
#ifndef _MSC_VER
  /*
   * MSVC considers that bad_variant1 can be used for visit because it can be
   * legally used as the argument type of as_variant(). In fact, bad_variant1
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

TEST(VariantPublicInterfaceTest, RetVisitPerfectReturn) {
  {
    using ret = immobile_data;
    EXPECT_EQ(visit<ret>(identity {}, variant<int, short>(13)).x, 13);
    EXPECT_EQ(visit<ret>(identity {}, variant<int, short>((short) 13)).x, 13);

    struct convertible_to_immobile_one {
      operator immobile_data() const { return immobile_data {1729}; }
    };

    struct convertible_to_immobile_other {
      operator immobile_data() const { return immobile_data {1138}; }
    };

    using v =
        variant<convertible_to_immobile_one, convertible_to_immobile_other>;
    EXPECT_EQ(visit<ret>(identity {}, v(convertible_to_immobile_one())).x,
              1729);
    EXPECT_EQ(visit<ret>(identity {}, v(convertible_to_immobile_other())).x,
              1138);
  }
  {
    using ret = mobile_data;
    EXPECT_EQ(visit<ret>(identity {}, variant<int, short>(13)).x, 13);
    EXPECT_EQ(visit<ret>(identity {}, variant<int, short>((short) 13)).x, 13);

    ret r1(1729), r2(1138);
    auto visitor1 = [&r1](auto) mutable -> ret& { return r1; };
    EXPECT_EQ(visit<ret&>(visitor1, variant<int, short>(13)).x, 1729);
    auto visitor2 = [&r2](auto) mutable -> ret&& { return std::move(r2); };
    EXPECT_EQ(visit<ret&&>(visitor2, variant<int, short>(13)).x, 1138);
  }
}

TEST(VariantPublicInterfaceTest, ImmobileFunctionRetVisit) {
  {
    using v = variant<int, double>;
    immobile_visitor<identity> visitor {};
    EXPECT_DOUBLE_EQ(visit<double>(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(visit<double>(std::as_const(visitor), v {3.14}), 3.14);
    EXPECT_DOUBLE_EQ(visit<double>(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(
        visit<double>(std::move(std::as_const(visitor)), v {1.414}), 1.414);
  }
  {
    using v = variant<int, double>;
    mobile_visitor<identity> visitor {};
    EXPECT_DOUBLE_EQ(visit<double>(visitor, v {3}), 3.0);
    EXPECT_DOUBLE_EQ(visit<double>(std::as_const(visitor), v {3.14}), 3.14);
    EXPECT_DOUBLE_EQ(visit<double>(std::move(visitor), v {1729}), 1729.0);
    EXPECT_DOUBLE_EQ(
        visit<double>(std::move(std::as_const(visitor)), v {1.414}), 1.414);
  }
}

struct expr;

struct neg {
  std::shared_ptr<expr> e;
};

struct add {
  std::shared_ptr<expr> lhs;
  std::shared_ptr<expr> rhs;
};

struct mul {
  std::shared_ptr<expr> lhs;
  std::shared_ptr<expr> rhs;
};

struct expr : variant<int, neg, add, mul> {
  using variant::variant;
};

int eval(const expr& expr) {
  struct visitor {
    int operator()(int i) const { return i; }
    int operator()(const neg& n) const { return -eval(*n.e); }
    int operator()(const add& a) const { return eval(*a.lhs) + eval(*a.rhs); }
    int operator()(const mul& m) const { return eval(*m.lhs) * eval(*m.rhs); }
  };
  return visit(visitor {}, expr);
}

TEST(VariantPublicInterfaceTest, VisitP2162R2) {
  // example from P2162R2
  struct disconnected {
    int val;
  };

  struct connecting {
    char val;
  };

  struct connected {
    double val;
  };

  struct state : variant<disconnected, connecting, connected> {
    using variant::variant;
  };

  {
    state v1 = disconnected {45};
    const state v2 = connecting {'d'};
    visit([](auto x) { EXPECT_EQ(x.val, 45); }, v1);
    visit([](auto x) { EXPECT_EQ(x.val, 'd'); }, v2);
    visit([](auto x) { EXPECT_EQ(x.val, 5.5); }, state {connected {5.5}});
    visit([](auto x) { EXPECT_EQ(x.val, 45); }, std::move(v1));
    visit([](auto x) { EXPECT_EQ(x.val, 'd'); }, std::move(v2));
  }
  {
    const expr e =
        add {std::make_shared<expr>(1),
             std::make_shared<expr>(mul {
                 std::make_shared<expr>(2),
                 std::make_shared<expr>(neg {std::make_shared<expr>(3)})})};

    EXPECT_EQ(eval(e), 1 + 2 * -3);
  }
}

template <bool valueless, class... Tys>
struct fake_variant {
  typename std::aligned_union<0, Tys...>::type _data;
  typename variant_index<valueless ? sizeof...(Tys) + 1 : sizeof...(Tys)>::type
      _index;
};

template <int>
struct empty { };

template <int>
struct throw_empty {
  throw_empty() = default;
  throw_empty(const throw_empty&) = default;
  throw_empty(throw_empty&&) { }
  throw_empty& operator=(const throw_empty&) = default;
  throw_empty& operator=(throw_empty&&) = default;
};

struct not_empty {
  int i;
};

struct many_bases : empty<0>, empty<1>, empty<2>, empty<3> { };

template <class... Tys>
constexpr bool check_variant_size_align =
    sizeof(variant<Tys...>) ==
        sizeof(fake_variant<variant_no_valueless_state<variant<Tys...>>::value,
                            Tys...>) &&
    alignof(variant<Tys...>) ==
        alignof(fake_variant<variant_no_valueless_state<variant<Tys...>>::value,
                             Tys...>);

TEST(VariantPublicInterfaceTest, VariantSizeType) {
  constexpr auto u8_max =
      static_cast<std::size_t>((std::numeric_limits<std::uint8_t>::max)());
  constexpr auto u16_max =
      static_cast<std::size_t>((std::numeric_limits<std::uint16_t>::max)());
  constexpr auto u32_max =
      static_cast<std::size_t>((std::numeric_limits<std::uint32_t>::max)());
  constexpr auto u64_max =
      static_cast<std::size_t>((std::numeric_limits<std::uint64_t>::max)());

  static_assert(
      std::is_same<typename variant_index<0>::type, std::uint8_t>::value);
  static_assert(
      std::is_same<typename variant_index<1>::type, std::uint8_t>::value);
  static_assert(
      std::is_same<typename variant_index<u8_max>::type, std::uint8_t>::value);
  static_assert(std::is_same<typename variant_index<u8_max + 1>::type,
                             std::uint16_t>::value);
  static_assert(std::is_same<typename variant_index<u16_max>::type,
                             std::uint16_t>::value);
  static_assert(std::is_same<typename variant_index<u16_max + 1>::type,
                             std::uint32_t>::value);
  static_assert(std::is_same<typename variant_index<u32_max>::type,
                             std::uint32_t>::value);
  static_assert(std::is_same<typename variant_index<u32_max + 1>::type,
                             std::uint64_t>::value);
  static_assert(std::is_same<typename variant_index<u64_max>::type,
                             std::uint64_t>::value);

  static_assert(variant_no_valueless_state<variant<bool>>::value);
  static_assert(check_variant_size_align<bool>);

  static_assert(variant_no_valueless_state<variant<char>>::value);
  static_assert(check_variant_size_align<char>);

  static_assert(variant_no_valueless_state<variant<unsigned char>>::value);
  static_assert(check_variant_size_align<unsigned char>);

  static_assert(variant_no_valueless_state<variant<int>>::value);
  static_assert(check_variant_size_align<int>);

  static_assert(variant_no_valueless_state<variant<unsigned int>>::value);
  static_assert(check_variant_size_align<unsigned int>);

  static_assert(variant_no_valueless_state<variant<long>>::value);
  static_assert(check_variant_size_align<long>);

  static_assert(variant_no_valueless_state<variant<long long>>::value);
  static_assert(check_variant_size_align<long long>);

  static_assert(variant_no_valueless_state<variant<float>>::value);
  static_assert(check_variant_size_align<float>);

  static_assert(variant_no_valueless_state<variant<double>>::value);
  static_assert(check_variant_size_align<double>);

  static_assert(variant_no_valueless_state<variant<void*>>::value);
  static_assert(check_variant_size_align<void*>);

  static_assert(variant_no_valueless_state<variant<empty<0>>>::value);
  static_assert(check_variant_size_align<empty<0>>);

  static_assert(!variant_no_valueless_state<variant<throw_empty<0>>>::value);
  static_assert(check_variant_size_align<throw_empty<0>>);

  static_assert(variant_no_valueless_state<variant<not_empty>>::value);
  static_assert(check_variant_size_align<not_empty>);

  static_assert(variant_no_valueless_state<variant<many_bases>>::value);
  static_assert(check_variant_size_align<many_bases>);

  static_assert(!variant_no_valueless_state<
                variant<bool, char, short, int, long, long long, float, double,
                        long double, void*, empty<0>, empty<1>, not_empty,
                        many_bases, throw_empty<0>>>::value);
  static_assert(
      check_variant_size_align<bool, char, short, int, long, long long, float,
                               double, long double, void*, empty<0>, empty<1>,
                               not_empty, many_bases, throw_empty<0>>);
}

template <class Ty, std::size_t>
using big_variant_type_helper = Ty;

template <class IndexSequence, class Element,
          template <class...> class Container>
struct big_variant_impl;

template <std::size_t... Is, class Element, template <class...> class Container>
struct big_variant_impl<std::index_sequence<Is...>, Element, Container> {
  using type = Container<big_variant_type_helper<Element, Is>...>;
};

template <std::size_t Count, class Element>
struct big_variant
    : big_variant_impl<std::make_index_sequence<Count>, Element, variant> { };

static_assert(
    std::is_same<typename big_variant<1, int>::type, variant<int>>::value);
static_assert(
    std::is_same<typename big_variant<2, int>::type, variant<int, int>>::value);
static_assert(std::is_same<typename big_variant<3, int>::type,
                           variant<int, int, int>>::value);

template <std::size_t Count, std::size_t Index, std::size_t N>
void test_big_variant_gets() {
  using v = typename big_variant<Count, std::string>::type;
  v x(std::in_place_index<Index>, "abc");
  EXPECT_EQ(x.index(), Index);
  EXPECT_EQ(get<Index>(x), "abc");

  if constexpr (N != 0) {
    test_big_variant_gets<Count, Index + (Count - Index - 1) / N, N - 1>();
  }
}

#ifdef __EDG__
constexpr std::size_t big = 20;
#else  // C1XX and Clang
constexpr std::size_t big = 64;
#endif

template <std::size_t Count>
void test_big_variant_size() {
  constexpr std::size_t n = 16;

  if constexpr (Count <= big) {
    using v = typename big_variant<Count, std::string>::type;
    test_big_variant_gets<Count, 0, (n < Count ? n : Count)>();

    constexpr std::size_t i = Count / 2;
    v x1(std::in_place_index<i>, "abc");
    v x2 = x1;
    EXPECT_EQ(x2.index(), i);
    EXPECT_EQ(get<i>(x2), "abc");
    v x3 = std::move(x1);
    EXPECT_EQ(x3.index(), i);
    EXPECT_EQ(get<i>(x3), "abc");

    constexpr std::size_t j = (Count <= 2 ? 0 : i + 1);
    x2.template emplace<j>("def");
    x1 = x2;
    EXPECT_EQ(x1.index(), j);
    EXPECT_EQ(get<j>(x1), "def");
    x2 = std::move(x3);
    EXPECT_EQ(x2.index(), i);
    EXPECT_EQ(get<i>(x2), "abc");

    x1.swap(x2);
    EXPECT_EQ(x2.index(), j);
    EXPECT_EQ(get<j>(x2), "def");
    EXPECT_EQ(x1.index(), i);
    EXPECT_EQ(get<i>(x1), "abc");

    auto visitor = [](const std::string& s) { return s; };
    EXPECT_EQ(visit(visitor, x1), "abc");
  }
}

// Be careful compiling this code as it consumes a lot of system resources.
#if 0

TEST(VariantPublicInterfaceTest, BigVariant) {
  test_big_variant_size<1>();
  test_big_variant_size<3>();
  test_big_variant_size<15>();
  test_big_variant_size<63>();
  test_big_variant_size<255>();
  test_big_variant_size<big>();
}

#endif

TEST(VariantPublicInterfaceTest, VisitPointerToMember) {
  struct base {
    int x;
    int f() const { return x; }
  };

  struct derived : base {
    int y;
  };

  using v = variant<base, derived>;

  EXPECT_EQ(visit(&base::x, v(base {13})), 13);
  EXPECT_EQ(visit(&base::x, v(derived {{42}, 29})), 42);

  EXPECT_EQ(visit(&base::f, v(base {13})), 13);
  EXPECT_EQ(visit(&base::f, v(derived {{42}, 29})), 42);
}

}  // namespace rust
