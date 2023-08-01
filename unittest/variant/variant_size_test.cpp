#include <common.hpp>

namespace rust {
namespace {
template <class Variant, std::size_t Size>
void variant_size_test_helper() {
  static_assert(variant_size<Variant>::value == Size);
  static_assert(variant_size<const Variant>::value == Size);
#ifndef USE_CXX20
  static_assert(variant_size<volatile Variant>::value == Size);
  static_assert(variant_size<const volatile Variant>::value == Size);
#endif
  static_assert(variant_size_v<Variant> == Size);
  static_assert(variant_size_v<const Variant> == Size);
#ifndef USE_CXX20
  static_assert(variant_size_v<volatile Variant> == Size);
  static_assert(variant_size_v<const volatile Variant> == Size);
#endif
}

TEST(VariantTestVariantSize, Basic) {
  variant_size_test_helper<variant<>, 0>();
  variant_size_test_helper<variant<int>, 1>();
  variant_size_test_helper<variant<float, long, double*, const int>, 4>();
  variant_size_test_helper<variant<int&, const int&, float&, double>, 4>();
}
}  // namespace
}  // namespace rust
