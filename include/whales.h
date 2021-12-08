#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <valarray>

#include "parse.h"
#include "util.h"

namespace aoc
{
struct CrabArmy
{
  void add_solider(uint32_t position)
  {
    _max_position = std::max(position, _max_position);
    _soliders_by_position[position]++;
  }

  void print()
  {
    for(auto const& [pos, num] : _soliders_by_position)
    {
      std::cout << pos << "=" << num << " ";
    }
    std::cout << std::endl;
  }

  uint32_t min_alignment() const
  {
    std::valarray<uint32_t> initial_positions(_max_position + 1),
        cost_vector(_max_position + 1);
    for(auto const& [pos, num] : _soliders_by_position)
    {
      initial_positions[pos] = num;
    }
    for(auto i = 1; i < _max_position + 1; ++i)
    {
      cost_vector[i] = i + cost_vector[i - 1];
    }
    uint32_t cost = std::numeric_limits<uint32_t>::max();
    for(uint32_t aligned_pos = 0; aligned_pos < _max_position + 1; ++aligned_pos)
    {
      uint32_t next_cost = (initial_positions * cost_vector).sum();
      if(next_cost > cost) break;
      cost_vector = cost_vector.cshift(-1);
      cost_vector[0] = aligned_pos + 1 + cost_vector[1];
      cost = next_cost;
    }

    return cost;
  }

 private:
  uint32_t _max_position = 0;
  std::map<uint32_t, uint32_t> _soliders_by_position;
};

inline CrabArmy parse_crabs()
{
  auto input = open_input("./inputs/7-1.txt");
  std::string line;
  CrabArmy army;
  while(std::getline(input, line))
  {
    std::istringstream iss(line);
    while(!iss.eof())
    {
      uint32_t pos;
      iss >> pos;
      army.add_solider(pos);
      iss.ignore(1, ',');
    }
  }

  return army;
}
}  // namespace aoc
