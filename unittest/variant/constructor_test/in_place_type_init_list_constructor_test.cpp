#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestInPlaceTypeInitListConstructor, Deleted) {
    using list = std::initializer_list<int>;

    static_assert(std::is_constructible<variant<list>, std::in_place_type_t<list>, list>::value);
    static_assert(std::is_constructible<variant<list>, std::in_place_type_t<list>, list&>::value);

    static_assert(std::is_constructible<variant<list&>, std::in_place_type_t<list&>, list>::value);
    static_assert(  //
        std::is_constructible<variant<list&>, std::in_place_type_t<list&>, list&>::value
    );

    {
        struct constructible_from_list {
            constructible_from_list(list);
        };

        {
            using v = variant<constructible_from_list>;

            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list&>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<list>, list>::value);
            static_assert(  //
                !std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    std::initializer_list<double>>::value
            );
            static_assert(  //
                !std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list,
                    int>::value
            );
        }
        {
            using v = variant<int, constructible_from_list>;

            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list&>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<long>, list>::value);
        }
        {
            using v = variant<constructible_from_list, int, constructible_from_list>;
            static_assert(  //
                !std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(std::is_constructible<v, std::in_place_type_t<int>, int>::value);
        }
    }
    {
        struct constructible_from_list {
            constructible_from_list(list&);
        };

        {
            using v = variant<constructible_from_list>;

            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list&>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<list>, list>::value);
        }
        {
            using v = variant<int, constructible_from_list>;

            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list&>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<int>, list>::value);
        }
    }
    {
        struct constructible_from_list {
            constructible_from_list(list&&);
        };

        {
            using v = variant<constructible_from_list>;

            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(  //
                !std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list&>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<list>, list>::value);
        }
        {
            using v = variant<int, constructible_from_list>;

            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list>::value
            );
            static_assert(  //
                !std::is_constructible<  //
                    v,
                    std::in_place_type_t<constructible_from_list>,
                    list&>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<int>, list>::value);
        }
    }
    {
        struct constructible_from_list_args {
            constructible_from_list_args(list, int);
        };

        {
            using v = variant<constructible_from_list_args>;

            static_assert(  //
                std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list,
                    int>::value
            );
            static_assert(  //
                std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list,
                    double>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<list>, list, int>::value);
            static_assert(  //
                !std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list>::value
            );
            static_assert(  //
                !std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list,
                    int,
                    int>::value
            );
        }
        {
            using v = variant<int, constructible_from_list_args>;

            static_assert(  //
                std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list,
                    int>::value
            );
            static_assert(  //
                std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list,
                    double>::value
            );
            static_assert(!std::is_constructible<v, std::in_place_type_t<int>, list, int>::value);
            static_assert(  //
                !std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list>::value
            );
            static_assert(
                !std::is_constructible<v, std::in_place_index_t<1>, list, int, int>::value
            );
        }
        {
            using v = variant<constructible_from_list_args, int, constructible_from_list_args>;
            static_assert(  //
                !std::is_constructible<
                    v,
                    std::in_place_type_t<constructible_from_list_args>,
                    list,
                    int>::value
            );
            static_assert(std::is_constructible<v, std::in_place_type_t<int>, int>::value);
        }
    }
}

TEST(VariantTestInPlaceTypeInitListConstructor, Noexcept) {
    using list = std::initializer_list<int>;
    {
        struct may_throw_constructible {
            may_throw_constructible(list, int);
            may_throw_constructible(list, double) noexcept;
        };

        {
            using v = variant<may_throw_constructible>;

            static_assert(  //
                !std::is_nothrow_constructible<
                    v,
                    std::in_place_type_t<may_throw_constructible>,
                    list,
                    int>::value
            );
            static_assert(  //
                std::is_constructible<  //
                    v,
                    std::in_place_type_t<may_throw_constructible>,
                    list,
                    int>::value
            );
            static_assert(  //
                std::is_nothrow_constructible<
                    v,
                    std::in_place_type_t<may_throw_constructible>,
                    list,
                    double>::value
            );
        }
    }
}

TEST(VariantTestInPlaceTypeInitListConstructor, BasicBehavior) {
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
        using v = variant<constructible_from_init_list, constructible_from_init_list_with_arg>;

        {
            v x(std::in_place_type<constructible_from_init_list>, { 1, 2, 3 });
            EXPECT_EQ(x.index(), 0);
            EXPECT_EQ(get<0>(x).list_size, 3);
            EXPECT_EQ(get<constructible_from_init_list>(x).list_size, 3);
        }
        {
            v x(std::in_place_type<constructible_from_init_list_with_arg>, { 1, 2, 3 }, 4);
            EXPECT_EQ(x.index(), 1);
            EXPECT_EQ(get<1>(x).list_size, 3);
            EXPECT_EQ(get<1>(x).value, 4);
            EXPECT_EQ(get<constructible_from_init_list_with_arg>(x).list_size, 3);
            EXPECT_EQ(get<constructible_from_init_list_with_arg>(x).value, 4);
        }
        {
            constexpr v x(std::in_place_type<constructible_from_init_list>, { 1, 2, 3 });
            static_assert(x.index() == 0);
            static_assert(get<0>(x).list_size == 3);
            static_assert(get<constructible_from_init_list>(x).list_size == 3);
        }
        {
            constexpr v x(  //
                std::in_place_type<constructible_from_init_list_with_arg>,
                { 1, 2, 3 },
                4
            );
            static_assert(x.index() == 1);
            static_assert(get<1>(x).list_size == 3);
            static_assert(get<1>(x).value == 4);
            static_assert(get<constructible_from_init_list_with_arg>(x).list_size == 3);
            static_assert(get<constructible_from_init_list_with_arg>(x).value == 4);
        }
    }
}
}  // namespace
}  // namespace rust
