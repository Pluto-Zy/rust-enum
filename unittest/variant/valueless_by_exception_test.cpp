#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestValuelessByException, Basic) {
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
}  // namespace
}  // namespace rust
