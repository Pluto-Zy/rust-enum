#include <common.hpp>
#include <type_traits>

namespace rust {
namespace {
TEST(VariantTestSwap, ValuelessByException) {
    using v = variant<int, valueless_t>;
    {
        v x1, x2;
        make_valueless(x1);
        make_valueless(x2);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        {
            x1.swap(x2);
            EXPECT_TRUE(x1.valueless_by_exception());
            EXPECT_TRUE(x2.valueless_by_exception());
        }
        {
            swap(x1, x2);
            EXPECT_TRUE(x1.valueless_by_exception());
            EXPECT_TRUE(x2.valueless_by_exception());
        }
    }
    {
        v x1(3), x2;
        make_valueless(x2);
        EXPECT_FALSE(x1.valueless_by_exception());
        EXPECT_TRUE(x2.valueless_by_exception());
        {
            x1.swap(x2);
            EXPECT_TRUE(x1.valueless_by_exception());
            EXPECT_FALSE(x2.valueless_by_exception());
            EXPECT_EQ(get<0>(x2), 3);

            x2.swap(x1);
            EXPECT_FALSE(x1.valueless_by_exception());
            EXPECT_TRUE(x2.valueless_by_exception());
            EXPECT_EQ(get<0>(x1), 3);
        }
        {
            swap(x1, x2);
            EXPECT_TRUE(x1.valueless_by_exception());
            EXPECT_FALSE(x2.valueless_by_exception());
            EXPECT_EQ(get<0>(x2), 3);

            swap(x1, x2);
            EXPECT_FALSE(x1.valueless_by_exception());
            EXPECT_TRUE(x2.valueless_by_exception());
            EXPECT_EQ(get<0>(x1), 3);
        }
    }
}

template <bool Throws>
void do_throw() { }

template <>
void do_throw<true>() {
    throw 3;
}

template <
    bool NTCopy,
    bool NTMove,
    bool NTCopyAssign,
    bool NTMoveAssign,
    bool NTSwap,
    bool EnableSwap = true>
struct nothrow_type_impl {
    static inline unsigned move_called = 0;
    static inline unsigned move_assign_called = 0;
    static inline unsigned swap_called = 0;

    int value;

    static void reset() {
        move_called = move_assign_called = swap_called = 0;
    }

    nothrow_type_impl() = default;
    explicit nothrow_type_impl(int v) : value(v) { }

    nothrow_type_impl(const nothrow_type_impl& other) noexcept(NTCopy) : value(other.value) {
        ADD_FAILURE();
    }

    nothrow_type_impl(nothrow_type_impl&& other) noexcept(NTMove) : value(other.value) {
        ++move_called;
        do_throw<!NTMove>();
        other.value = -1;
    }

    auto operator=(const nothrow_type_impl&) noexcept(NTCopyAssign) -> nothrow_type_impl& {
        ADD_FAILURE();
        return *this;
    }

    auto operator=(nothrow_type_impl&& other) noexcept(NTMoveAssign) -> nothrow_type_impl& {
        ++move_assign_called;
        do_throw<!NTMoveAssign>();
        value = other.value;
        other.value = -1;
        return *this;
    }
};

template <bool NTCopy, bool NTMove, bool NTCopyAssign, bool NTMoveAssign, bool NTSwap>
void swap(
    nothrow_type_impl<NTCopy, NTMove, NTCopyAssign, NTMoveAssign, NTSwap, true>& lhs,
    nothrow_type_impl<NTCopy, NTMove, NTCopyAssign, NTMoveAssign, NTSwap, true>& rhs
) noexcept(NTSwap) {
    ++lhs.swap_called;
    do_throw<!NTSwap>();

    int temp = lhs.value;
    lhs.value = rhs.value;
    rhs.value = temp;
}

// throwing copy, nothrow move ctor/assign, no swap provided
using nothrow_moveable = nothrow_type_impl<false, true, false, true, false, false>;
// throwing copy and move assign, nothrow move ctor, no swap provided
using nothrow_move_ctor = nothrow_type_impl<false, true, false, false, false, false>;
// nothrow move ctor, throwing move assignment, swap provided
using nothrow_move_ctor_with_throwing_swap =
    nothrow_type_impl<false, true, false, false, false, true>;
// throwing move ctor, nothrow move assignment, no swap provided
using throwing_move_ctor = nothrow_type_impl<false, false, false, true, false, false>;
// throwing special members, nothrow swap
using throwing_type_with_nothrow_swap = nothrow_type_impl<false, false, false, false, true, true>;
using nothrow_type_with_throwing_swap = nothrow_type_impl<true, true, true, true, false, true>;
// throwing move assign with nothrow move and nothrow swap
using throwing_move_assign_nothrow_move_ctor_with_swap =
    nothrow_type_impl<false, true, false, false, true, true>;
// throwing move assign with nothrow move but no swap.
using throwing_move_assign_nothrow_move_ctor =
    nothrow_type_impl<false, true, false, false, false, false>;

TEST(VariantTestSwap, SameAlternative) {
    {
        using impl = throwing_type_with_nothrow_swap;
        using v = variant<impl, int>;
        impl::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<0>, 100);

        x1.swap(x2);
        EXPECT_EQ(impl::swap_called, 1);
        EXPECT_EQ(get<0>(x1).value, 100);
        EXPECT_EQ(get<0>(x2).value, 42);

        swap(x1, x2);
        EXPECT_EQ(impl::swap_called, 2);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_EQ(get<0>(x2).value, 100);
    }
    {
        using impl = nothrow_moveable;
        using v = variant<impl, int>;
        impl::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<0>, 100);
        x1.swap(x2);
        EXPECT_EQ(impl::swap_called, 0);
        EXPECT_EQ(impl::move_called, 1);
        EXPECT_EQ(impl::move_assign_called, 2);
        EXPECT_EQ(get<0>(x1).value, 100);
        EXPECT_EQ(get<0>(x2).value, 42);

        impl::reset();
        swap(x1, x2);
        EXPECT_EQ(impl::swap_called, 0);
        EXPECT_EQ(impl::move_called, 1);
        EXPECT_EQ(impl::move_assign_called, 2);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_EQ(get<0>(x2).value, 100);
    }
    {
        using impl = nothrow_type_with_throwing_swap;
        using v = variant<impl, int>;
        impl::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<0>, 100);

        EXPECT_THROW(x1.swap(x2), int);
        EXPECT_EQ(impl::swap_called, 1);
        EXPECT_EQ(impl::move_called, 0);
        EXPECT_EQ(impl::move_assign_called, 0);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_EQ(get<0>(x2).value, 100);
    }
    {
        using impl = throwing_move_ctor;
        using v = variant<impl, int>;
        impl::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<0>, 100);

        EXPECT_THROW(x1.swap(x2), int);
        EXPECT_EQ(impl::move_called, 1);
        EXPECT_EQ(impl::move_assign_called, 0);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_EQ(get<0>(x2).value, 100);
    }
    {
        using impl = throwing_move_assign_nothrow_move_ctor;
        using v = variant<impl, int>;
        impl::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<0>, 100);

        EXPECT_THROW(x1.swap(x2), int);
        EXPECT_EQ(impl::move_called, 1);
        EXPECT_EQ(impl::move_assign_called, 1);
        EXPECT_EQ(get<0>(x1).value, -1);  // x1 was moved from
        EXPECT_EQ(get<0>(x2).value, 100);
    }
    {
        using v = variant<int&, int>;
        int data1 = 3, data2 = 4;
        v x1(std::in_place_index<0>, data1);
        v x2(std::in_place_index<0>, data2);
        EXPECT_EQ(&get<0>(x1), &data1);
        EXPECT_EQ(&get<0>(x2), &data2);

        x1.swap(x2);
        EXPECT_EQ(&get<0>(x1), &data2);
        EXPECT_EQ(&get<0>(x2), &data1);
        EXPECT_EQ(data1, 3);
        EXPECT_EQ(data2, 4);

        swap(x1, x2);
        EXPECT_EQ(&get<0>(x1), &data1);
        EXPECT_EQ(&get<0>(x2), &data2);
        EXPECT_EQ(data1, 3);
        EXPECT_EQ(data2, 4);
    }
    {
        using v = variant<int&, int&>;
        int data1 = 3, data2 = 4;
        v x1(std::in_place_index<1>, data1);
        v x2(std::in_place_index<1>, data2);
        EXPECT_EQ(&get<1>(x1), &data1);
        EXPECT_EQ(&get<1>(x2), &data2);

        x1.swap(x2);
        EXPECT_EQ(&get<1>(x1), &data2);
        EXPECT_EQ(&get<1>(x2), &data1);
        EXPECT_EQ(data1, 3);
        EXPECT_EQ(data2, 4);

        swap(x1, x2);
        EXPECT_EQ(&get<1>(x1), &data1);
        EXPECT_EQ(&get<1>(x2), &data2);
        EXPECT_EQ(data1, 3);
        EXPECT_EQ(data2, 4);
    }
}

struct non_throwing_non_noexcept_type {
    static inline unsigned move_called = 0;
    static void reset() {
        move_called = 0;
    }

    int value;

    non_throwing_non_noexcept_type() = default;
    non_throwing_non_noexcept_type(int v) : value(v) { }

    non_throwing_non_noexcept_type(non_throwing_non_noexcept_type&& other) noexcept(false) :
        value(other.value) {
        ++move_called;
        other.value = -1;
    }

    auto operator=(non_throwing_non_noexcept_type&&) noexcept(false)
        -> non_throwing_non_noexcept_type& {
        ADD_FAILURE();
        return *this;
    }
};

struct throws_on_second_move {
    int value;
    unsigned move_count;

    throws_on_second_move(int v) : value(v), move_count(0) { }
    throws_on_second_move(throws_on_second_move&& other) noexcept(false) :
        value(other.value), move_count(other.move_count + 1) {
        if (move_count == 2)
            do_throw<true>();
        other.value = -1;
    }
    auto operator=(throws_on_second_move&&) -> throws_on_second_move& {
        ADD_FAILURE();
        return *this;
    }
};

TEST(VariantTestSwap, DifferentAlternatives) {
    {
        using impl = nothrow_move_ctor_with_throwing_swap;
        using v = variant<impl, int>;
        impl::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<1>, 100);

        x1.swap(x2);
        EXPECT_EQ(impl::swap_called, 0);
        EXPECT_EQ(impl::move_called, 1);
        EXPECT_EQ(impl::move_assign_called, 0);
        EXPECT_EQ(get<1>(x1), 100);
        EXPECT_EQ(get<0>(x2).value, 42);

        impl::reset();
        swap(x1, x2);
        EXPECT_EQ(impl::swap_called, 0);
        EXPECT_EQ(impl::move_called, 2);
        EXPECT_EQ(impl::move_assign_called, 0);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_EQ(get<1>(x2), 100);
    }
    {
        using impl1 = throwing_type_with_nothrow_swap;
        using impl2 = non_throwing_non_noexcept_type;
        using v = variant<impl1, impl2>;
        impl1::reset();
        impl2::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<1>, 100);

        EXPECT_THROW(x1.swap(x2), int);
        EXPECT_EQ(impl1::swap_called, 0);
        EXPECT_EQ(impl1::move_called, 1);
        EXPECT_EQ(impl1::move_assign_called, 0);
        EXPECT_EQ(impl2::move_called, 1);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_TRUE(x2.valueless_by_exception());
    }
    {
        using impl1 = non_throwing_non_noexcept_type;
        using impl2 = throwing_type_with_nothrow_swap;
        using v = variant<impl1, impl2>;
        impl1::reset();
        impl2::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<1>, 100);

        EXPECT_THROW(x1.swap(x2), int);
        EXPECT_EQ(impl1::move_called, 0);
        EXPECT_EQ(impl2::swap_called, 0);
        EXPECT_EQ(impl2::move_called, 1);
        EXPECT_EQ(impl2::move_assign_called, 0);
        EXPECT_EQ(get<0>(x1).value, 42);
        EXPECT_EQ(get<1>(x2).value, 100);
    }
    {
        using impl1 = throws_on_second_move;
        using impl2 = non_throwing_non_noexcept_type;
        using v = variant<impl1, impl2>;
        impl2::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<1>, 100);

        x1.swap(x2);
        EXPECT_EQ(impl2::move_called, 2);
        EXPECT_EQ(get<1>(x1).value, 100);
        EXPECT_EQ(get<0>(x2).value, 42);
        EXPECT_EQ(get<0>(x2).move_count, 1);
    }
    {
        using impl1 = non_throwing_non_noexcept_type;
        using impl2 = throws_on_second_move;
        using v = variant<impl1, impl2>;
        impl1::reset();
        v x1(std::in_place_index<0>, 42);
        v x2(std::in_place_index<1>, 100);

        EXPECT_THROW(x1.swap(x2), int);
        EXPECT_EQ(impl1::move_called, 1);
        EXPECT_TRUE(x1.valueless_by_exception());
        EXPECT_EQ(get<0>(x2).value, 42);
    }
    {
        using v = variant<int&, int&>;
        int data1 = 3, data2 = 4;
        v x1(std::in_place_index<0>, data1);
        v x2(std::in_place_index<1>, data2);
        EXPECT_EQ(&get<0>(x1), &data1);
        EXPECT_EQ(&get<1>(x2), &data2);

        x1.swap(x2);
        EXPECT_EQ(&get<1>(x1), &data2);
        EXPECT_EQ(&get<0>(x2), &data1);
        EXPECT_EQ(data1, 3);
        EXPECT_EQ(data2, 4);

        swap(x1, x2);
        EXPECT_EQ(&get<0>(x1), &data1);
        EXPECT_EQ(&get<1>(x2), &data2);
        EXPECT_EQ(data1, 3);
        EXPECT_EQ(data2, 4);
    }
    {
        using v = variant<long&, int const&>;
        long data1 = 3;
        int data2 = 4;
        v x1(data1);
        v x2(data2);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);

        x1.swap(x2);
        EXPECT_EQ(x1.index(), 1);
        EXPECT_EQ(x2.index(), 0);
        EXPECT_EQ(&get<1>(x1), &data2);
        EXPECT_EQ(&get<0>(x2), &data1);
        EXPECT_EQ(data1, 3l);
        EXPECT_EQ(data2, 4);

        swap(x1, x2);
        EXPECT_EQ(x1.index(), 0);
        EXPECT_EQ(x2.index(), 1);
        EXPECT_EQ(&get<0>(x1), &data1);
        EXPECT_EQ(&get<1>(x2), &data2);
    }
}

struct not_swappable { };
void swap(not_swappable&, not_swappable&) = delete;

TEST(VariantTestSwap, Noexcept) {
    {
        using v = variant<int, nothrow_moveable>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);

        v x1, x2;
        x1.swap(x2);
        swap(x1, x2);
    }
    {
        using v = variant<int, nothrow_move_ctor>;
        static_assert(std::is_swappable<v>::value);
        static_assert(!std::is_nothrow_swappable<v>::value);

        v x1, x2;
        x1.swap(x2);
        swap(x1, x2);
    }
    {
        using v = variant<int, throwing_type_with_nothrow_swap>;
        static_assert(std::is_swappable<v>::value);
        static_assert(!std::is_nothrow_swappable<v>::value);

        v x1, x2;
        x1.swap(x2);
        swap(x1, x2);
    }
    {
        using v = variant<int, throwing_move_assign_nothrow_move_ctor>;
        static_assert(std::is_swappable<v>::value);
        static_assert(!std::is_nothrow_swappable<v>::value);

        v x1, x2;
        x1.swap(x2);
        swap(x1, x2);
    }
    {
        using v = variant<int, throwing_move_assign_nothrow_move_ctor_with_swap>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);

        v x1, x2;
        x1.swap(x2);
        swap(x1, x2);
    }
    {
        using v = variant<int, not_swappable>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);

        v x1, x2;
        using std::swap;
        swap(x1, x2);
    }
    {
        using v = variant<int&>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);

        int data = 3;
        v x1(data), x2(data);
        x1.swap(x2);
        using std::swap;
        swap(x1, x2);
    }
    {
        using v = variant<int&, const int&>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);

        int data = 3;
        v x1(data), x2(data);
        x1.swap(x2);
        using std::swap;
        swap(x1, x2);
    }
    {
        using v = variant<int&, long&>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);

        long data = 3;
        v x1(data), x2(data);
        x1.swap(x2);
        using std::swap;
        swap(x1, x2);
    }
    {
        using v = variant<const int, int&>;
        static_assert(!std::is_swappable<v>::value);
    }
    {
        using v = variant<
            nothrow_move_ctor&,
            throwing_type_with_nothrow_swap&,
            throwing_move_assign_nothrow_move_ctor&>;
        static_assert(std::is_swappable<v>::value);
        static_assert(std::is_nothrow_swappable<v>::value);
    }
}
}  // namespace
}  // namespace rust
