#ifndef RUST_ENUM_COMMON_HPP
#define RUST_ENUM_COMMON_HPP

#include <gtest/gtest.h>
#include <rust_enum/variant.hpp>

namespace rust {
struct valueless_t {
    valueless_t() = default;
    valueless_t(const valueless_t&) {
        throw 1;
    }
    valueless_t(valueless_t&&) {
        throw 2;
    }
    valueless_t& operator=(const valueless_t&) {
        throw 3;
    }
    valueless_t& operator=(valueless_t&&) {
        throw 4;
    }
};

inline bool operator==(const valueless_t&, const valueless_t&) {
    ADD_FAILURE();
    return false;
}

inline bool operator!=(const valueless_t&, const valueless_t&) {
    ADD_FAILURE();
    return false;
}

inline bool operator<(const valueless_t&, const valueless_t&) {
    ADD_FAILURE();
    return false;
}

inline bool operator<=(const valueless_t&, const valueless_t&) {
    ADD_FAILURE();
    return false;
}

inline bool operator>(const valueless_t&, const valueless_t&) {
    ADD_FAILURE();
    return false;
}

inline bool operator>=(const valueless_t&, const valueless_t&) {
    ADD_FAILURE();
    return false;
}

template <class Variant>
void make_valueless(Variant& v) {
    Variant valueless(std::in_place_type<valueless_t>);
    EXPECT_THROW((v = valueless), int);
}

// TODO: This is not thread-safe.
struct counter {
    static inline std::size_t alive_count = 0, copy_construct_count = 0, move_construct_count = 0,
                              copy_assign_count = 0, move_assign_count = 0;

    counter() {
        ++alive_count;
    }
    counter(const counter&) : counter() {
        ++copy_construct_count;
    }
    counter(counter&&) : counter() {
        ++move_construct_count;
    }
    counter& operator=(const counter&) {
        ++copy_assign_count;
        return *this;
    }
    counter& operator=(counter&&) {
        ++move_assign_count;
        return *this;
    }
    ~counter() {
        --alive_count;
    }

    static void reset() {
        copy_construct_count = 0;
        move_construct_count = 0;
        copy_assign_count = 0;
        move_assign_count = 0;
    }
};
}  // namespace rust

#endif  // RUST_ENUM_COMMON_HPP
