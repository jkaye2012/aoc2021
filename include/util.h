#pragma once

#include <cmath>
#include <concepts>
#include <iostream>
#include <ostream>
#include <span>
#include <type_traits>
#include <valarray>

namespace aoc
{
template <class M>
concept Monoid = std::is_default_constructible_v<M> && requires(M& t, M const& v)
{
  // clang-format off
  { t += v } -> std::same_as<M&>;
  // clang-format on
};

template <typename T, typename R = typename std::remove_cv<T>::type>
R sum(std::span<T> s) requires Monoid<R>
{
  R result{};
  for(auto i : s)
  {
    result += i;
  }

  return result;
}

template <std::unsigned_integral U, typename S = std::make_signed_t<U>>
U abs_diff(U fst, U snd)
{
  return std::abs(static_cast<S>(fst - snd));
}

template <typename T>
std::ostream& operator<<(std::ostream& os, std::valarray<T> const& v)
{
  for(auto val : v)
  {
    os << val << " ";
  }
  os << std::endl;
  return os;
}

template <typename T>
void print_matrix(std::valarray<T> const& v, size_t width)
{
  auto w = 0;
  for(auto val : v)
  {
    std::cout << val;
    if(++w == width)
    {
      w = 0;
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}
}  // namespace aoc
