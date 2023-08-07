#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_STD_IMPL_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_STD_IMPL_HPP

#include <type_traits>

#include <rust_enum/variant/detail/macro.hpp>

#ifdef USE_CXX20
    #include <memory>
#else
    #include <utility>
#endif

#ifdef USE_CXX23
    #include <utility>
#endif

namespace rust::detail {
#ifdef USE_CXX20
using std::remove_cvref;

// std::construct_at() is constexpr in C++20
using std::construct_at;
using std::destroy_at;
#else
// Implementation of C++20 std::remove_cvref
template <class Ty>
struct remove_cvref : std::remove_cv<typename std::remove_reference<Ty>::type> { };

template <class Ty, class... Args>
auto construct_at(Ty* ptr, Args&&... args) -> Ty* {
    return ::new (const_cast<void*>(static_cast<const volatile void*>(ptr)))
        Ty(std::forward<Args>(args)...);
}

template <class Ty>
void destroy_at(Ty* location) noexcept {
    location->~Ty();
}
#endif  // USE_CXX20

#ifdef USE_CXX23
using std::unreachable;
#else
    #ifdef __GNUC__  // GCC 4.8+, Clang, Intel and other compilers compatible with
                     // GCC (-std=c++0x or above)
template <class = int>  // FIXME
[[noreturn]] __attribute__((always_inline)) CONSTEXPR17 void unreachable() {
    __builtin_unreachable();
}
    #elif defined(_MSC_VER)  // MSVC
[[noreturn]] __forceinline CONSTEXPR17 void unreachable() {
    __assume(false);
}
    #else
        #warning "Unknown compiler. Cannot implement `unreachable()` correctly."
[[noreturn]] void unreachable() {
    std::exit(1);
}
    #endif
#endif
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_STD_IMPL_HPP
