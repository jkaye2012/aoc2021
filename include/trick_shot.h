#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <utility>

namespace aoc
{
inline int x_after_steps(int vx, size_t steps)
{
  size_t avx = std::abs(vx);
  auto min = std::min(avx, steps);
  size_t dx = (min / 2.0) * (2 * avx - min + 1);
  return vx < 0 ? -dx : dx;
}

inline int y_after_steps(int vy, size_t steps)
{
  int uy = vy - (steps - 1);
  return 0.5 * steps * (vy + uy);
}

inline std::pair<int, int> location_after_steps(int vx, int vy, size_t steps)
{
  return {x_after_steps(vx, steps), y_after_steps(vy, steps)};
}

inline std::pair<size_t, size_t> possible_trick_steps(std::pair<int, int> const& x_target)
{
  std::pair<size_t, size_t> result{0, 0};
  size_t steps = 1;
  for(int x = x_after_steps(steps, steps); x <= x_target.second;
      ++steps, x = x_after_steps(steps, steps))
  {
    if(x >= x_target.first && result.first == 0) result.first = steps;
  }
  result.second = steps - 1;
  return result;
}

inline std::pair<int, int> maximize_y(std::pair<int, int> const& y_target, size_t steps)
{
  auto max_vy = std::numeric_limits<int>::min();
  int max_y;
  for(size_t step = steps; true; ++step)
  {
    auto y = y_after_steps(step, step * 2 + 2);
    if(y >= y_target.first && y <= y_target.second)
    {
      max_vy = step;
      max_y = y_after_steps(step, step);
    }

    if(max_vy > std::numeric_limits<int>::min() && y < y_target.first) break;
  }
  return {max_vy, max_y};
}

inline std::pair<int, int> trick_shot(std::pair<int, int> const& x_target,
                                      std::pair<int, int> const& y_target)
{
  std::pair<int, int> shot{0, std::numeric_limits<int>::min()};
  auto [step_min, step_max] = possible_trick_steps(x_target);
  for(auto step = step_min; step <= step_max; ++step)
  {
    auto [vy, y] = maximize_y(y_target, step);
    if(vy > shot.second)
    {
      shot.first = step;
      shot.second = vy;
      std::cout << step << " " << vy << " " << y << std::endl;
    }
  }

  return shot;
}

inline std::map<size_t, std::vector<int>> possible_vx(std::pair<int, int> const& x_target)
{
  std::map<size_t, std::vector<int>> result;
  for(auto vx = 0; vx <= x_target.second; ++vx)
  {
    for(auto step = 1, x = x_after_steps(vx, step); x <= x_target.second && x >= step;
        ++step, x = x_after_steps(vx, step))
    {
      if(x <= x_target.second && x >= x_target.first)
      {
        result[step].push_back(vx);
      }
    }
  }
  return result;
}

inline std::map<size_t, std::vector<int>> possible_vy(std::pair<int, int> const& y_target)
{
  std::map<size_t, std::vector<int>> result;
  for(auto vy = y_target.first; vy <= std::abs(y_target.first); ++vy)
  {
    for(auto step = 1, y = y_after_steps(vy, step); y >= y_target.first;
        ++step, y = y_after_steps(vy, step))
    {
      if(y >= y_target.first && y <= y_target.second)
      {
        result[step].push_back(vy);
      }
    }
  }
  return result;
}

inline size_t num_velocities(std::pair<int, int> const& x_target,
                             std::pair<int, int> const& y_target)
{
  auto x_possibilities = possible_vx(x_target);
  auto y_possibilities = possible_vy(y_target);
  std::set<std::pair<int, int>> velocities;
  for(auto const& [step, vxs] : x_possibilities)
  {
    for(auto const& vy : y_possibilities[step])
    {
      for(auto const& vx : vxs)
      {
        velocities.insert({vx, vy});
      }
    }
  }
  return velocities.size();
}
}  // namespace aoc
