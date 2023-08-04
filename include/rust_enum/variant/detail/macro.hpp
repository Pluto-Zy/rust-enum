#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_MACRO_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_MACRO_HPP

#if __cplusplus >= 201103L
    #define USE_CXX11
#endif
#if __cplusplus >= 201402L
    #define USE_CXX14
#endif
#if __cplusplus >= 201703L
    #define USE_CXX17
#endif
#if __cplusplus >= 202002L
    #define USE_CXX20
#endif
#if __cplusplus >= 202302L
    #define USE_CXX23
#endif

#ifdef USE_CXX11
    #define CONSTEXPR11 constexpr
#else
    #define CONSTEXPR11 inline
#endif

#ifdef USE_CXX14
    #define CONSTEXPR14 constexpr
#else
    #define CONSTEXPR14 inline
#endif

#ifdef USE_CXX17
    #define CONSTEXPR17 constexpr
#else
    #define CONSTEXPR17 inline
#endif

#ifdef USE_CXX20
    #define CONSTEXPR20 constexpr
    #define CXX20_DEPRECATE_VOLATILE                                                              \
        [[deprecated(                                                                             \
            "Some operations on volatile-qualified types in the STL are deprecated in C++20."     \
        )]]
#else
    #define CONSTEXPR20 inline
    #define CXX20_DEPRECATE_VOLATILE
#endif

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_MACRO_HPP
