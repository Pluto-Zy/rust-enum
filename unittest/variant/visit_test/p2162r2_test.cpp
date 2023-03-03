#include <common.hpp>
#include <memory>

namespace rust {
namespace {
struct expr;

struct neg {
  std::shared_ptr<expr> e;
};

struct add {
  std::shared_ptr<expr> lhs;
  std::shared_ptr<expr> rhs;
};

struct mul {
  std::shared_ptr<expr> lhs;
  std::shared_ptr<expr> rhs;
};

struct expr : variant<int, neg, add, mul> {
  using variant::variant;
};

int eval(const expr& expr) {
  struct visitor {
    int operator()(int i) const { return i; }
    int operator()(const neg& n) const { return -eval(*n.e); }
    int operator()(const add& a) const { return eval(*a.lhs) + eval(*a.rhs); }
    int operator()(const mul& m) const { return eval(*m.lhs) * eval(*m.rhs); }
  };
  return visit(visitor {}, expr);
}

TEST(VariantTestP2162R2, P2162R2) {
  // example from P2162R2
  struct disconnected {
    int val;
  };

  struct connecting {
    char val;
  };

  struct connected {
    double val;
  };

  struct state : variant<disconnected, connecting, connected> {
    using variant::variant;
  };

  {
    state v1 = disconnected {45};
    const state v2 = connecting {'d'};
    visit([](auto x) { EXPECT_EQ(x.val, 45); }, v1);
    visit([](auto x) { EXPECT_EQ(x.val, 'd'); }, v2);
    visit([](auto x) { EXPECT_EQ(x.val, 5.5); }, state {connected {5.5}});
    visit([](auto x) { EXPECT_EQ(x.val, 45); }, std::move(v1));
    visit([](auto x) { EXPECT_EQ(x.val, 'd'); }, std::move(v2));
  }
  {
    const expr e =
        add {std::make_shared<expr>(1),
             std::make_shared<expr>(mul {
                 std::make_shared<expr>(2),
                 std::make_shared<expr>(neg {std::make_shared<expr>(3)})})};

    EXPECT_EQ(eval(e), 1 + 2 * -3);
  }
}
}  // namespace
}  // namespace rust
