#include <common.hpp>
#include <type_traits>
#include <valueless_strategy_test_helper.hpp>

namespace rust {
namespace {
TEST(VariantTestCopyAssignment, Deleted) {
    static_assert(std::is_copy_assignable<variant<int>>::value);
    static_assert(std::is_copy_assignable<variant<int, double>>::value);
    static_assert(!std::is_copy_assignable<variant<const int>>::value);
    static_assert(!std::is_copy_assignable<variant<int, const double>>::value);
    static_assert(std::is_copy_assignable<variant<monostate>>::value);
    static_assert(std::is_copy_assignable<variant<int, monostate>>::value);

    static_assert(std::is_copy_assignable<variant<int&>>::value);
    static_assert(std::is_copy_assignable<variant<int&, int&>>::value);
    static_assert(std::is_copy_assignable<variant<const int&>>::value);
    static_assert(std::is_copy_assignable<variant<int&, int>>::value);
    static_assert(!std::is_copy_assignable<variant<int&, const int>>::value);
    static_assert(std::is_copy_assignable<variant<int&, const int&>>::value);

    struct copy_only {
        copy_only(const copy_only&) = default;
        copy_only(copy_only&&) = delete;
        auto operator=(const copy_only&) -> copy_only& = default;
        auto operator=(copy_only&&) -> copy_only& = delete;
    };
    static_assert(std::is_copy_assignable<variant<int, copy_only>>::value);

    struct copy_construct_only {
        copy_construct_only(const copy_construct_only&) = default;
        copy_construct_only(copy_construct_only&&) = delete;
        auto operator=(const copy_construct_only&) -> copy_construct_only& = delete;
        auto operator=(copy_construct_only&&) -> copy_construct_only& = delete;
    };
    static_assert(!std::is_copy_assignable<variant<int, copy_construct_only>>::value);
    static_assert(std::is_copy_assignable<variant<int, copy_construct_only&>>::value);

    struct copy_assign_only {
        copy_assign_only(const copy_assign_only&) = delete;
        copy_assign_only(copy_assign_only&&) = delete;
        auto operator=(const copy_assign_only&) -> copy_assign_only& = default;
        auto operator=(copy_assign_only&&) -> copy_assign_only& = delete;
    };
    static_assert(!std::is_copy_assignable<variant<int, copy_assign_only>>::value);
    static_assert(std::is_copy_assignable<variant<int, copy_assign_only&>>::value);

    struct non_copyable {
        non_copyable(const non_copyable&) = delete;
        non_copyable(non_copyable&&) = default;
        auto operator=(const non_copyable&) -> non_copyable& = delete;
        auto operator=(non_copyable&&) -> non_copyable& = default;
    };
    static_assert(!std::is_copy_assignable<variant<int, non_copyable>>::value);
    static_assert(std::is_copy_assignable<variant<int, non_copyable&>>::value);
}

TEST(VariantTestCopyAssignment, Trivial) {
    static_assert(std::is_trivially_copy_assignable<variant<int>>::value);
    static_assert(std::is_trivially_copy_assignable<variant<int, long>>::value);
    static_assert(std::is_trivially_copy_assignable<variant<monostate>>::value);
    static_assert(std::is_trivially_copy_assignable<variant<int, monostate>>::value);

    static_assert(std::is_trivially_copy_assignable<variant<int&>>::value);
    static_assert(std::is_trivially_copy_assignable<variant<int const&>>::value);
    static_assert(std::is_trivially_copy_assignable<variant<int&, float&>>::value);
    static_assert(std::is_trivially_copy_assignable<variant<int&, float>>::value);

    struct trivially_copyable {
        trivially_copyable(const trivially_copyable&) = default;
        auto operator=(const trivially_copyable&) -> trivially_copyable& = default;
    };
    static_assert(std::is_trivially_copy_assignable<variant<int, trivially_copyable>>::value);

    struct non_trivially_copy_constructible {
        non_trivially_copy_constructible(const non_trivially_copy_constructible&) { }
        non_trivially_copy_constructible(non_trivially_copy_constructible&&) = default;
        auto operator=(const non_trivially_copy_constructible&)
            -> non_trivially_copy_constructible& = default;
        auto operator=(non_trivially_copy_constructible&&)
            -> non_trivially_copy_constructible& = default;
        ~non_trivially_copy_constructible() = default;
    };
    static_assert(
        !std::is_trivially_copy_assignable<variant<int, non_trivially_copy_constructible>>::value
    );
    static_assert(std::is_copy_assignable<variant<int, non_trivially_copy_constructible>>::value);
    static_assert(
        std::is_trivially_copy_assignable<variant<int, non_trivially_copy_constructible&>>::value
    );
    static_assert(  //
        !std::is_trivially_copy_assignable<variant<  //
            int,
            non_trivially_copy_constructible,
            non_trivially_copy_constructible&>>::value
    );

    struct non_trivially_copy_assignable {
        non_trivially_copy_assignable(const non_trivially_copy_assignable&) = default;
        non_trivially_copy_assignable(non_trivially_copy_assignable&&) = default;
        auto operator=(const non_trivially_copy_assignable&) -> non_trivially_copy_assignable& {
            return *this;
        }
        auto operator=(non_trivially_copy_assignable&&)
            -> non_trivially_copy_assignable& = default;
        ~non_trivially_copy_assignable() = default;
    };
    static_assert(
        !std::is_trivially_copy_assignable<variant<int, non_trivially_copy_assignable>>::value
    );
    static_assert(std::is_copy_assignable<variant<int, non_trivially_copy_assignable>>::value);
    static_assert(
        std::is_trivially_copy_assignable<variant<int, non_trivially_copy_assignable&>>::value
    );
    static_assert(
        !std::is_trivially_copy_assignable<
            variant<int, non_trivially_copy_assignable, non_trivially_copy_assignable&>>::value
    );

    struct non_trivially_destructible {
        non_trivially_destructible(const non_trivially_destructible&) = default;
        non_trivially_destructible(non_trivially_destructible&&) = default;
        auto operator=(const non_trivially_destructible&) -> non_trivially_destructible& = default;
        auto operator=(non_trivially_destructible&&) -> non_trivially_destructible& = default;
        ~non_trivially_destructible() { }
    };
    static_assert(
        !std::is_trivially_copy_assignable<variant<int, non_trivially_destructible>>::value
    );
    static_assert(std::is_copy_assignable<variant<int, non_trivially_destructible>>::value);
    static_assert(
        std::is_trivially_copy_assignable<variant<int, non_trivially_destructible&>>::value
    );
    static_assert(!std::is_trivially_copy_assignable<
                  variant<int, non_trivially_destructible, non_trivially_destructible&>>::value);

    // clang-format off
    struct trivially_copy_assignable_non_trivially_moveable {
        trivially_copy_assignable_non_trivially_moveable(
            const trivially_copy_assignable_non_trivially_moveable&
        ) = default;
        trivially_copy_assignable_non_trivially_moveable(
            trivially_copy_assignable_non_trivially_moveable&&
        ) { }
        auto operator=(
            const trivially_copy_assignable_non_trivially_moveable&
        ) -> trivially_copy_assignable_non_trivially_moveable& = default;
        auto operator=(
            trivially_copy_assignable_non_trivially_moveable&&
        ) -> trivially_copy_assignable_non_trivially_moveable& {
            return *this;
        }
    };
    // clang-format on
    static_assert(std::is_trivially_copy_assignable<
                  variant<int, trivially_copy_assignable_non_trivially_moveable>>::value);
    static_assert(std::is_trivially_copy_assignable<
                  variant<int, trivially_copy_assignable_non_trivially_moveable&>>::value);
}

TEST(VariantTestCopyAssignment, Noexcept) {
    static_assert(std::is_nothrow_copy_assignable<variant<int>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int, double>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<monostate>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int, monostate>>::value);

    static_assert(std::is_nothrow_copy_assignable<variant<int&>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int const&>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int&, float&>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int&, double>>::value);

    struct nothrow_copyable {
        nothrow_copyable(const nothrow_copyable&) noexcept = default;
        nothrow_copyable(nothrow_copyable&&) = default;
        auto operator=(const nothrow_copyable&) noexcept -> nothrow_copyable& = default;
        auto operator=(nothrow_copyable&&) -> nothrow_copyable& = default;
    };
    static_assert(std::is_nothrow_copy_assignable<variant<int, nothrow_copyable>>::value);

    struct throw_copy_constructible {
        throw_copy_constructible(const throw_copy_constructible&);
        auto operator=(const throw_copy_constructible&) noexcept
            -> throw_copy_constructible& = default;
    };
    static_assert(!std::is_nothrow_copy_assignable<variant<int, throw_copy_constructible>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int, throw_copy_constructible&>>::value);
    static_assert(!std::is_nothrow_copy_assignable<
                  variant<int, throw_copy_constructible, throw_copy_constructible&>>::value);

    struct throw_copy_assignable {
        throw_copy_assignable(const throw_copy_assignable&) noexcept = default;
        auto operator=(const throw_copy_assignable&) -> throw_copy_assignable&;
    };
    static_assert(!std::is_nothrow_copy_assignable<variant<int, throw_copy_assignable>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int, throw_copy_assignable&>>::value);
    static_assert(!std::is_nothrow_copy_assignable<
                  variant<int, throw_copy_assignable, throw_copy_assignable&>>::value);

    struct throw_copyable {
        throw_copyable(const throw_copyable&);
        auto operator=(const throw_copyable&) -> throw_copyable&;
    };
    static_assert(!std::is_nothrow_copy_assignable<variant<int, throw_copyable>>::value);
    static_assert(std::is_nothrow_copy_assignable<variant<int, throw_copyable&>>::value);
    static_assert(
        !std::is_nothrow_copy_assignable<variant<int, throw_copyable, throw_copyable&>>::value
    );
}

TEST(VariantTestCopyAssignment, BasicBehavior) {
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
        v& ref = (x1 = x2);
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

        v& ref = (x1 = x2);
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_TRUE(holds_alternative<counter>(x1));
        EXPECT_TRUE(holds_alternative<counter>(x2));
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_construct_count, 1);
        counter::reset();
    }
    {
        using v = variant<int>;
        v x1(2), x2(3);
        EXPECT_EQ(get<0>(x1), 2);
        EXPECT_EQ(get<0>(x2), 3);
        x1 = x2;
        EXPECT_EQ(get<0>(x1), 3);
        EXPECT_EQ(get<0>(x2), 3);
    }
    {
        using v = variant<counter>;
        v x1, x2;
        EXPECT_EQ(counter::alive_count, 2);
        EXPECT_EQ(counter::copy_assign_count, 0);
        x1 = x2;
        EXPECT_EQ(counter::copy_assign_count, 1);
        counter::reset();
    }
    {
        using v = variant<int, long>;
        v x1(std::in_place_index<1>, 3);
        v x2(std::in_place_index<1>, 4);
        EXPECT_EQ(get<1>(x1), 3);
        EXPECT_EQ(get<1>(x2), 4);

        v& ref = (x1 = x2);
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(get<1>(x1), "bcd");
        EXPECT_EQ(get<1>(x2), "bcd");
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(counter::copy_assign_count, 1);
        EXPECT_EQ(counter::move_assign_count, 0);
        counter::reset();
    }
    {
        using v = variant<int, valueless_t>;
        static_assert(!detail::variant_no_valueless_state<v>::value);
        v x1(std::in_place_index<1>);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());

        EXPECT_THROW((x1 = x2), int);
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(get<0>(x1), 4);
        EXPECT_EQ(get<0>(x2), 4);
        EXPECT_EQ(counter::alive_count, 0);
        counter::reset();
    }
    {
        struct nothrow_copy_constructible {
            bool copy_ctor = false;
            bool move_ctor = false;

            nothrow_copy_constructible() = default;
            nothrow_copy_constructible(const nothrow_copy_constructible&) noexcept {
                copy_ctor = true;
            }
            nothrow_copy_constructible(nothrow_copy_constructible&&) {
                move_ctor = true;
            }
            auto operator=(const nothrow_copy_constructible&)
                -> nothrow_copy_constructible& = default;
            auto operator=(nothrow_copy_constructible&&) -> nothrow_copy_constructible& = default;
        };

        struct throw_copy_constructible {
            throw_copy_constructible() = default;
            throw_copy_constructible(const throw_copy_constructible&) { }
            auto operator=(const throw_copy_constructible&) -> throw_copy_constructible& = default;
        };

        using v = variant<throw_copy_constructible, nothrow_copy_constructible>;
        static_assert(!detail::variant_no_valueless_state<v>::value);
        v x1(std::in_place_index<0>);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(get<1>(x1).copy_ctor);
        EXPECT_FALSE(get<1>(x1).move_ctor);
    }
    {
        struct may_throw_copy_constructible {
            bool copy_ctor = false;
            bool move_ctor = false;

            may_throw_copy_constructible() = default;
            may_throw_copy_constructible(const may_throw_copy_constructible&) {
                copy_ctor = true;
            }
            may_throw_copy_constructible(may_throw_copy_constructible&&) noexcept {
                move_ctor = true;
            }
            auto operator=(const may_throw_copy_constructible&)
                -> may_throw_copy_constructible& = default;
            auto operator=(may_throw_copy_constructible&&)
                -> may_throw_copy_constructible& = default;
        };

        using v = variant<int, may_throw_copy_constructible>;
        static_assert(detail::variant_no_valueless_state<v>::value);
        v x1(std::in_place_index<0>);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_FALSE(get<1>(x1).copy_ctor);
        EXPECT_TRUE(get<1>(x1).move_ctor);
    }
    {
        struct throw_copy_constructible {
            bool move_ctor = false;

            throw_copy_constructible() = default;
            throw_copy_constructible(const throw_copy_constructible&) {
                throw 1;
            }
            throw_copy_constructible(throw_copy_constructible&&) noexcept {
                move_ctor = true;
            }
            auto operator=(const throw_copy_constructible&) -> throw_copy_constructible& = default;
            auto operator=(throw_copy_constructible&&) -> throw_copy_constructible& = default;
        };

        using v = variant<int, throw_copy_constructible>;
        static_assert(detail::variant_no_valueless_state<v>::value);
        v x1(std::in_place_index<0>, 4);
        v x2(std::in_place_index<1>);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        EXPECT_THROW(x1 = x2, int);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(get<0>(x1), 4);
    }
    {
        using v = variant<int&>;
        int val1 = 3, val2 = 4;
        v x1 = val1;
        EXPECT_EQ(&get<0>(x1), &val1);
        v x2 = val2;
        EXPECT_EQ(&get<0>(x2), &val2);

        x1 = x2;
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

        x1 = x2;
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

        x1 = x2;
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
        x1 = x2;
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
constexpr auto constexpr_copy_assign_impl(
    V1&& lhs,
    V2&& rhs,
    std::size_t expected_index,
    Ty expected_value
) -> bool {
    V2 const _rhs(std::forward<V2>(rhs));
    lhs = _rhs;
    return lhs.index() == expected_index && get<Ty>(lhs) == expected_value;
}

TEST(VariantTestCopyAssignment, Constexpr) {
    using v = variant<long, void*, int>;
    static_assert(std::is_trivially_copyable<v>::value);
    static_assert(std::is_trivially_copy_assignable<v>::value);
    static_assert(constexpr_copy_assign_impl(v(3), v(4), 2, 4));
    static_assert(constexpr_copy_assign_impl(v(3l), v(4l), 0, 4l));
    static_assert(constexpr_copy_assign_impl(v(nullptr), v(4l), 0, 4l));
    static_assert(constexpr_copy_assign_impl(v(3l), v(nullptr), 1, static_cast<void*>(nullptr)));
    static_assert(constexpr_copy_assign_impl(v(3l), v(4), 2, 4));
    static_assert(constexpr_copy_assign_impl(v(3), v(4l), 0, 4l));
}

TEST(VariantTestCopyAssignment, ValuelessStrategy) {
    {
        using v = variant<int, may_throw_copy_constructible_strategy_default>;
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(get<1>(x1).copy_ctor);
        EXPECT_FALSE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int, throw_copy_constructible_strategy_default>;
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

        EXPECT_THROW(x1 = x2, int);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x2.index(), 1);
    }
    {
        using v = variant<int, may_throw_copy_constructible_strategy_fallback>;
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(get<1>(x1).copy_ctor);
        EXPECT_FALSE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int, throw_copy_constructible_strategy_fallback>;
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

        EXPECT_THROW(x1 = x2, int);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(get<0>(x1), 0);
    }
    {
        using v = variant<int, may_throw_copy_constructible_strategy_novalueless>;
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

        v& ref = (x1 = x2);
        EXPECT_EQ(&ref, &x1);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_TRUE(get<1>(x1).copy_ctor);
        EXPECT_FALSE(get<1>(x1).move_ctor);
    }
    {
        using v = variant<int, throw_copy_constructible_strategy_novalueless>;
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

        EXPECT_THROW(x1 = x2, int);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_FALSE(x2.valueless_by_exception());
        EXPECT_EQ(x2.index(), 1);
    }
}
}  // namespace
}  // namespace rust
