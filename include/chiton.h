#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
using Point = std::pair<size_t, size_t>;
using ByLocation = std::unordered_map<Point, size_t, pair_hash>;

static inline std::unordered_map<Point, std::vector<Point>, pair_hash> neighbors_impl(
    size_t width, size_t height)
{
  std::unordered_map<Point, std::vector<Point>, pair_hash> result;
  for(auto x = 0; x < width; ++x)
  {
    for(auto y = 0; y < height; ++y)
    {
      Point point{x, y};
      if(point.first > 0) result[point].emplace_back(point.first - 1, point.second);
      if(point.first < width - 1)
        result[point].emplace_back(point.first + 1, point.second);
      if(point.second > 0) result[point].emplace_back(point.first, point.second - 1);
      if(point.second < height - 1)
        result[point].emplace_back(point.first, point.second + 1);
    }
  }

  return result;
}

static inline std::vector<Point> const& neighbors(Point const& point, size_t width,
                                                  size_t height)
{
  static auto map = neighbors_impl(width, height);
  return map.at(point);
}

struct Location
{
  explicit Location(Point point) : Id(std::move(point)) {}

  Point Id;
  size_t Cost = std::numeric_limits<size_t>::max();

  bool operator<(Location const& other) const
  {
    if(Cost != other.Cost) return Cost < other.Cost;
    return Id < other.Id;
  }
};

struct ChitonCave
{
  size_t Width;
  size_t Height;
  ByLocation Costs;

  void print() const
  {
    for(auto y = 0; y < Height; ++y)
    {
      for(auto x = 0; x < Width; ++x)
      {
        std::cout << Costs.at({x, y});
      }
      std::cout << std::endl;
    }
  }

  void replicate(size_t num)
  {
    auto new_width = Width * num;
    auto new_height = Height * num;
    for(auto x = 0; x < new_width; ++x)
    {
      for(auto y = 0; y < new_height; ++y)
      {
        if(x < Width && y < Height) continue;
        auto refx = x % Width;
        auto refy = y % Height;
        auto distance = x / Width + y / Height;
        Costs[{x, y}] = Costs[{refx, refy}] + distance;
        if(Costs[{x, y}] > 9) Costs[{x, y}] = Costs[{x, y}] - 9;
      }
    }
    Width = new_width;
    Height = new_height;
  }

  size_t shortest_path() const
  {
    ByLocation tentative;
    Location origin({0, 0});
    origin.Cost = 0;
    std::set<Location> unvisited{origin};
    for(auto const& [point, _] : Costs)
    {
      unvisited.emplace(point);
      tentative[point] = std::numeric_limits<size_t>::max();
    }
    tentative[origin.Id] = 0;
    Location end({Width - 1, Height - 1});
    while(unvisited.contains(end))
    {
      search(tentative, unvisited);
    }

    return tentative.at(end.Id);
  }

 private:
  void search(ByLocation& tentative, std::set<Location>& unvisited) const
  {
    auto pointIt = unvisited.begin();
    auto const& ns = neighbors(pointIt->Id, Width, Height);
    for(auto const& n : ns)
    {
      Location locn(n);
      if(unvisited.contains(locn))
      {
        auto prev = tentative.at(n);
        auto curr = tentative.at(pointIt->Id) + Costs.at(n);
        if(curr < prev)
        {
          tentative[n] = curr;
          unvisited.erase(locn);
          locn.Cost = curr;
          unvisited.insert(locn);
        }
      }
    }
    unvisited.erase(pointIt);
  }
};

inline ChitonCave parse_chiton()
{
  auto input = open_input("./inputs/15-1.txt");
  ChitonCave cave;
  size_t x = 0, y = 0;
  std::string line;
  while(std::getline(input, line))
  {
    x = 0;
    for(auto c : line)
    {
      cave.Costs.emplace(std::make_pair(x++, y), c - '0');
    }

    ++y;
  }
  cave.Width = x;
  cave.Height = y;
  return cave;
}
}  // namespace aoc
