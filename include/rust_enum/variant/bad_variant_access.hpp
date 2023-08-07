#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_BAD_VARIANT_ACCESS_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_BAD_VARIANT_ACCESS_HPP

#include <stdexcept>

namespace rust {
class bad_variant_access : public std::exception {
public:
    const char* what() const noexcept override {
        return "bad variant access";
    }
};

[[noreturn]] inline void throw_bad_variant_access() {
    throw bad_variant_access {};
}
}  // namespace rust

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_BAD_VARIANT_ACCESS_HPP
