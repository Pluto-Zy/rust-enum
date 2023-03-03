#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestDestructor, Trivial) {
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

TEST(VariantTestDestructor, BasicBehavior) {
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
}  // namespace
}  // namespace rust