#ifndef RUST_ENUM_VISIT_TEST_HELPER_HPP
#define RUST_ENUM_VISIT_TEST_HELPER_HPP

#include <gtest/gtest.h>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace rust {
enum call_type : unsigned {
  CT_NONE,
  CT_NON_CONST = 1,
  CT_CONST = 2,
  CT_LVALUE = 4,
  CT_RVALUE = 8,
};

constexpr call_type operator|(call_type lhs, call_type rhs) {
  return static_cast<call_type>(static_cast<unsigned>(lhs) |
                                static_cast<unsigned>(rhs));
}

struct forwarding_call_object {
  using self = forwarding_call_object;

  template <class... Args>
  self& operator()(Args&&...) & {
    set_call<Args&&...>(CT_NON_CONST | CT_LVALUE);
    return *this;
  }

  template <class... Args>
  const self& operator()(Args&&...) const& {
    set_call<Args&&...>(CT_CONST | CT_LVALUE);
    return *this;
  }

  template <class... Args>
  self&& operator()(Args&&...) && {
    set_call<Args&&...>(CT_NON_CONST | CT_RVALUE);
    return std::move(*this);
  }

  template <class... Args>
  const self&& operator()(Args&&...) const&& {
    set_call<Args&&...>(CT_CONST | CT_RVALUE);
    return std::move(*this);
  }

  template <class... Args>
  static void set_call(call_type type) {
    ASSERT_EQ(last_call_type, CT_NONE);
    ASSERT_TRUE(last_call_args.empty());
    last_call_type = type;
    last_call_args = {std::type_index(typeid(Args))...};
  }

  template <class... Args>
  static bool check_call(call_type type) {
    std::type_index expected[] = {std::type_index(typeid(int)),
                                  std::type_index(typeid(Args))...};
    bool const result =
        last_call_type == type &&
        std::equal(last_call_args.begin(), last_call_args.end(),
                   std::begin(expected) + 1, std::end(expected));

    last_call_type = CT_NONE;
    last_call_args.clear();

    return result;
  }

  constexpr operator int() const { return 0; }

  static inline call_type last_call_type = CT_NONE;
  static inline std::vector<std::type_index> last_call_args;
};

struct return_first {
  template <class... Args>
  constexpr int operator()(int val, Args&&...) const {
    return val;
  }
};

struct return_first_reference {
  // We return the pointer rather than reference, since MSVC will perform
  // lvalue-to-rvalue conversion when we discard the returned reference. Then it
  // will try to read the value of the referenced object, which will be constant
  // expression and cannot be used with constexpr.
  //
  // According to
  // https://en.cppreference.com/w/cpp/language/expressions#Discarded-value_expressions
  //
  // - The lvalue-to-rvalue conversion is applied if and only if the expression
  // is a volatile-qualified glvalue and ...
  //
  // which show that the behavior of MSVC may be incorrect. We can also find
  // that GCC and Clang both have the correct behavior.
  template <class... Args>
  constexpr int* operator()(int& ref, Args&&...) const {
    return &ref;
  }
};

struct return_arity {
  template <class... Args>
  constexpr unsigned operator()(Args&&...) const {
    return static_cast<unsigned>(sizeof...(Args));
  }
};

template <class Fn>
struct mobile_visitor {
  mobile_visitor() = default;

  mobile_visitor(const mobile_visitor&) { ADD_FAILURE(); }
  mobile_visitor(mobile_visitor&&) { ADD_FAILURE(); }
  mobile_visitor& operator=(const mobile_visitor&) {
    ADD_FAILURE();
    return *this;
  }
  mobile_visitor& operator=(mobile_visitor&&) {
    ADD_FAILURE();
    return *this;
  }

  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    return Fn {}(std::forward<Args>(args)...);
  }
};

template <class Fn>
struct immobile_visitor : mobile_visitor<Fn> {
  immobile_visitor() = default;
  immobile_visitor(const immobile_visitor&) = delete;
  immobile_visitor& operator=(const immobile_visitor&) = delete;
};
}  // namespace rust

#endif  // RUST_ENUM_VISIT_TEST_HELPER_HPP
