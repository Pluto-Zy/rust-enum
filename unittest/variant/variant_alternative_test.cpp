#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestVariantAlternative, Basic) {
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

TEST(VariantTestVariantAlternative, Reference) {
  using v = variant<int, int&, const int&, float>;

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
      std::is_same<typename variant_alternative<1, v>::type, int&>::value);
  static_assert(std::is_same<typename variant_alternative<1, const v>::type,
                             int&>::value);
  static_assert(std::is_same<typename variant_alternative<1, volatile v>::type,
                             int&>::value);
  static_assert(
      std::is_same<typename variant_alternative<1, const volatile v>::type,
                   int&>::value);

  static_assert(std::is_same<typename variant_alternative<2, v>::type,
                             const int&>::value);
  static_assert(std::is_same<typename variant_alternative<2, const v>::type,
                             const int&>::value);
  static_assert(std::is_same<typename variant_alternative<2, volatile v>::type,
                             const int&>::value);
  static_assert(
      std::is_same<typename variant_alternative<2, const volatile v>::type,
                   const int&>::value);

  static_assert(
      std::is_same<typename variant_alternative<3, v>::type, float>::value);
  static_assert(std::is_same<typename variant_alternative<3, const v>::type,
                             const float>::value);
  static_assert(std::is_same<typename variant_alternative<3, volatile v>::type,
                             volatile float>::value);
  static_assert(
      std::is_same<typename variant_alternative<3, const volatile v>::type,
                   const volatile float>::value);
}
}  // namespace
}  // namespace rust
