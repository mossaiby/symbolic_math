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

namespace symbolic_math
{

struct Binding
{
  const void *tag;
  double value;
};

constexpr double get_binding_value(const void *tag, std::initializer_list<Binding> bindings)
{
  for (const auto &b : bindings)
  {
    if (b.tag == tag)
    {
      return b.value;
    }
  }
  throw std::logic_error("symbolic_math: error: undefined symbol in expression");
}

template <typename>
struct Symbol_Id
{
  static constexpr auto singleton = []{};
  static constexpr const void *tag = std::addressof(singleton);
};

template <typename Id = Symbol_Id<decltype([]{})>>
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

  constexpr double evaluate(std::initializer_list<Binding> bindings) const
  {
    return get_binding_value(tag, bindings);
  }
};

struct Constant
{
  double value;
  constexpr Constant(double v) : value(v) {}
  constexpr double evaluate(std::initializer_list<Binding>) const { return value; }
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
};

template <typename LHS, typename RHS>
constexpr auto operator*(const LHS &lhs, const RHS &rhs)
{
  return Multiply<LHS, RHS>{lhs, rhs};
}

constexpr Constant make_constant(double d) { return Constant(d); }

template <typename T>
constexpr auto operator*(double d, const T &expression)
{
  return Multiply<Constant, T>{make_constant(d), expression};
}

template <typename T>
constexpr auto operator*(const T &expr, double d)
{
  return Multiply<T, Constant>{expr, make_constant(d)};
}

template <typename T>
constexpr auto operator+(double d, const T &expression)
{
  return Add<Constant, T>{make_constant(d), expression};
}

template <typename T>
constexpr auto operator+(const T &expression, double d)
{
  return Add<T, Constant>{expression, make_constant(d)};
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
};
template <typename E>
Expression(const E &) -> Expression<E>;

template <typename>
using symbol = Symbol<>;

using constant = Constant;

template <typename T>
using expression = Expression<T>;

}