#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <iostream>
#include <ostream>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
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

template <class A>
concept Addable = requires(A a)
{
  // clang-format off
  { a + a } -> std::same_as<A>;
  // clang-format on
};

template <class A>
concept Subtractable = requires(A a)
{
  // clang-format off
  { a - a } -> std::same_as<A>;
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

struct pair_hash
{
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& p) const
  {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    return h1 ^ h2;
  }
};

template <std::size_t Index, typename... T>
struct tuple_hash_impl
{
  using Tuple = std::tuple<T...>;

  std::size_t operator()(Tuple const& t)
  {
    if constexpr(Index == 0)
    {
      return std::get<Index>(t);
    }
    else
    {
      return std::hash<std::tuple_element_t<Index, Tuple>>()(std::get<Index>(t)) ^
             tuple_hash_impl<Index - 1, T...>()(t);
    }
  }
};

struct tuple_hash
{
  template <typename... T>
  std::size_t operator()(std::tuple<T...> const& t) const
  {
    return tuple_hash_impl<std::tuple_size_v<std::tuple<T...>> - 1, T...>()(t);
  }
};

struct array_hash
{
  template <typename T, size_t N>
  std::size_t operator()(std::array<T, N> const& a) const
  {
    auto hasher = std::hash<T>();
    size_t result = 0;
    for(size_t n = 0; n < N; ++n)
    {
      result ^= hasher(a[n]);
    }
    return result;
  }
};

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, std::array<T, N> const& a)
{
  for(size_t n = 0; n < N; ++n)
  {
    os << a[n];
    if(n != N - 1) os << ",";
  }
  return os;
}

template <typename T, size_t N>
std::array<T, N> operator+(std::array<T, N> const& fst,
                           std::array<T, N> const& snd) requires Addable<T>
{
  std::array<T, N> result;
  for(size_t n = 0; n < N; ++n)
  {
    result[n] = fst[n] + snd[n];
  }
  return result;
}

template <typename T, size_t N>
std::array<T, N> operator-(std::array<T, N> const& fst,
                           std::array<T, N> const& snd) requires Subtractable<T>
{
  std::array<T, N> result;
  for(size_t n = 0; n < N; ++n)
  {
    result[n] = fst[n] - snd[n];
  }
  return result;
}

template <typename F, typename S>
std::ostream& operator<<(std::ostream& os, std::pair<F, S> const& p)
{
  os << p.first << "," << p.second;
  return os;
}
}  // namespace aoc
