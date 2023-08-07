#include <common.hpp>

namespace rust {
namespace {
TEST(VariantTestHoldsAlternative, Basic) {
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
    {
        static int x = 3;
        constexpr variant<int, int&> v(std::in_place_index<1>, x);
        static_assert(holds_alternative<int&>(v));
        static_assert(noexcept(holds_alternative<int&>(v)));
        static_assert(!holds_alternative<int>(v));
        static_assert(noexcept(holds_alternative<int>(v)));
    }
}
}  // namespace
}  // namespace rust
