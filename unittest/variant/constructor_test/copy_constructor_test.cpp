#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestCopyConstructor, Deleted) {
    static_assert(std::is_copy_constructible<variant<int>>::value);
    static_assert(std::is_copy_constructible<variant<int, long>>::value);
    static_assert(std::is_copy_constructible<variant<const int>>::value);
    static_assert(std::is_copy_constructible<variant<int, const double>>::value);
    static_assert(std::is_copy_constructible<variant<monostate>>::value);
    static_assert(std::is_copy_constructible<variant<int, monostate>>::value);

    static_assert(std::is_copy_constructible<variant<int&>>::value);
    static_assert(std::is_copy_constructible<variant<float&>>::value);
    static_assert(std::is_copy_constructible<variant<int&, float>>::value);
    static_assert(std::is_copy_constructible<variant<int&, float&>>::value);
    static_assert(std::is_copy_constructible<variant<std::string&>>::value);
    static_assert(std::is_copy_constructible<variant<int const&>>::value);
    static_assert(std::is_copy_constructible<variant<int&, float const>>::value);

    {
        struct non_copyable {
            non_copyable(const non_copyable&) = delete;
            non_copyable(non_copyable&&) = default;
            auto operator=(const non_copyable&) -> non_copyable& = default;
            auto operator=(non_copyable&&) -> non_copyable& = default;
        };
        static_assert(!std::is_copy_constructible<variant<non_copyable>>::value);
        static_assert(!std::is_copy_constructible<variant<int, non_copyable>>::value);
        static_assert(std::is_copy_constructible<variant<int, non_copyable&>>::value);
        static_assert(std::is_copy_constructible<variant<int, non_copyable const&>>::value);
        static_assert(  //
            !std::is_copy_constructible<variant<int, non_copyable&, non_copyable>>::value
        );
    }
    {
        struct non_copy_assignable {
            non_copy_assignable(const non_copy_assignable&) = default;
            non_copy_assignable(non_copy_assignable&&) = default;
            auto operator=(const non_copy_assignable&) -> non_copy_assignable& = delete;
            auto operator=(non_copy_assignable&&) -> non_copy_assignable& = default;
        };
        static_assert(std::is_copy_constructible<variant<non_copy_assignable>>::value);
        static_assert(std::is_copy_constructible<variant<int, non_copy_assignable>>::value);
    }
    {
        struct move_only {
            move_only(const move_only&) = delete;
            move_only(move_only&&) = default;
            auto operator=(const move_only&) -> move_only& = delete;
            auto operator=(move_only&&) -> move_only& = default;
        };
        static_assert(!std::is_copy_constructible<variant<move_only>>::value);
        static_assert(!std::is_copy_constructible<variant<int, move_only>>::value);
        static_assert(std::is_copy_constructible<variant<move_only&>>::value);
        static_assert(std::is_copy_constructible<variant<int, move_only&>>::value);
    }
}

TEST(VariantTestCopyConstructor, Trivial) {
    static_assert(std::is_trivially_copy_constructible<variant<int>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<int, long>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<const int>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<int, const double>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<monostate>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<int, monostate>>::value);

    static_assert(std::is_trivially_copy_constructible<variant<int&>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<float&>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<int&, float>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<int&, float&>>::value);
    static_assert(std::is_trivially_copy_constructible<variant<std::string&>>::value);
    static_assert(!std::is_trivially_copy_constructible<variant<std::string&, std::string>>::value
    );

    {
        struct trivially_copyable {
            trivially_copyable(const trivially_copyable&) = default;
            auto operator=(const trivially_copyable&) -> trivially_copyable& = default;
        };
        static_assert(std::is_trivially_copy_constructible<variant<trivially_copyable>>::value);
        static_assert(  //
            std::is_trivially_copy_constructible<variant<trivially_copyable, int>>::value
        );
    }
    {
        struct trivially_copy_constructible {
            trivially_copy_constructible(const trivially_copy_constructible&) = default;
            auto operator=(const trivially_copy_constructible&) -> trivially_copy_constructible& {
                return *this;
            }
        };
        static_assert(
            std::is_trivially_copy_constructible<variant<trivially_copy_constructible>>::value
        );
        static_assert(
            std::is_trivially_copy_constructible<variant<trivially_copy_constructible, int>>::value
        );
    }
    {
        struct trivially_copy_assignable {
            trivially_copy_assignable(const trivially_copy_assignable&) { }
            auto operator=(const trivially_copy_assignable&)
                -> trivially_copy_assignable& = default;
        };
        static_assert(
            !std::is_trivially_copy_constructible<variant<trivially_copy_assignable>>::value
        );
        static_assert(
            !std::is_trivially_copy_constructible<variant<trivially_copy_assignable, int>>::value
        );
        static_assert(std::is_copy_constructible<variant<trivially_copy_assignable>>::value);
        static_assert(std::is_copy_constructible<variant<trivially_copy_assignable, int>>::value);
        static_assert(
            std::is_trivially_copy_constructible<variant<trivially_copy_assignable&>>::value
        );
        static_assert(
            std::is_trivially_copy_constructible<variant<trivially_copy_assignable&, int>>::value
        );
    }
    {
        struct non_trivially_moveable {
            non_trivially_moveable(const non_trivially_moveable&) = default;
            non_trivially_moveable(non_trivially_moveable&&) { }
            auto operator=(const non_trivially_moveable&) -> non_trivially_moveable& = default;
            auto operator=(non_trivially_moveable&&) -> non_trivially_moveable& {
                return *this;
            }
        };
        static_assert(  //
            std::is_trivially_copy_constructible<variant<non_trivially_moveable>>::value
        );
        static_assert(
            std::is_trivially_copy_constructible<variant<non_trivially_moveable, int>>::value
        );
    }
    {
        struct non_trivially_copyable {
            non_trivially_copyable(const non_trivially_copyable&) { }
            non_trivially_copyable(non_trivially_copyable&&) = default;
            auto operator=(const non_trivially_copyable&) -> non_trivially_copyable& {
                return *this;
            }
            auto operator=(non_trivially_copyable&&) -> non_trivially_copyable& = default;
        };
        static_assert(  //
            !std::is_trivially_copy_constructible<variant<non_trivially_copyable>>::value
        );
        static_assert(
            !std::is_trivially_copy_constructible<variant<non_trivially_copyable, int>>::value
        );
        static_assert(std::is_copy_constructible<variant<non_trivially_copyable>>::value);
        static_assert(std::is_copy_constructible<variant<non_trivially_copyable, int>>::value);
        static_assert(  //
            std::is_trivially_copy_constructible<variant<non_trivially_copyable&>>::value
        );
        static_assert(
            std::is_trivially_copy_constructible<variant<non_trivially_copyable&, int>>::value
        );
    }
}

TEST(VariantTestCopyConstructor, Noexcept) {
    static_assert(std::is_nothrow_copy_constructible<variant<int>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int, long>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<const int>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int, const double>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<monostate>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int, monostate>>::value);

    static_assert(std::is_nothrow_copy_constructible<variant<int&>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<float&>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int&, float>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int&, float&>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<std::string&>>::value);
    static_assert(!std::is_nothrow_copy_constructible<variant<std::string&, std::string>>::value);

    struct nothrow_copyable {
        nothrow_copyable(const nothrow_copyable&) = default;
        nothrow_copyable(nothrow_copyable&&) = default;
        auto operator=(const nothrow_copyable&) -> nothrow_copyable& = default;
        auto operator=(nothrow_copyable&&) -> nothrow_copyable& = default;
    };
    static_assert(std::is_nothrow_copy_constructible<variant<nothrow_copyable>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int, nothrow_copyable>>::value);

    struct throw_copy_constructible {
        throw_copy_constructible(const throw_copy_constructible&);
        throw_copy_constructible(throw_copy_constructible&&) = default;
        auto operator=(const throw_copy_constructible&) -> throw_copy_constructible& = default;
        auto operator=(throw_copy_constructible&&) -> throw_copy_constructible& = default;
    };
    static_assert(!std::is_nothrow_copy_constructible<variant<throw_copy_constructible>>::value);
    static_assert(
        !std::is_nothrow_copy_constructible<variant<int, throw_copy_constructible>>::value
    );
    static_assert(std::is_copy_constructible<variant<throw_copy_constructible>>::value);
    static_assert(std::is_copy_constructible<variant<int, throw_copy_constructible>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<throw_copy_constructible&>>::value);
    static_assert(
        std::is_nothrow_copy_constructible<variant<int, throw_copy_constructible&>>::value
    );

    struct throw_copy_assignable {
        throw_copy_assignable(const throw_copy_assignable&) = default;
        throw_copy_assignable(throw_copy_assignable&&) = default;
        auto operator=(const throw_copy_assignable&) -> throw_copy_assignable&;
        auto operator=(throw_copy_assignable&&) -> throw_copy_assignable& = default;
    };
    static_assert(std::is_nothrow_copy_constructible<variant<throw_copy_assignable>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int, throw_copy_assignable>>::value);

    struct throw_copyable {
        throw_copyable(const throw_copyable&);
        throw_copyable(throw_copyable&&) = default;
        auto operator=(const throw_copyable&) -> throw_copyable&;
        auto operator=(throw_copyable&&) -> throw_copyable& = default;
    };
    static_assert(!std::is_nothrow_copy_constructible<variant<int, throw_copyable>>::value);
    static_assert(std::is_copy_constructible<variant<int, throw_copyable>>::value);
    static_assert(std::is_nothrow_copy_constructible<variant<int, throw_copyable&>>::value);
}

TEST(VariantTestCopyConstructor, BasicBehavior) {
    {
        using v = variant<int>;
        v x1(std::in_place_index<0>, 3);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_TRUE(holds_alternative<int>(x1));
        EXPECT_EQ(get<int>(x1), 3);
        EXPECT_EQ(get<0>(x1), 3);
        v x2 = x1;
        EXPECT_EQ(x2.index(), 0);
        EXPECT_TRUE(holds_alternative<int>(x2));
        EXPECT_EQ(get<int>(x2), 3);
        EXPECT_EQ(get<0>(x2), 3);
    }
    {
        using v = variant<int, long>;
        v x1(std::in_place_index<1>, 3);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x1));
        EXPECT_EQ(get<long>(x1), 3);
        EXPECT_EQ(get<1>(x1), 3);
        v x2 = x1;
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x2));
        EXPECT_EQ(get<long>(x2), 3);
        EXPECT_EQ(get<1>(x2), 3);
    }
    {
        using v = variant<counter>;
        v x1;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_TRUE(holds_alternative<counter>(x1));
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        v x2 = x1;
        EXPECT_EQ(x2.index(), 0);
        EXPECT_TRUE(holds_alternative<counter>(x2));
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_construct_count, 1);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        counter::reset();
    }
    {
        using v = variant<int, counter>;
        v x1(std::in_place_index<1>);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<counter>(x1));
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        v x2 = x1;
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<counter>(x2));
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_construct_count, 1);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        counter::reset();
    }
    {
        using v = variant<int, valueless_t>;
        v x1;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_FALSE(x1.valueless_by_exception());
        make_valueless(x1);
        EXPECT_TRUE(x1.valueless_by_exception());
        v x2 = x1;
        EXPECT_TRUE(x2.valueless_by_exception());
    }
    {
        using v = variant<int const&>;
        int const data = 3;
        v x1 = data;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(&get<0>(x1), &data);
        v x2 = x1;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(&get<0>(x1), &data);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(&get<0>(x2), &data);
    }
    {
        using v = variant<int const&, double&>;
        double data = 3.0;
        v x1 = data;
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(&get<1>(x1), &data);
        v x2 = x1;
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(&get<1>(x1), &data);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(&get<1>(x2), &data);
    }
    {
        using v = variant<int const, double&>;
        double data = 3.0;
        v x1 = data;
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(&get<1>(x1), &data);
        v x2 = x1;
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(&get<1>(x1), &data);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(&get<1>(x2), &data);
    }
    {
        using v = variant<counter&>;
        counter c;
        v x1 = c;
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        EXPECT_EQ(counter::move_assign_count, 0);
        v x2 = x1;
        EXPECT_EQ(&get<0>(x1), &c);
        EXPECT_EQ(&get<0>(x2), &c);
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        EXPECT_EQ(counter::move_assign_count, 0);
        counter::reset();
    }
}

template <class Ty, class V>
constexpr auto constexpr_copy_construct_impl(
    V&& source,
    std::size_t expected_index,
    Ty expected_value
) -> bool {
    V x = source;
    return x.index() == expected_index && x.index() == source.index()
        && get<Ty>(x) == get<Ty>(source) && get<Ty>(x) == expected_value;
}

TEST(VariantTestCopyConstructor, Constexpr) {
    {
        using v = variant<int>;
        constexpr v x1(std::in_place_index<0>, 3);
        static_assert(x1.index() == 0);
        static_assert(holds_alternative<int>(x1));
        static_assert(get<int>(x1) == 3);
        static_assert(get<0>(x1) == 3);
        constexpr v x2 = x1;
        static_assert(x2.index() == 0);
        static_assert(holds_alternative<int>(x2));
        static_assert(get<int>(x2) == 3);
        static_assert(get<0>(x2) == 3);
    }
    {
        using v = variant<int, long>;
        constexpr v x1(std::in_place_index<1>, 3);
        static_assert(x1.index() == 1);
        static_assert(holds_alternative<long>(x1));
        static_assert(get<long>(x1) == 3l);
        static_assert(get<1>(x1) == 3l);
        constexpr v x2 = x1;
        static_assert(x2.index() == 1);
        static_assert(holds_alternative<long>(x2));
        static_assert(get<long>(x2) == 3l);
        static_assert(get<1>(x2) == 3l);
    }
    {
        struct copyable {
            int value;
            constexpr copyable(int val) : value(val) { }
            copyable(const copyable&) = default;
            copyable(copyable&&) = delete;
        };

        {
            using v = variant<copyable>;
            constexpr v x1(std::in_place_index<0>, 3);
            static_assert(x1.index() == 0);
            static_assert(holds_alternative<copyable>(x1));
            static_assert(get<copyable>(x1).value == 3);
            static_assert(get<0>(x1).value == 3);
            constexpr v x2 = x1;
            static_assert(x2.index() == 0);
            static_assert(holds_alternative<copyable>(x2));
            static_assert(get<copyable>(x2).value == 3);
            static_assert(get<0>(x2).value == 3);
        }
        {
            using v = variant<int, copyable>;
            constexpr v x1(std::in_place_index<1>, 3);
            static_assert(x1.index() == 1);
            static_assert(holds_alternative<copyable>(x1));
            static_assert(get<copyable>(x1).value == 3);
            static_assert(get<1>(x1).value == 3);
            constexpr v x2 = x1;
            static_assert(x2.index() == 1);
            static_assert(holds_alternative<copyable>(x2));
            static_assert(get<copyable>(x2).value == 3);
            static_assert(get<1>(x2).value == 3);
        }
    }
    {
        using v = variant<double&>;
        static double data = 3.0;
        constexpr v x1 = data;
        static_assert(x1.index() == 0);
        static_assert(&get<double&>(x1) == &data);
        constexpr v x2 = x1;
        static_assert(x2.index() == 0);
        static_assert(&get<double&>(x2) == &data);
    }
    {
        using v = variant<int const&, double&>;
        static int const data = 3;
        constexpr v x1 = data;
        static_assert(x1.index() == 0);
        static_assert(&get<0>(x1) == &data);
        constexpr v x2 = x1;
        static_assert(x1.index() == 0);
        static_assert(&get<0>(x1) == &data);
        static_assert(x2.index() == 0);
        static_assert(&get<0>(x2) == &data);
    }
    {
        using v = variant<long, void*, const int>;
        static_assert(constexpr_copy_construct_impl(v(3l), 0, 3l));
        static_assert(constexpr_copy_construct_impl(v(nullptr), 1, static_cast<void*>(nullptr)));
        static_assert(constexpr_copy_construct_impl<const int>(v(3), 2, 3));
    }
}
}  // namespace
}  // namespace rust
