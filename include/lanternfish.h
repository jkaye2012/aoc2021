#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <valarray>
#include <vector>

#include "parse.h"

namespace aoc
{
static constexpr uint32_t SpawnCycle = 7;
static constexpr uint32_t SpawnDelay = 2;

struct School
{
  size_t size() const { return _fish_in_cycle.sum() + _fish_in_delay.sum(); }

  void add_fish_to_cycle(uint32_t cycle) { _fish_in_cycle[cycle]++; }

  size_t pass_days(size_t days)
  {
    for(auto i = 0; i < days; ++i)
    {
      auto toSpawn = _fish_in_cycle[0];
      _fish_in_cycle = _fish_in_cycle.cshift(1);
      _fish_in_cycle[SpawnCycle - 1] += _fish_in_delay[0];
      _fish_in_delay = _fish_in_delay.shift(1);
      _fish_in_delay[SpawnDelay - 1] = toSpawn;
    }

    return size();
  }

 private:
  std::valarray<size_t> _fish_in_cycle = std::valarray<size_t>(SpawnCycle);
  std::valarray<size_t> _fish_in_delay = std::valarray<size_t>(SpawnDelay);
};

inline School parse_school()
{
  auto input = open_input("./inputs/6-1.txt");
  std::string line;
  School school;
  while(std::getline(input, line))
  {
    std::istringstream iss(line);
    while(!iss.eof())
    {
      uint32_t cycle;
      iss >> cycle;
      school.add_fish_to_cycle(cycle);
      iss.ignore(1, ',');
    }
  }

  return school;
}
}  // namespace aoc
