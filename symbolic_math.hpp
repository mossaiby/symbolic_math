//
// symbolic_math.hpp
// a simple constexpr symbolic math library for c++23 and later
//
// some ideas from https://www.youtube.com/watch?v=lPfA4SFojao
// created using chatgpt and deepseek, improved by Farshid Mossaiby
//

#pragma once

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <format>

namespace symbolic_math
{

  using Tag = const void *;

  struct Binding
  {
    Tag tag;
    double value;
  };

  struct SymbolicBinding
  {
    Tag tag;
    std::string name;
  };

  constexpr double get_binding_value(Tag tag, std::initializer_list<Binding> symbolic_bindings)
  {
    for (const auto &b : symbolic_bindings)
    {
      if (b.tag == tag)
      {
        return b.value;
      }
    }
    throw std::logic_error("symbolic_math: get_binding_value: error: undefined symbol in expression");
  }

  constexpr std::string get_symbolic_binding_name(Tag tag, std::initializer_list<SymbolicBinding> symbolic_bindings)
  {
    for (const auto &b : symbolic_bindings)
    {
      if (b.tag == tag)
      {
        return b.name;
      }
    }
    return "";
  }

  template <typename>
  struct Symbol_Id
  {
    static constexpr auto singleton = [] {};
    static constexpr Tag tag = std::addressof(singleton);
  };

  template <typename Id = Symbol_Id<decltype([] {})>>
  struct Symbol
  {
    static constexpr const auto tag = Id::tag;

    constexpr Symbol() = default;
    constexpr Symbol(const Symbol &) = default;

    constexpr Symbol &operator=(const Symbol &) = delete;
    constexpr Symbol &operator=(Symbol &&) = delete;
    constexpr Binding operator=(double v) const noexcept
    {
      return Binding{tag, v};
    }
    constexpr SymbolicBinding operator=(std::string n) const noexcept
    {
      return SymbolicBinding{tag, n};
    }
    constexpr double evaluate(std::initializer_list<Binding> bindings) const
    {
      return get_binding_value(tag, bindings);
    }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      return get_symbolic_binding_name(tag, symbolic_bindings);
    }
  };

  template <typename Id = Symbol_Id<decltype([] {})>>
  struct Constant
  {
    static constexpr const auto tag = Id::tag;
    double value;
    constexpr Constant(double v) : value(v) {}
    constexpr SymbolicBinding operator=(std::string n) const noexcept
    {
      return SymbolicBinding{tag, n};
    }
    constexpr double evaluate(std::initializer_list<Binding>) const { return value; }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      std::string name = get_symbolic_binding_name(tag, symbolic_bindings);
      if (name.length() == 0)
      {
        return std::format("{}", value);
      }

      else
      {
        return name;
      }
    }
  };

  template <typename LHS, typename RHS>
  struct Add
  {
    LHS lhs;
    RHS rhs;
    constexpr double evaluate(std::initializer_list<Binding> bindings) const
    {
      return lhs.evaluate(bindings) + rhs.evaluate(bindings);
    }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      return "(" + lhs.symbolic_evaluate(symbolic_bindings) + " + " + rhs.symbolic_evaluate(symbolic_bindings) + ")";
    }
  };

  template <typename LHS, typename RHS>
  constexpr auto operator+(const LHS &lhs, const RHS &rhs)
  {
    return Add<LHS, RHS>{lhs, rhs};
  }

  template <typename LHS, typename RHS>
  struct Subtract
  {
    LHS lhs;
    RHS rhs;
    constexpr double evaluate(std::initializer_list<Binding> bindings) const
    {
      return lhs.evaluate(bindings) - rhs.evaluate(bindings);
    }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      return "(" + lhs.symbolic_evaluate(symbolic_bindings) + " - " + rhs.symbolic_evaluate(symbolic_bindings) + ")";
    }
  };

  template <typename LHS, typename RHS>
  constexpr auto operator-(const LHS &lhs, const RHS &rhs)
  {
    return Subtract<LHS, RHS>{lhs, rhs};
  }

  template <typename LHS, typename RHS>
  struct Multiply
  {
    LHS lhs;
    RHS rhs;
    constexpr double evaluate(std::initializer_list<Binding> bindings) const
    {
      return lhs.evaluate(bindings) * rhs.evaluate(bindings);
    }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      return "(" + lhs.symbolic_evaluate(symbolic_bindings) + " * " + rhs.symbolic_evaluate(symbolic_bindings) + ")";
    }
  };

  template <typename LHS, typename RHS>
  constexpr auto operator*(const LHS &lhs, const RHS &rhs)
  {
    return Multiply<LHS, RHS>{lhs, rhs};
  }

  template <typename LHS, typename RHS>
  struct Divide
  {
    LHS lhs;
    RHS rhs;
    constexpr double evaluate(std::initializer_list<Binding> bindings) const
    {
      return lhs.evaluate(bindings) / rhs.evaluate(bindings);
    }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      return "(" + lhs.symbolic_evaluate(symbolic_bindings) + " / " + rhs.symbolic_evaluate(symbolic_bindings) + ")";
    }
  };

  template <typename LHS, typename RHS>
  constexpr auto operator/(const LHS &lhs, const RHS &rhs)
  {
    return Divide<LHS, RHS>{lhs, rhs};
  }

  constexpr auto make_constant(double d) { return Constant(d); }

  template <typename T>
  constexpr auto operator*(double d, const T &expression)
  {
    auto c = make_constant(d);
    return Multiply<decltype(c), T>{c, expression};
  }

  template <typename T>
  constexpr auto operator*(const T &expr, double d)
  {
    auto c = make_constant(d);
    return Multiply<T, decltype(c)>{expr, c};
  }

  template <typename T>
  constexpr auto operator+(double d, const T &expression)
  {
    auto c = make_constant(d);
    return Add<decltype(c), T>{c, expression};
  }

  template <typename T>
  constexpr auto operator+(const T &expression, double d)
  {
    auto c = make_constant(d);
    return Add<T, decltype(c)>{expression, c};
  }

  template <typename E>
  struct Expression
  {
    E e;
    constexpr Expression(const E &e) : e(e) {}
    constexpr double evaluate(std::initializer_list<Binding> bindings) const
    {
      return e.evaluate(bindings);
    }
    constexpr std::string symbolic_evaluate(std::initializer_list<SymbolicBinding> symbolic_bindings) const
    {
      return e.symbolic_evaluate(symbolic_bindings);
    }
  };
  template <typename E>
  Expression(const E &) -> Expression<E>;

}