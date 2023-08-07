#ifndef RUST_ENUM_VALUELESS_STRATEGY_TEST_HELPER_HPP
#define RUST_ENUM_VALUELESS_STRATEGY_TEST_HELPER_HPP

#include "rust_enum/variant.hpp"
#include <initializer_list>

namespace rust {
struct may_throw_copy_constructible {
    bool copy_ctor = false;
    bool move_ctor = false;

    may_throw_copy_constructible() = default;
    may_throw_copy_constructible(const may_throw_copy_constructible&) {
        copy_ctor = true;
    }
    may_throw_copy_constructible(may_throw_copy_constructible&&) {
        move_ctor = true;
    }
    auto operator=(const may_throw_copy_constructible&) -> may_throw_copy_constructible& = default;
    auto operator=(may_throw_copy_constructible&&) -> may_throw_copy_constructible& = default;
};

struct throw_copy_constructible {
    bool move_ctor = false;

    throw_copy_constructible() = default;
    throw_copy_constructible(const throw_copy_constructible&) {
        throw 1;
    }
    throw_copy_constructible(throw_copy_constructible&&) {
        move_ctor = true;
    }
    auto operator=(const throw_copy_constructible&) -> throw_copy_constructible& = default;
    auto operator=(throw_copy_constructible&&) -> throw_copy_constructible& = default;
};

struct may_throw_copy_constructible_strategy_default : may_throw_copy_constructible { };
struct throw_copy_constructible_strategy_default : throw_copy_constructible { };

struct may_throw_copy_constructible_strategy_fallback : may_throw_copy_constructible { };
struct throw_copy_constructible_strategy_fallback : throw_copy_constructible { };

template <>
struct variant_valueless_strategy<variant<int, may_throw_copy_constructible_strategy_fallback>> {
    static constexpr variant_valueless_strategy_t value = variant_valueless_strategy_t::FALLBACK;
};

template <>
struct variant_valueless_strategy<variant<int, throw_copy_constructible_strategy_fallback>> {
    static constexpr variant_valueless_strategy_t value = variant_valueless_strategy_t::FALLBACK;
};

struct may_throw_copy_constructible_strategy_novalueless : may_throw_copy_constructible { };
struct throw_copy_constructible_strategy_novalueless : throw_copy_constructible { };

template <>
struct variant_valueless_strategy<
    variant<int, may_throw_copy_constructible_strategy_novalueless>> {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

template <>
struct variant_valueless_strategy<variant<int, throw_copy_constructible_strategy_novalueless>> {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

struct may_throw_move_constructible : may_throw_copy_constructible { };

struct throw_move_constructible {
    bool copy_ctor = false;

    throw_move_constructible() = default;
    throw_move_constructible(const throw_move_constructible&) {
        copy_ctor = true;
    }
    throw_move_constructible(throw_move_constructible&&) {
        throw 2;
    }
    auto operator=(const throw_move_constructible&) -> throw_move_constructible& = default;
    auto operator=(throw_move_constructible&&) -> throw_move_constructible& = default;
};

struct may_throw_move_constructible_strategy_default : may_throw_move_constructible { };
struct throw_move_constructible_strategy_default : throw_move_constructible { };

struct may_throw_move_constructible_strategy_fallback : may_throw_move_constructible { };
struct throw_move_constructible_strategy_fallback : throw_move_constructible { };

template <>
struct variant_valueless_strategy<variant<int, may_throw_move_constructible_strategy_fallback>> {
    static constexpr variant_valueless_strategy_t value = variant_valueless_strategy_t::FALLBACK;
};

template <>
struct variant_valueless_strategy<variant<int, throw_move_constructible_strategy_fallback>> {
    static constexpr variant_valueless_strategy_t value = variant_valueless_strategy_t::FALLBACK;
};

struct may_throw_move_constructible_strategy_novalueless : may_throw_move_constructible { };
struct throw_move_constructible_strategy_novalueless : throw_move_constructible { };

template <>
struct variant_valueless_strategy<
    variant<int, may_throw_move_constructible_strategy_novalueless>> {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

template <>
struct variant_valueless_strategy<variant<int, throw_move_constructible_strategy_novalueless>> {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

struct dummy {
    int value;
    dummy() noexcept : value(2) { }
    explicit dummy(int val) : value(val) { }
};

struct may_throw_constructible {
    bool move_ctor = false;

    may_throw_constructible(int) { }
    may_throw_constructible(std::initializer_list<int>) { }
    may_throw_constructible(std::initializer_list<int>, int) { }
    may_throw_constructible(const may_throw_constructible&) = default;
    may_throw_constructible(may_throw_constructible&&) {
        move_ctor = true;
    }
    auto operator=(const may_throw_constructible&) -> may_throw_constructible& = default;
    auto operator=(may_throw_constructible&&) -> may_throw_constructible& = default;
};

struct throw_constructible {
    int value;
    bool move_ctor;

    throw_constructible() : value(2), move_ctor(false) { }
    throw_constructible(int) {
        throw 1;
    }
    throw_constructible(std::initializer_list<int>) {
        throw 2;
    }
    throw_constructible(std::initializer_list<int>, int) {
        throw 3;
    }
    throw_constructible(const throw_constructible&) = default;
    throw_constructible(throw_constructible&&) {
        move_ctor = true;
    }
    auto operator=(const throw_constructible&) -> throw_constructible& = default;
    auto operator=(throw_constructible&&) -> throw_constructible& = default;
};

struct may_throw_constructible_strategy_default : may_throw_constructible {
    using may_throw_constructible::may_throw_constructible;
};

struct throw_constructible_strategy_default : throw_constructible {
    using throw_constructible::throw_constructible;
};

struct throw_constructible_strategy_fallback : throw_constructible {
    using throw_constructible::throw_constructible;
};

struct may_throw_constructible_strategy_fallback : may_throw_constructible {
    using may_throw_constructible::may_throw_constructible;
};

template <>
struct variant_valueless_strategy<variant<dummy, throw_constructible_strategy_fallback>> {
    static constexpr variant_valueless_strategy_t value = variant_valueless_strategy_t::FALLBACK;
};

template <>
struct variant_valueless_strategy<variant<dummy, may_throw_constructible_strategy_fallback>> {
    static constexpr variant_valueless_strategy_t value = variant_valueless_strategy_t::FALLBACK;
};

struct throw_constructible_strategy_novalueless : throw_constructible {
    using throw_constructible::throw_constructible;
};

struct may_throw_constructible_strategy_novalueless : may_throw_constructible {
    using may_throw_constructible::may_throw_constructible;
};

template <>
struct variant_valueless_strategy<variant<dummy, throw_constructible_strategy_novalueless>> {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::DISALLOW_VALUELESS;
};

template <>
struct variant_valueless_strategy<variant<dummy, may_throw_constructible_strategy_novalueless>> {
    static constexpr variant_valueless_strategy_t value =
        variant_valueless_strategy_t::DISALLOW_VALUELESS;
};
}  // namespace rust

#endif  // RUST_ENUM_VALUELESS_STRATEGY_TEST_HELPER_HPP
