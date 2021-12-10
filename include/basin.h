#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <valarray>
#include <vector>

#include "parse.h"

namespace aoc
{
enum class Direction
{
  Up,
  Down,
  Left,
  Right
};

using Coord = std::pair<uint32_t, uint32_t>;
using Coords = std::vector<Coord>;

struct Heightmap
{
  Heightmap(uint32_t width, uint32_t height, std::vector<uint32_t> locations)
    : _width(width), _height(height), _size(width * height), _locations(_size)
  {
    if(locations.size() != _size) throw std::out_of_range("Invalid number of locations");
    for(auto idx = 0; idx < _size; ++idx)
    {
      _locations[idx] = locations[idx];
    }
  }

  std::map<Direction, uint32_t> neighbors(uint32_t x, uint32_t y) const
  {
    std::map<Direction, uint32_t> n;
    if(x != 0) n[Direction::Left] = at(x - 1, y);
    if(x < _width - 1) n[Direction::Right] = at(x + 1, y);
    if(y != 0) n[Direction::Up] = at(x, y - 1);
    if(y < _height - 1) n[Direction::Down] = at(x, y + 1);
    return n;
  }

  std::vector<Coord> neighbors(Coord coord) const
  {
    auto [x, y] = coord;
    std::vector<Coord> n;
    if(x != 0) n.emplace_back(x - 1, y);
    if(x < _width - 1) n.emplace_back(x + 1, y);
    if(y != 0) n.emplace_back(x, y - 1);
    if(y < _height - 1) n.emplace_back(x, y + 1);
    return n;
  }

  uint32_t total_risk() const
  {
    uint32_t risk = 0;
    for(auto [x, y] : find_low_points())
    {
      risk += at(x, y) + 1;
    }
    return risk;
  }

  uint32_t basin_risk() const
  {
    auto basins = find_basins();
    std::sort(basins.begin(), basins.end(), std::greater<uint32_t>());
    return basins.at(0) * basins.at(1) * basins.at(2);
  }

 private:
  uint32_t at(uint32_t x, uint32_t y) const { return _locations[y * _width + x]; }
  uint32_t at(Coord const& c) const { return at(c.first, c.second); }

  Coords find_low_points() const
  {
    Coords low_points;
    for(auto h = 0; h < _height; ++h)
    {
      for(auto w = 0; w < _width; ++w)
      {
        auto l = at(w, h);
        auto n = neighbors(w, h);
        if(std::all_of(n.begin(), n.end(), [l](auto it) { return it.second > l; }))
        {
          low_points.emplace_back(w, h);
        }
      }
    }

    return low_points;
  }

  std::vector<uint32_t> find_basins() const
  {
    std::vector<uint32_t> basins;
    for(auto c : find_low_points())
    {
      std::set<Coord> seen;
      basins.push_back(basin_recurse(seen, c));
    }
    return basins;
  }

  uint32_t basin_recurse(std::set<Coord>& seen, Coord coord) const
  {
    if(at(coord) == 9 || seen.contains(coord)) return 0;
    seen.insert(coord);
    auto n = neighbors(coord);
    return std::accumulate(n.begin(), n.end(), 1, [&seen, this](auto r, auto const& c) {
      return r + basin_recurse(seen, c);
    });
  }

 private:
  uint32_t _width;
  uint32_t _height;
  uint32_t _size;
  std::valarray<uint32_t> _locations;
};

inline Heightmap parse_basin()
{
  auto input = open_input("./inputs/9-1.txt");
  std::string line;
  uint32_t width = 0;
  uint32_t height = 0;
  std::vector<uint32_t> locations;
  while(std::getline(input, line))
  {
    ++height;
    std::istringstream iss(line);
    char l;
    bool should_calc_width = width == 0;
    while(iss.get(l))
    {
      if(should_calc_width) ++width;
      locations.push_back(l - 48);
    }
  }

  return Heightmap(width, height, std::move(locations));
}
}  // namespace aoc
