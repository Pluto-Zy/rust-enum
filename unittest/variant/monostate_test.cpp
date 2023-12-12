#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestMonostate, Basic) {
    using m = monostate;

    static_assert(std::is_trivially_default_constructible<m>::value);
    static_assert(std::is_trivially_copy_constructible<m>::value);
    static_assert(std::is_trivially_copy_assignable<m>::value);
    static_assert(std::is_trivially_destructible<m>::value);

    constexpr m _m [[maybe_unused]] {};
}

TEST(VariantTestMonostate, Compare) {
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

#ifdef USE_CXX20
        static_assert(std::is_same<decltype(m1 <=> m2), std::strong_ordering>::value);
        static_assert(noexcept(m1 <=> m2));
#endif
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

#ifdef USE_CXX20
        static_assert(std::is_same<decltype(m1 <=> m2), std::strong_ordering>::value);
        static_assert(noexcept(m1 <=> m2));
#endif
    }
}
}  // namespace
}  // namespace rust
