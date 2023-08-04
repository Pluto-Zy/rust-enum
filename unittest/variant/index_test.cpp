#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestIndex, Basic) {
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
    {
        using v = variant<int&>;
        static int data = 3;
        v x1(data);
        static_assert(std::is_same<decltype(x1.index()), std::size_t>::value);
        static_assert(noexcept(x1.index()));
        EXPECT_EQ(x1.index(), 0);
        constexpr v x2(data);
        static_assert(x2.index() == 0);
    }
    {
        using v = variant<int&, long&>;
        static long data = 3;
        v x1(data);
        static_assert(std::is_same<decltype(x1.index()), std::size_t>::value);
        static_assert(noexcept(x1.index()));
        EXPECT_EQ(x1.index(), 1);
        constexpr v x2(data);
        static_assert(x2.index() == 1);
    }

    // TODO: export valueless_npos as public interface and test it
}
}  // namespace
}  // namespace rust