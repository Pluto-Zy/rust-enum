#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestGet, IndexOverload) {
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
  {
    variant<int, valueless_t> v;
    make_valueless(v);
    EXPECT_TRUE(v.valueless_by_exception());
    EXPECT_THROW(get<0>(v), bad_variant_access);
    EXPECT_THROW(get<1>(v), bad_variant_access);
  }
}

TEST(VariantTestGet, TypeOverload) {
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
  {
    variant<int, valueless_t> v;
    make_valueless(v);
    EXPECT_TRUE(v.valueless_by_exception());
    EXPECT_THROW(get<int>(v), bad_variant_access);
    EXPECT_THROW(get<valueless_t>(v), bad_variant_access);
  }
}
}  // namespace
}  // namespace rust