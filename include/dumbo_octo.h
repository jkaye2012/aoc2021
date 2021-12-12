#pragma once

#include <cstddef>
#include <iostream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <valarray>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
static constexpr size_t Width = 10;
static constexpr size_t Height = 10;
static constexpr size_t Size = Width * Height;
static const std::valarray<size_t> StepMatrix(1, Size);

inline std::map<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>>
all_neighbors()
{
  std::map<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>> res;
  for(auto x = 0; x < Width; ++x)
  {
    for(auto y = 0; y < Height; ++y)
    {
      std::pair<size_t, size_t> coord{x, y};
      for(auto dx = -1; dx <= 1; ++dx)
      {
        if((dx == -1 && x == 0) || (dx == 1 && x == Width - 1)) continue;
        for(auto dy = -1; dy <= 1; ++dy)
        {
          if((dy == -1 && y == 0) || (dy == 1 && y == Height - 1)) continue;
          if(dx == 0 && dy == 0) continue;
          res[coord].push_back({x + dx, y + dy});
        }
      }
    }
  }
  return res;
}

static inline std::vector<std::pair<size_t, size_t>> neighbors(size_t x, size_t y)
{
  static auto _neighbors = all_neighbors();
  return _neighbors.at({x, y});
}

struct Step
{
  size_t total() const { return _total; }

  bool set(size_t x, size_t y)
  {
    auto& f = _flashed[y * Width + x];
    if(f) return false;
    f = true;
    _total++;
    return true;
  }

  bool converged() const { return total() == Size; }

  void print() { print_matrix(_flashed, Width); }

 private:
  size_t _total = 0;
  std::valarray<bool> _flashed = std::valarray<bool>(Size);
};

struct DumboOctopus
{
  [[nodiscard]] size_t& at(size_t x, size_t y) { return _octopi[y * Width + x]; }
  [[nodiscard]] size_t& at(std::pair<size_t, size_t> const& p)
  {
    return at(p.first, p.second);
  }

  void print() const { print_matrix(_octopi, Width); }

  size_t run_steps(size_t steps)
  {
    size_t flashed = 0;
    for(auto i = 0; i < steps; ++i)
    {
      Step step;
      _octopi += StepMatrix;
      flash(step);
      flashed += step.total();
      reset();

      // step.print();
      // std::cout << "Flashed: " << flashed << std::endl;
      // print();
    }
    return flashed;
  }

  size_t run_until_convergence()
  {
    size_t steps = 0;
    Step step;
    while(!step.converged())
    {
      step = Step();
      ++steps;
      _octopi += StepMatrix;
      flash(step);
      reset();

      // step.print();
      // std::cout << "Flashed: " << flashed << std::endl;
      // print();
    }
    return steps;
  }

 private:
  void flash(Step& step)
  {
    auto flashed = false;
    for(auto y = 0; y < Height; ++y)
    {
      for(auto x = 0; x < Width; ++x)
      {
        if(at(x, y) > 9 && step.set(x, y))
        {
          flashed = true;
          for(auto const& neighbor : neighbors(x, y))
          {
            at(neighbor) += 1;
          }
        }
      }
    }
    if(flashed) flash(step);
  }

  void reset()
  {
    _octopi = _octopi.apply([](auto const& e) { return e > 9 ? 0 : e; });
  }

  std::valarray<size_t> _octopi = std::valarray<size_t>(Size);
};

inline DumboOctopus parse_dumbo()
{
  auto input = open_input("./inputs/11-1.txt");
  DumboOctopus dumbo;
  std::string line;
  for(auto y = 0; y < Height; ++y)
  {
    if(!std::getline(input, line))
      throw std::out_of_range("Failed to parse required line");
    for(auto x = 0; x < Width; ++x)
    {
      auto o = line.substr(x, 1);
      dumbo.at(x, y) = std::atoi(o.c_str());
    }
  }

  return dumbo;
}
}  // namespace aoc
