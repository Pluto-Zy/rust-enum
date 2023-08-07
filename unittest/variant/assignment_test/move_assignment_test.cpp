#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

namespace rust {
namespace {
TEST(VariantTestMoveAssignment, Deleted) {
    static_assert(std::is_move_assignable<variant<int>>::value);
    static_assert(std::is_move_assignable<variant<int, double>>::value);
    static_assert(!std::is_move_assignable<variant<const int>>::value);
    static_assert(!std::is_move_assignable<variant<int, const double>>::value);
    static_assert(std::is_move_assignable<variant<monostate>>::value);
    static_assert(std::is_move_assignable<variant<int, monostate>>::value);

    static_assert(std::is_move_assignable<variant<int&>>::value);
    static_assert(std::is_move_assignable<variant<int&, int&>>::value);
    static_assert(std::is_move_assignable<variant<const int&>>::value);
    static_assert(std::is_move_assignable<variant<int&, int>>::value);
    static_assert(!std::is_move_assignable<variant<int&, const int>>::value);
    static_assert(std::is_move_assignable<variant<int&, const int&>>::value);

    struct move_only {
        move_only(const move_only&) = delete;
        move_only(move_only&&) = default;
        auto operator=(const move_only&) -> move_only& = delete;
        auto operator=(move_only&&) -> move_only& = default;
    };
    static_assert(!std::is_copy_assignable<variant<int, move_only>>::value);
    static_assert(std::is_move_assignable<variant<int, move_only>>::value);

    struct move_construct_only {
        move_construct_only(const move_construct_only&) = delete;
        move_construct_only(move_construct_only&&) = default;
        auto operator=(const move_construct_only&) -> move_construct_only& = delete;
        auto operator=(move_construct_only&&) -> move_construct_only& = delete;
    };
    static_assert(!std::is_copy_assignable<variant<int, move_construct_only>>::value);
    static_assert(!std::is_move_assignable<variant<int, move_construct_only>>::value);
    static_assert(std::is_move_assignable<variant<int, move_construct_only&>>::value);

    struct move_assign_only {
        move_assign_only(const move_assign_only&) = delete;
        move_assign_only(move_assign_only&&) = delete;
        auto operator=(const move_assign_only&) -> move_assign_only& = delete;
        auto operator=(move_assign_only&&) -> move_assign_only& = default;
    };
    static_assert(!std::is_copy_assignable<variant<int, move_assign_only>>::value);
    static_assert(!std::is_move_assignable<variant<int, move_assign_only>>::value);
    static_assert(std::is_move_assignable<variant<int, move_assign_only&>>::value);

    struct non_moveable {
        non_moveable() = default;
        non_moveable(const non_moveable&) = default;
        non_moveable(non_moveable&&) = delete;
        auto operator=(const non_moveable&) -> non_moveable& = default;
        auto operator=(non_moveable&&) -> non_moveable& = delete;
    };
    static_assert(std::is_copy_assignable<variant<int, non_moveable>>::value);
    // use copy assignment
    static_assert(std::is_move_assignable<variant<int, non_moveable>>::value);
    static_assert(std::is_copy_assignable<variant<int, non_moveable&>>::value);

    {
        using v = variant<counter, non_moveable>;
        static_assert(std::is_copy_assignable<v>::value);
        static_assert(std::is_move_assignable<v>::value);

        v x1(std::in_place_index<1>);
        v x2(std::in_place_index<0>);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);

        x1 = std::move(x2);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_construct_count, 1);
        EXPECT_EQ(counter::move_construct_count, 0);
    }
}

TEST(VariantTestMoveAssignment, Trivial) {
    static_assert(std::is_trivially_move_assignable<variant<int>>::value);
    static_assert(std::is_trivially_move_assignable<variant<int, long>>::value);
    static_assert(std::is_trivially_move_assignable<variant<monostate>>::value);
    static_assert(std::is_trivially_move_assignable<variant<int, monostate>>::value);

    static_assert(std::is_trivially_move_assignable<variant<int&>>::value);
    static_assert(std::is_trivially_move_assignable<variant<int const&>>::value);
    static_assert(std::is_trivially_move_assignable<variant<int&, float&>>::value);
    static_assert(std::is_trivially_move_assignable<variant<int&, float>>::value);

    struct trivially_moveable {
        trivially_moveable(const trivially_moveable&) = default;
        trivially_moveable(trivially_moveable&&) = default;
        auto operator=(const trivially_moveable&) -> trivially_moveable& = default;
        auto operator=(trivially_moveable&&) -> trivially_moveable& = default;
    };
    static_assert(std::is_trivially_move_assignable<variant<int, trivially_moveable>>::value);

    struct non_trivially_move_constructible {
        non_trivially_move_constructible(const non_trivially_move_constructible&) = default;
        non_trivially_move_constructible(non_trivially_move_constructible&&) { }
        auto operator=(const non_trivially_move_constructible&)
            -> non_trivially_move_constructible& = default;
        auto operator=(non_trivially_move_constructible&&)
            -> non_trivially_move_constructible& = default;
        ~non_trivially_move_constructible() = default;
    };
    static_assert(
        !std::is_trivially_move_assignable<variant<int, non_trivially_move_constructible>>::value
    );
    static_assert(std::is_move_assignable<variant<int, non_trivially_move_constructible>>::value);
    static_assert(
        std::is_trivially_move_assignable<variant<int, non_trivially_move_constructible&>>::value
    );
    static_assert(  //
        !std::is_trivially_move_assignable<variant<  //
            int,
            non_trivially_move_constructible,
            non_trivially_move_constructible&>>::value
    );

    struct non_trivially_move_assignable {
        non_trivially_move_assignable(const non_trivially_move_assignable&) = default;
        non_trivially_move_assignable(non_trivially_move_assignable&&) = default;
        auto operator=(const non_trivially_move_assignable&)
            -> non_trivially_move_assignable& = default;
        auto operator=(non_trivially_move_assignable&&) -> non_trivially_move_assignable& {
            return *this;
        }
        ~non_trivially_move_assignable() = default;
    };
    static_assert(
        !std::is_trivially_move_assignable<variant<int, non_trivially_move_assignable>>::value
    );
    static_assert(std::is_move_assignable<variant<int, non_trivially_move_assignable>>::value);
    static_assert(
        std::is_trivially_move_assignable<variant<int, non_trivially_move_assignable&>>::value
    );
    static_assert(
        !std::is_trivially_move_assignable<
            variant<int, non_trivially_move_assignable, non_trivially_move_assignable&>>::value
    );

    struct non_trivially_destructible {
        non_trivially_destructible(const non_trivially_destructible&) = default;
        non_trivially_destructible(non_trivially_destructible&&) = default;
        auto operator=(const non_trivially_destructible&) -> non_trivially_destructible& = default;
        auto operator=(non_trivially_destructible&&) -> non_trivially_destructible& = default;
        ~non_trivially_destructible() { }
    };
    static_assert(
        !std::is_trivially_move_assignable<variant<int, non_trivially_destructible>>::value
    );
    static_assert(std::is_move_assignable<variant<int, non_trivially_destructible>>::value);
    static_assert(
        std::is_trivially_move_assignable<variant<int, non_trivially_destructible&>>::value
    );
    static_assert(!std::is_trivially_move_assignable<
                  variant<int, non_trivially_destructible, non_trivially_destructible&>>::value);

    // clang-format off
    struct trivially_move_assignable_non_trivially_copyable {
        trivially_move_assignable_non_trivially_copyable(
            const trivially_move_assignable_non_trivially_copyable&
        ) { }
        trivially_move_assignable_non_trivially_copyable(
            trivially_move_assignable_non_trivially_copyable&&
        ) = default;
        auto operator=(
            const trivially_move_assignable_non_trivially_copyable&
        ) -> trivially_move_assignable_non_trivially_copyable& {
            return *this;
        }
        auto operator=(
            trivially_move_assignable_non_trivially_copyable&&
        ) -> trivially_move_assignable_non_trivially_copyable& = default;
    };
    // clang-format on
    static_assert(std::is_trivially_move_assignable<
                  variant<int, trivially_move_assignable_non_trivially_copyable>>::value);
    static_assert(std::is_trivially_move_assignable<
                  variant<int, trivially_move_assignable_non_trivially_copyable&>>::value);
}

TEST(VariantTestMoveAssignment, Noexcept) {
    static_assert(std::is_nothrow_move_assignable<variant<int>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int, double>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<monostate>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int, monostate>>::value);

    static_assert(std::is_nothrow_move_assignable<variant<int&>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int const&>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int&, float&>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int&, double>>::value);

    struct nothrow_moveable {
        nothrow_moveable(const nothrow_moveable&) = default;
        nothrow_moveable(nothrow_moveable&&) noexcept = default;
        auto operator=(const nothrow_moveable&) -> nothrow_moveable& = default;
        auto operator=(nothrow_moveable&&) noexcept -> nothrow_moveable& = default;
    };
    static_assert(std::is_nothrow_move_assignable<variant<int, nothrow_moveable>>::value);

    struct throw_move_constructible {
        throw_move_constructible(const throw_move_constructible&) = default;
        throw_move_constructible(throw_move_constructible&&);
        auto operator=(const throw_move_constructible&) -> throw_move_constructible& = default;
        auto operator=(throw_move_constructible&&) noexcept -> throw_move_constructible& = default;
    };
    static_assert(!std::is_nothrow_move_assignable<variant<int, throw_move_constructible>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int, throw_move_constructible&>>::value);
    static_assert(!std::is_nothrow_move_assignable<
                  variant<int, throw_move_constructible, throw_move_constructible&>>::value);

    struct throw_move_assignable {
        throw_move_assignable(const throw_move_assignable&) = default;
        throw_move_assignable(throw_move_assignable&&) noexcept = default;
        auto operator=(const throw_move_assignable&) -> throw_move_assignable& = default;
        auto operator=(throw_move_assignable&&) -> throw_move_assignable&;
    };
    static_assert(!std::is_nothrow_move_assignable<variant<int, throw_move_assignable>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int, throw_move_assignable&>>::value);
    static_assert(!std::is_nothrow_move_assignable<
                  variant<int, throw_move_assignable, throw_move_assignable&>>::value);

    struct throw_moveable {
        throw_moveable(const throw_moveable&) = default;
        throw_moveable(throw_moveable&&);
        auto operator=(const throw_moveable&) -> throw_moveable& = default;
        auto operator=(throw_moveable&&) -> throw_moveable&;
    };
    static_assert(!std::is_nothrow_move_assignable<variant<int, throw_moveable>>::value);
    static_assert(std::is_nothrow_move_assignable<variant<int, throw_moveable&>>::value);
    static_assert(
        !std::is_nothrow_move_assignable<variant<int, throw_moveable, throw_moveable&>>::value
    );
}

TEST(VariantTestMoveAssignment, BasicBehavior) {
    {
        using v = variant<int, valueless_t>;
        static_assert(!detail::variant_no_valueless_state<v>::value);
        v x1, x2;
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        make_valueless(x1);
        make_valueless(x2);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
    }
    {
        using v = variant<counter, valueless_t>;
        static_assert(!detail::variant_no_valueless_state<v>::value);
        v x1, x2;
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(counter::alive_count, 2);

        make_valueless(x2);
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        EXPECT_EQ(counter::alive_count, 0);
        counter::reset();
    }
    {
        using v = variant<counter, valueless_t>;
        static_assert(!detail::variant_no_valueless_state<v>::value);
        v x1, x2;
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(counter::alive_count, 2);

        make_valueless(x1);
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_TRUE(holds_alternative<counter>(x1));
        EXPECT_TRUE(holds_alternative<counter>(x2));
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::move_construct_count, 1);
        counter::reset();
    }
    {
        using v = variant<int>;
        v x1(2), x2(3);
        EXPECT_EQ(get<0>(x1), 2);
        EXPECT_EQ(get<0>(x2), 3);
        x1 = std::move(x2);
        EXPECT_EQ(get<0>(x1), 3);
        EXPECT_EQ(get<0>(x2), 3);
    }
    {
        using v = variant<counter>;
        v x1, x2;
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::move_assign_count, 0);
        x1 = std::move(x2);
        EXPECT_EQ(counter::move_assign_count, 1);
        counter::reset();
    }
    {
        using v = variant<int, long>;
        v x1(std::in_place_index<1>, 3);
        v x2(std::in_place_index<1>, 4);
        EXPECT_EQ(get<1>(x1), 3);
        EXPECT_EQ(get<1>(x2), 4);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(get<1>(x1), 4);
        EXPECT_EQ(get<1>(x2), 4);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<long>(x1));
        EXPECT_TRUE(holds_alternative<long>(x2));
    }
    {
        using v = variant<int, std::string>;
        v x1(std::in_place_index<1>, "abc");
        v x2(std::in_place_index<1>, "bcd");
        EXPECT_EQ(get<1>(x1), "abc");
        EXPECT_EQ(get<1>(x2), "bcd");

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(get<1>(x1), "bcd");
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(holds_alternative<std::string>(x1));
        EXPECT_TRUE(holds_alternative<std::string>(x2));
    }
    {
        using v = variant<int, counter>;
        v x1(std::in_place_index<1>);
        v x2(std::in_place_index<1>);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_assign_count, 0);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_assign_count, 1);
        counter::reset();
    }
    {
        using v = variant<int, valueless_t>;
        static_assert(!detail::variant_no_valueless_state<v>::value);
        v x1(std::in_place_index<1>);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());

        EXPECT_THROW((x1 = std::move(x2)), int);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
    }
    {
        using v = variant<int, counter>;
        v x1(std::in_place_index<1>);
        v x2(std::in_place_index<0>, 4);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(counter::alive_count, 1);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(get<0>(x1), 4);
        EXPECT_EQ(get<0>(x2), 4);
        EXPECT_EQ(counter::alive_count, 0);
        counter::reset();
    }
    {
        struct nothrow_move_constructible {
            bool copy_ctor = false;
            bool move_ctor = false;

            nothrow_move_constructible() = default;
            nothrow_move_constructible(const nothrow_move_constructible&) {
                copy_ctor = true;
            }
            nothrow_move_constructible(nothrow_move_constructible&&) noexcept {
                move_ctor = true;
            }
            auto operator=(const nothrow_move_constructible&)
                -> nothrow_move_constructible& = default;
            auto operator=(nothrow_move_constructible&&) -> nothrow_move_constructible& = default;
        };

        using v = variant<int, nothrow_move_constructible>;
        static_assert(detail::variant_no_valueless_state<v>::value);
        v x1(std::in_place_index<0>);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_FALSE(get<1>(x1).copy_ctor);
        EXPECT_TRUE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int&>;
        int val1 = 3, val2 = 4;
        v x1 = val1;
        EXPECT_EQ(&get<0>(x1), &val1);
        v x2 = val2;
        EXPECT_EQ(&get<0>(x2), &val2);

        x1 = std::move(x2);
        EXPECT_EQ(&get<0>(x1), &val2);
        EXPECT_EQ(&get<0>(x2), &val2);
    }
    {
        using v = variant<int&, float&>;
        int val1 = 3;
        float val2 = 4;
        v x1 = val1;
        EXPECT_EQ(&get<0>(x1), &val1);
        v x2 = val2;
        EXPECT_EQ(&get<1>(x2), &val2);

        x1 = std::move(x2);
        EXPECT_EQ(&get<1>(x1), &val2);
        EXPECT_EQ(&get<1>(x2), &val2);
    }
    {
        using v = variant<int, float&>;
        int val1 = 3;
        float val2 = 4;
        v x1 = val1;
        EXPECT_NE(&get<0>(x1), &val1);
        val1 = 5;
        EXPECT_EQ(get<0>(x1), 3);
        v x2 = val2;
        EXPECT_EQ(&get<1>(x2), &val2);

        x1 = std::move(x2);
        EXPECT_EQ(&get<1>(x1), &val2);
        EXPECT_EQ(&get<1>(x2), &val2);
    }
    {
        using v = variant<counter&, int>;
        counter c;
        v x1 = 3, x2 = c;
        EXPECT_EQ(counter::alive_count, 1);
        EXPECT_EQ(counter::copy_construct_count, 0);
        EXPECT_EQ(counter::copy_assign_count, 0);
        EXPECT_EQ(counter::move_construct_count, 0);
        EXPECT_EQ(counter::move_assign_count, 0);
        x1 = std::move(x2);
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

template <class V1, class V2, class Ty>
constexpr auto constexpr_move_assign_impl(
    V1&& lhs,
    V2&& rhs,
    std::size_t expected_index,
    Ty expected_value
) -> bool {
    lhs = std::move(rhs);
    return lhs.index() == expected_index && get<Ty>(lhs) == expected_value;
}

TEST(VariantTestMoveAssignment, Constexpr) {
    using v = variant<long, void*, int>;
    static_assert(std::is_move_assignable<v>::value);
    static_assert(constexpr_move_assign_impl(v(3), v(4), 2, 4));
    static_assert(constexpr_move_assign_impl(v(3l), v(4l), 0, 4l));
    static_assert(constexpr_move_assign_impl(v(nullptr), v(4l), 0, 4l));
    static_assert(constexpr_move_assign_impl(v(3l), v(nullptr), 1, static_cast<void*>(nullptr)));
    static_assert(constexpr_move_assign_impl(v(3l), v(4), 2, 4));
    static_assert(constexpr_move_assign_impl(v(3), v(4l), 0, 4l));
}

TEST(VariantTestMoveAssignment, ValuelessStrategy) {
    {
        using v = variant<int, may_throw_move_constructible_strategy_default>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::LET_VARIANT_DECIDE
        );
        static_assert(!detail::variant_no_valueless_state<v>::value);

        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_FALSE(get<1>(x1).copy_ctor);
        EXPECT_TRUE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int, throw_move_constructible_strategy_default>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::LET_VARIANT_DECIDE
        );
        static_assert(!detail::variant_no_valueless_state<v>::value);

        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        EXPECT_THROW(x1 = std::move(x2), int);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x2.index(), 1);
    }
    {
        using v = variant<int, may_throw_move_constructible_strategy_fallback>;
        static_assert(
            variant_valueless_strategy<v>::value == variant_valueless_strategy_t::FALLBACK
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_FALSE(get<1>(x1).copy_ctor);
        EXPECT_TRUE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int, throw_move_constructible_strategy_fallback>;
        static_assert(
            variant_valueless_strategy<v>::value == variant_valueless_strategy_t::FALLBACK
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(get<0>(x1), 4);
        EXPECT_EQ(x2.index(), 1);

        EXPECT_THROW(x1 = std::move(x2), int);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(get<0>(x1), 0);
    }
    {
        using v = variant<int, may_throw_move_constructible_strategy_novalueless>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::DISALLOW_VALUELESS
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = std::move(x2));
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_FALSE(get<1>(x1).copy_ctor);
        EXPECT_TRUE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int, throw_move_constructible_strategy_novalueless>;
        static_assert(
            variant_valueless_strategy<v>::value
            == variant_valueless_strategy_t::DISALLOW_VALUELESS
        );
        static_assert(detail::variant_no_valueless_state<v>::value);

        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        EXPECT_THROW(x1 = std::move(x2), int);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x2.index(), 1);
    }
}
}  // namespace
}  // namespace rust
