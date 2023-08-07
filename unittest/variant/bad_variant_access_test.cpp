#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestBadVariantAccess, Basic) {
    static_assert(std::is_nothrow_default_constructible<bad_variant_access>::value);
    static_assert(std::is_base_of<std::exception, bad_variant_access>::value);
    static_assert(std::is_nothrow_copy_constructible<bad_variant_access>::value);
    static_assert(std::is_nothrow_copy_assignable<bad_variant_access>::value);
    static_assert(noexcept(bad_variant_access {}.what()));

    const bad_variant_access e;
    EXPECT_TRUE(e.what());
}
}  // namespace
}  // namespace rust
