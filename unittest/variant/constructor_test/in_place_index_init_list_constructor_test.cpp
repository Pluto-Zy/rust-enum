#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestInPlaceIndexInitListConstructor, Deleted) {
    using list = std::initializer_list<int>;

    static_assert(std::is_constructible<variant<list>, std::in_place_index_t<0>, list>::value);
    static_assert(std::is_constructible<variant<list>, std::in_place_index_t<0>, list&>::value);

    static_assert(std::is_constructible<variant<list&>, std::in_place_index_t<0>, list>::value);
    static_assert(std::is_constructible<variant<list&>, std::in_place_index_t<0>, list&>::value);

    {
        struct constructible_from_list {
            constructible_from_list(list);
        };

        {
            using v = variant<constructible_from_list>;

            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list>::value);
            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list&>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list>::value);
            static_assert(!std::is_constructible<
                          v,
                          std::in_place_index_t<0>,
                          std::initializer_list<double>>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<0>, list, int>::value);
        }
        {
            using v = variant<int, constructible_from_list>;

            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list>::value);
            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list&>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<2>, list>::value);
        }
    }
    {
        struct constructible_from_list {
            constructible_from_list(list&);
        };

        {
            using v = variant<constructible_from_list>;

            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list>::value);
            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list&>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list>::value);
        }
        {
            using v = variant<int, constructible_from_list>;

            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list>::value);
            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list&>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<2>, list>::value);
        }
    }
    {
        struct constructible_from_list {
            constructible_from_list(list&&);
        };

        {
            using v = variant<constructible_from_list>;

            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<0>, list&>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list>::value);
        }
        {
            using v = variant<int, constructible_from_list>;

            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list&>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<2>, list>::value);
        }
    }
    {
        struct constructible_from_list_args {
            constructible_from_list_args(list, int);
        };

        {
            using v = variant<constructible_from_list_args>;

            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list, int>::value);
            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list, double>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list, int>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<0>, list>::value);
            static_assert(
                !std::is_constructible<v, std::in_place_index_t<0>, list, int, int>::value
            );
        }
        {
            using v = variant<int, constructible_from_list_args>;

            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list, int>::value);
            static_assert(std::is_constructible<v, std::in_place_index_t<1>, list, double>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<2>, list, int>::value);
            static_assert(!std::is_constructible<v, std::in_place_index_t<1>, list>::value);
            static_assert(
                !std::is_constructible<v, std::in_place_index_t<1>, list, int, int>::value
            );
        }
    }
}

TEST(VariantTestInPlaceIndexInitListConstructor, Noexcept) {
    using list = std::initializer_list<int>;
    {
        struct may_throw_constructible {
            may_throw_constructible(list, int);
            may_throw_constructible(list, double) noexcept;
        };

        {
            using v = variant<may_throw_constructible>;

            static_assert(
                !std::is_nothrow_constructible<v, std::in_place_index_t<0>, list, int>::value
            );
            static_assert(std::is_constructible<v, std::in_place_index_t<0>, list, int>::value);
            static_assert(
                std::is_nothrow_constructible<v, std::in_place_index_t<0>, list, double>::value
            );
        }
    }
}

TEST(VariantTestInPlaceIndexInitListConstructor, BasicBehavior) {
    using list_t = std::initializer_list<int>;

    struct constructible_from_init_list {
        std::size_t list_size;
        constexpr constructible_from_init_list(list_t list) : list_size(list.size()) { }
    };

    struct constructible_from_init_list_with_arg {
        std::size_t list_size;
        int value;
        constexpr constructible_from_init_list_with_arg(list_t list, int val) :
            list_size(list.size()), value(val) { }
    };

    {
        using v = variant<
            constructible_from_init_list,
            constructible_from_init_list_with_arg,
            constructible_from_init_list>;

        {
            v x(std::in_place_index<0>, { 1, 2, 3 });
            EXPECT_EQ(x.index(), 0);
            EXPECT_EQ(get<0>(x).list_size, 3);
        }
        {
            v x(std::in_place_index<1>, { 1, 2, 3 }, 4);
            EXPECT_EQ(x.index(), 1);
            EXPECT_EQ(get<1>(x).list_size, 3);
            EXPECT_EQ(get<1>(x).value, 4);
        }
        {
            v x(std::in_place_index<2>, { 1, 2, 3 });
            EXPECT_EQ(x.index(), 2);
            EXPECT_EQ(get<2>(x).list_size, 3);
        }
        {
            constexpr v x(std::in_place_index<0>, { 1, 2, 3 });
            static_assert(x.index() == 0);
            static_assert(get<0>(x).list_size == 3);
        }
        {
            constexpr v x(std::in_place_index<1>, { 1, 2, 3 }, 4);
            static_assert(x.index() == 1);
            static_assert(get<1>(x).list_size == 3);
            static_assert(get<1>(x).value == 4);
        }
        {
            constexpr v x(std::in_place_index<2>, { 1, 2, 3 });
            static_assert(x.index() == 2);
            static_assert(get<2>(x).list_size == 3);
        }
    }
}
}  // namespace
}  // namespace rust