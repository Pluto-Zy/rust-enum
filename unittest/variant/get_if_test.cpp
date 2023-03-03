#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestGetIf, IndexOverload) {
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

TEST(VariantTestGetIf, TypeOverload) {
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
}  // namespace
}  // namespace rust