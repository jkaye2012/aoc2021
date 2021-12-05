#include <concepts>
#include <span>
#include <type_traits>

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
}  // namespace aoc
