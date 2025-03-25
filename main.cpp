//
// main.cpp
// test for symbolic_math.hpp
//
// some ideas from https://www.youtube.com/watch?v=lPfA4SFojao
// created using chatgpt and deepseek, improved by Farshid Mossaiby
//

#include "symbolic_math.hpp"

int main()
{
  constexpr symbolic_math::Constant pi = 3.14159265358979323846;

  // declare symbols
  constexpr symbolic_math::Symbol x;
  constexpr symbolic_math::Symbol y;
  constexpr symbolic_math::Symbol z;

  // build a symbolic expression
  constexpr symbolic_math::Expression f = 2.0 * x + (y - z) * pi;

  // evaluate the expression
  constexpr double result = f.evaluate({ x = 4.0, y = 2.0, z = 1.0 });
  constexpr double expected = 2.0 * 4.0 + (2.0 - 1.0) * 3.14159265358979323846;
  static_assert(result == expected, "result does not match expected value");

  return 0;
}