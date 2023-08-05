#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestMoveConstructor, Deleted) {
    static_assert(std::is_move_constructible<variant<int>>::value);
    static_assert(std::is_move_constructible<variant<int, long>>::value);
    static_assert(std::is_move_constructible<variant<const int>>::value);
    static_assert(std::is_move_constructible<variant<int, const double>>::value);
    static_assert(std::is_move_constructible<variant<monostate>>::value);
    static_assert(std::is_move_constructible<variant<int, monostate>>::value);

    static_assert(std::is_move_constructible<variant<int&>>::value);
    static_assert(std::is_move_constructible<variant<float&>>::value);
    static_assert(std::is_move_constructible<variant<int&, float>>::value);
    static_assert(std::is_move_constructible<variant<int&, float&>>::value);
    static_assert(std::is_move_constructible<variant<std::string&>>::value);
    static_assert(std::is_move_constructible<variant<int const&>>::value);
    static_assert(std::is_move_constructible<variant<int&, float const>>::value);

    {
        struct non_moveable {
            non_moveable(const non_moveable&) = default;
            non_moveable(non_moveable&&) = delete;
            auto operator=(const non_moveable&) -> non_moveable& = default;
            auto operator=(non_moveable&&) -> non_moveable& = default;
        };
        static_assert(std::is_move_constructible<variant<non_moveable>>::value);
        static_assert(std::is_move_constructible<variant<int, non_moveable>>::value);
    }
    {
        struct non_copy_moveable {
            non_copy_moveable(const non_copy_moveable&) = delete;
            non_copy_moveable(non_copy_moveable&&) = delete;
            auto operator=(const non_copy_moveable&) -> non_copy_moveable& = default;
            auto operator=(non_copy_moveable&&) -> non_copy_moveable& = default;
        };
        static_assert(!std::is_move_constructible<variant<non_copy_moveable>>::value);
        static_assert(!std::is_move_constructible<variant<int, non_copy_moveable>>::value);
        static_assert(std::is_move_constructible<variant<non_copy_moveable&>>::value);
        static_assert(
            !std::is_move_constructible<variant<non_copy_moveable, non_copy_moveable&>>::value
        );
    }
    {
        struct non_move_assignable {
            non_move_assignable(const non_move_assignable&) = default;
            non_move_assignable(non_move_assignable&&) = default;
            auto operator=(const non_move_assignable&) -> non_move_assignable& = default;
            auto operator=(non_move_assignable&&) -> non_move_assignable& = delete;
        };
        static_assert(std::is_move_constructible<variant<non_move_assignable>>::value);
        static_assert(std::is_move_constructible<variant<int, non_move_assignable>>::value);
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
        static_assert(std::is_move_constructible<variant<move_only>>::value);
        static_assert(std::is_move_constructible<variant<int, move_only>>::value);
    }
    {
        struct copy_only {
            copy_only(const copy_only&) = default;
            copy_only(copy_only&&) = delete;
            auto operator=(const copy_only&) -> copy_only& = default;
            auto operator=(copy_only&&) -> copy_only& = delete;
        };
        static_assert(std::is_move_constructible<variant<copy_only>>::value);
        static_assert(std::is_move_constructible<variant<int, copy_only>>::value);
    }
    {
        struct move_construct_only {
            move_construct_only(const move_construct_only&) = delete;
            move_construct_only(move_construct_only&&) = default;
            auto operator=(const move_construct_only&) -> move_construct_only& = delete;
            auto operator=(move_construct_only&&) -> move_construct_only& = delete;
        };
        static_assert(std::is_move_constructible<variant<move_construct_only>>::value);
        static_assert(std::is_move_constructible<variant<int, move_construct_only>>::value);
    }
}

TEST(VariantTestMoveConstructor, Trivial) {
    static_assert(std::is_trivially_move_constructible<variant<int>>::value);
    static_assert(std::is_trivially_move_constructible<variant<int, long>>::value);
    static_assert(std::is_trivially_move_constructible<variant<const int>>::value);
    static_assert(std::is_trivially_move_constructible<variant<int, const double>>::value);
    static_assert(std::is_trivially_move_constructible<variant<monostate>>::value);
    static_assert(std::is_trivially_move_constructible<variant<int, monostate>>::value);

    static_assert(std::is_trivially_move_constructible<variant<int&>>::value);
    static_assert(std::is_trivially_move_constructible<variant<float&>>::value);
    static_assert(std::is_trivially_move_constructible<variant<int&, float>>::value);
    static_assert(std::is_trivially_move_constructible<variant<int&, float&>>::value);
    static_assert(std::is_trivially_move_constructible<variant<std::string&>>::value);
    static_assert(  //
        !std::is_trivially_move_constructible<variant<std::string&, std::string>>::value
    );

    {
        struct trivially_moveable {
            trivially_moveable(const trivially_moveable&);
            trivially_moveable(trivially_moveable&&) = default;
            auto operator=(const trivially_moveable&) -> trivially_moveable&;
            auto operator=(trivially_moveable&&) -> trivially_moveable& = default;
        };
        static_assert(std::is_trivially_move_constructible<variant<trivially_moveable>>::value);
        static_assert(  //
            std::is_trivially_move_constructible<variant<trivially_moveable, int>>::value
        );
    }
    {
        struct trivially_move_constructible {
            trivially_move_constructible(const trivially_move_constructible&);
            trivially_move_constructible(trivially_move_constructible&&) = default;
            auto operator=(const trivially_move_constructible&) -> trivially_move_constructible&;
            auto operator=(trivially_move_constructible&&) -> trivially_move_constructible&;
        };
        static_assert(
            std::is_trivially_move_constructible<variant<trivially_move_constructible>>::value
        );
        static_assert(
            std::is_trivially_move_constructible<variant<trivially_move_constructible, int>>::value
        );
    }
    {
        struct trivially_move_assignable {
            trivially_move_assignable(const trivially_move_assignable&);
            trivially_move_assignable(trivially_move_assignable&&);
            auto operator=(const trivially_move_assignable&) -> trivially_move_assignable&;
            auto operator=(trivially_move_assignable&&) -> trivially_move_assignable& = default;
        };
        static_assert(
            !std::is_trivially_move_constructible<variant<trivially_move_assignable>>::value
        );
        static_assert(
            !std::is_trivially_move_constructible<variant<trivially_move_assignable, int>>::value
        );
        static_assert(std::is_move_constructible<variant<trivially_move_assignable>>::value);
        static_assert(std::is_move_constructible<variant<trivially_move_assignable, int>>::value);
        static_assert(
            std::is_trivially_move_constructible<variant<trivially_move_assignable&>>::value
        );
        static_assert(
            std::is_trivially_move_constructible<variant<trivially_move_assignable&, int>>::value
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
            std::is_trivially_move_constructible<variant<non_trivially_copyable>>::value
        );
        static_assert(
            std::is_trivially_move_constructible<variant<non_trivially_copyable, int>>::value
        );
    }
}

TEST(VariantTestMoveConstructor, Noexcept) {
    static_assert(std::is_nothrow_move_constructible<variant<int>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int, long>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<const int>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int, const double>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<monostate>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int, monostate>>::value);

    static_assert(std::is_nothrow_move_constructible<variant<int&>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<float&>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int&, float>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int&, float&>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<std::string&>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<std::string&, std::string>>::value);

    struct nothrow_moveable {
        nothrow_moveable(const nothrow_moveable&);
        nothrow_moveable(nothrow_moveable&&) = default;
        auto operator=(const nothrow_moveable&) -> nothrow_moveable&;
        auto operator=(nothrow_moveable&&) -> nothrow_moveable&;
    };
    static_assert(std::is_nothrow_move_constructible<variant<nothrow_moveable>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int, nothrow_moveable>>::value);

    struct throw_move_constructible {
        throw_move_constructible(const throw_move_constructible&) = default;
        throw_move_constructible(throw_move_constructible&&);
        auto operator=(const throw_move_constructible&) -> throw_move_constructible& = default;
        auto operator=(throw_move_constructible&&) -> throw_move_constructible& = default;
    };
    static_assert(!std::is_nothrow_move_constructible<variant<throw_move_constructible>>::value);
    static_assert(
        !std::is_nothrow_move_constructible<variant<int, throw_move_constructible>>::value
    );
    static_assert(std::is_move_constructible<variant<throw_move_constructible>>::value);
    static_assert(std::is_move_constructible<variant<int, throw_move_constructible>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<throw_move_constructible&>>::value);
    static_assert(
        std::is_nothrow_move_constructible<variant<int, throw_move_constructible&>>::value
    );

    struct throw_move_assignable {
        throw_move_assignable(const throw_move_assignable&) = default;
        throw_move_assignable(throw_move_assignable&&) = default;
        auto operator=(const throw_move_assignable&) -> throw_move_assignable& = default;
        auto operator=(throw_move_assignable&&) -> throw_move_assignable&;
    };
    static_assert(std::is_nothrow_move_constructible<variant<throw_move_assignable>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int, throw_move_assignable>>::value);

    struct throw_moveable {
        throw_moveable(const throw_moveable&) = default;
        throw_moveable(throw_moveable&&);
        auto operator=(const throw_moveable&) -> throw_moveable& = default;
        auto operator=(throw_moveable&&) -> throw_moveable&;
    };
    static_assert(!std::is_nothrow_move_constructible<variant<int, throw_moveable>>::value);
    static_assert(std::is_move_constructible<variant<int, throw_moveable>>::value);
    static_assert(std::is_nothrow_move_constructible<variant<int, throw_moveable&>>::value);
}

TEST(VariantTestMoveConstructor, BasicBehavior) {
    {
        using v = variant<int>;
        v x1(std::in_place_index<0>, 3);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_TRUE(holds_alternative<int>(x1));
        EXPECT_EQ(get<int>(x1), 3);
        EXPECT_EQ(get<0>(x1), 3);
        v x2 = std::move(x1);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_TRUE(holds_alternative<int>(x2));
        EXPECT_EQ(get<int>(x2), 3);
        EXPECT_EQ(get<0>(x2), 3);
        EXPECT_EQ(x1.index(), 0);
    }
    {
        using v = variant<int, long>;
        v x1(std::in_place_index<1>, 3);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x1));
        EXPECT_EQ(get<long>(x1), 3);
        EXPECT_EQ(get<1>(x1), 3);
        v x2 = std::move(x1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x2));
        EXPECT_EQ(get<long>(x2), 3);
        EXPECT_EQ(get<1>(x2), 3);
        EXPECT_EQ(x1.index(), 1);
    }
    {
        using v = variant<counter>;
        v x1;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_TRUE(holds_alternative<counter>(x1));
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        EXPECT_EQ(counter::move_assign_count, 0);
        v x2 = std::move(x1);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_TRUE(holds_alternative<counter>(x2));
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::move_construct_count, 1);
        EXPECT_EQ(counter::move_assign_count, 0);
        counter::reset();
    }
    {
        using v = variant<int, counter>;
        v x1(std::in_place_index<1>);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_TRUE(holds_alternative<counter>(x1));
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        EXPECT_EQ(counter::move_assign_count, 0);
        v x2 = std::move(x1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<counter>(x2));
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::move_construct_count, 1);
        EXPECT_EQ(counter::move_assign_count, 0);
        counter::reset();
    }
    {
        using v = variant<int, valueless_t>;
        v x1;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_FALSE(x1.valueless_by_exception());
        make_valueless(x1);
        EXPECT_TRUE(x1.valueless_by_exception());
        v x2 = std::move(x1);
        EXPECT_TRUE(x2.valueless_by_exception());
    }
    {
        using v = variant<int const&>;
        int const data = 3;
        v x1 = data;
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(&get<0>(x1), &data);
        v x2 = std::move(x1);
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
        v x2 = std::move(x1);
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
        v x2 = std::move(x1);
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
        v x2 = std::move(x1);
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
constexpr auto constexpr_move_construct_impl(
    V&& source,
    std::size_t expected_index,
    Ty expected_value
) -> bool {
    V x = std::move(source);
    return x.index() == expected_index && x.index() == source.index()
        && get<Ty>(x) == get<Ty>(source) && get<Ty>(x) == expected_value;
}

TEST(VariantTestMoveConstructor, Constexpr) {
    {
        using v = variant<int>;
        static_assert(constexpr_move_construct_impl(v(std::in_place_index<0>, 3), 0, 3));
    }
    {
        using v = variant<int, long>;
        static_assert(constexpr_move_construct_impl(v(std::in_place_index<1>, 3), 1, 3l));
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
            // `static_cast` is used to convert prvalue to xvalue
            constexpr v x2 = static_cast<v&&>(v(std::in_place_index<0>, 3));
            static_assert(x2.index() == 0);
            static_assert(holds_alternative<copyable>(x2));
            static_assert(get<copyable>(x2).value == 3);
            static_assert(get<0>(x2).value == 3);
        }
        {
            using v = variant<int, copyable>;
            constexpr v x2 = static_cast<v&&>(v(std::in_place_index<1>, 3));
            static_assert(x2.index() == 1);
            static_assert(holds_alternative<copyable>(x2));
            static_assert(get<copyable>(x2).value == 3);
            static_assert(get<1>(x2).value == 3);
        }
    }
    {
        using v = variant<long, void*, const int>;
        static_assert(constexpr_move_construct_impl(v(3l), 0, 3l));
        static_assert(constexpr_move_construct_impl(v(nullptr), 1, static_cast<void*>(nullptr)));
        static_assert(constexpr_move_construct_impl<const int>(v(3), 2, 3));
    }
    {
        using v = variant<double&>;
        static double data = 3.0;
        constexpr v x = static_cast<v&&>(data);
        static_assert(x.index() == 0);
        static_assert(&get<double&>(x) == &data);
    }
    {
        using v = variant<int const&, double&>;
        static int const data = 3;
        constexpr v x = static_cast<v&&>(data);
        static_assert(x.index() == 0);
        static_assert(&get<0>(x) == &data);
    }
}
}  // namespace
}  // namespace rust
