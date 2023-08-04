#include <common.hpp>
#include <limits>
#include <type_traits>

namespace rust {
namespace {
template <bool valueless, class... Tys>
struct fake_variant {
    typename std::aligned_union<0, Tys...>::type _data;
    typename detail::variant_index<valueless ? sizeof...(Tys) + 1 : sizeof...(Tys)>::type _index;
};

template <int>
struct empty { };

template <int>
struct throw_empty {
    throw_empty() = default;
    throw_empty(const throw_empty&) = default;
    throw_empty(throw_empty&&) { }
    throw_empty& operator=(const throw_empty&) = default;
    throw_empty& operator=(throw_empty&&) = default;
};

struct not_empty {
    int i;
};

struct many_bases : empty<0>, empty<1>, empty<2>, empty<3> { };

template <class... Tys>
constexpr bool check_variant_size_align = sizeof(variant<Tys...>)
        == sizeof(fake_variant<detail::variant_no_valueless_state<variant<Tys...>>::value, Tys...>)
    && alignof(variant<Tys...>)
        == alignof(fake_variant<
                   detail::variant_no_valueless_state<variant<Tys...>>::value,
                   Tys...>);

TEST(VariantTestBigVariant, SizeAndAlignment) {
    constexpr auto u8_max = static_cast<std::size_t>((std::numeric_limits<std::uint8_t>::max)());
    constexpr auto u16_max = static_cast<std::size_t>((std::numeric_limits<std::uint16_t>::max)());
    constexpr auto u32_max = static_cast<std::size_t>((std::numeric_limits<std::uint32_t>::max)());

    static_assert(std::is_same<detail::variant_index<0>::type, std::uint8_t>::value);
    static_assert(std::is_same<detail::variant_index<1>::type, std::uint8_t>::value);
    static_assert(std::is_same<detail::variant_index<u8_max>::type, std::uint8_t>::value);
    static_assert(std::is_same<detail::variant_index<u8_max + 1>::type, std::uint16_t>::value);
    static_assert(std::is_same<detail::variant_index<u16_max>::type, std::uint16_t>::value);
    static_assert(std::is_same<detail::variant_index<u16_max + 1>::type, std::uint32_t>::value);
    static_assert(std::is_same<detail::variant_index<u32_max>::type, std::uint32_t>::value);

    static_assert(detail::variant_no_valueless_state<variant<bool>>::value);
    static_assert(check_variant_size_align<bool>);

    static_assert(detail::variant_no_valueless_state<variant<char>>::value);
    static_assert(check_variant_size_align<char>);

    static_assert(detail::variant_no_valueless_state<variant<unsigned char>>::value);
    static_assert(check_variant_size_align<unsigned char>);

    static_assert(detail::variant_no_valueless_state<variant<int>>::value);
    static_assert(check_variant_size_align<int>);

    static_assert(detail::variant_no_valueless_state<variant<unsigned int>>::value);
    static_assert(check_variant_size_align<unsigned int>);

    static_assert(detail::variant_no_valueless_state<variant<long>>::value);
    static_assert(check_variant_size_align<long>);

    static_assert(detail::variant_no_valueless_state<variant<long long>>::value);
    static_assert(check_variant_size_align<long long>);

    static_assert(detail::variant_no_valueless_state<variant<float>>::value);
    static_assert(check_variant_size_align<float>);

    static_assert(detail::variant_no_valueless_state<variant<double>>::value);
    static_assert(check_variant_size_align<double>);

    static_assert(detail::variant_no_valueless_state<variant<void*>>::value);
    static_assert(check_variant_size_align<void*>);

    static_assert(detail::variant_no_valueless_state<variant<empty<0>>>::value);
    static_assert(check_variant_size_align<empty<0>>);

    static_assert(!detail::variant_no_valueless_state<variant<throw_empty<0>>>::value);
    static_assert(check_variant_size_align<throw_empty<0>>);

    static_assert(detail::variant_no_valueless_state<variant<not_empty>>::value);
    static_assert(check_variant_size_align<not_empty>);

    static_assert(detail::variant_no_valueless_state<variant<many_bases>>::value);
    static_assert(check_variant_size_align<many_bases>);

    static_assert(!detail::variant_no_valueless_state<variant<
                      bool,
                      char,
                      short,
                      int,
                      long,
                      long long,
                      float,
                      double,
                      long double,
                      void*,
                      empty<0>,
                      empty<1>,
                      not_empty,
                      many_bases,
                      throw_empty<0>>>::value);
    static_assert(check_variant_size_align<
                  bool,
                  char,
                  short,
                  int,
                  long,
                  long long,
                  float,
                  double,
                  long double,
                  void*,
                  empty<0>,
                  empty<1>,
                  not_empty,
                  many_bases,
                  throw_empty<0>>);
}

template <class Ty, std::size_t>
using big_variant_type_helper = Ty;

template <class IndexSequence, class Element, template <class...> class Container>
struct big_variant_impl;

template <std::size_t... Is, class Element, template <class...> class Container>
struct big_variant_impl<std::index_sequence<Is...>, Element, Container> {
    using type = Container<big_variant_type_helper<Element, Is>...>;
};

template <std::size_t Count, class Element>
struct big_variant : big_variant_impl<std::make_index_sequence<Count>, Element, variant> { };

static_assert(std::is_same<typename big_variant<1, int>::type, variant<int>>::value);
static_assert(std::is_same<typename big_variant<2, int>::type, variant<int, int>>::value);
static_assert(std::is_same<typename big_variant<3, int>::type, variant<int, int, int>>::value);

template <std::size_t Count, std::size_t Index, std::size_t N>
void test_big_variant_gets() {
    using v = typename big_variant<Count, std::string>::type;
    v x(std::in_place_index<Index>, "abc");
    EXPECT_EQ(x.index(), Index);
    EXPECT_EQ(get<Index>(x), "abc");

    if constexpr (N != 0) {
        test_big_variant_gets<Count, Index + (Count - Index - 1) / N, N - 1>();
    }
}

#ifdef __EDG__
constexpr std::size_t big = 20;
#else  // C1XX and Clang
constexpr std::size_t big = 64;
#endif

template <std::size_t Count>
void test_big_variant_size() {
    constexpr std::size_t n = 16;

    if constexpr (Count <= big) {
        using v = typename big_variant<Count, std::string>::type;
        test_big_variant_gets<Count, 0, (n < Count ? n : Count)>();

        constexpr std::size_t i = Count / 2;
        v x1(std::in_place_index<i>, "abc");
        v x2 = x1;
        EXPECT_EQ(x2.index(), i);
        EXPECT_EQ(get<i>(x2), "abc");
        v x3 = std::move(x1);
        EXPECT_EQ(x3.index(), i);
        EXPECT_EQ(get<i>(x3), "abc");

        constexpr std::size_t j = (Count <= 2 ? 0 : i + 1);
        x2.template emplace<j>("def");
        x1 = x2;
        EXPECT_EQ(x1.index(), j);
        EXPECT_EQ(get<j>(x1), "def");
        x2 = std::move(x3);
        EXPECT_EQ(x2.index(), i);
        EXPECT_EQ(get<i>(x2), "abc");

        x1.swap(x2);
        EXPECT_EQ(x2.index(), j);
        EXPECT_EQ(get<j>(x2), "def");
        EXPECT_EQ(x1.index(), i);
        EXPECT_EQ(get<i>(x1), "abc");

        auto visitor = [](const std::string& s) { return s; };
        EXPECT_EQ(visit(visitor, x1), "abc");
    }
}

TEST(VariantTestBigVariant, BigVariant) {
    test_big_variant_size<1>();
    test_big_variant_size<3>();
    test_big_variant_size<15>();
    test_big_variant_size<63>();
    test_big_variant_size<255>();
    test_big_variant_size<big>();
}
}  // namespace
}  // namespace rust
