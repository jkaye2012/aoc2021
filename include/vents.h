#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>
#include <valarray>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
struct Point
{
  uint32_t x = 0;
  uint32_t y = 0;

  uint32_t distance(Point const& other) const { return abs_diff(this->x, other.x) + 1; }

  int32_t slope(Point const& other) const
  {
    // Slope is inverted from standard cartesian
    return -1 * static_cast<int32_t>(this->y - other.y) /
           static_cast<int32_t>(this->x - other.x);
  }
};

inline std::istream& operator>>(std::istream& is, Point& point)
{
  is >> point.x;
  is.ignore(1, ',');
  is >> point.y;
  return is;
}

struct Line
{
  Point start{};
  Point end{};

  std::slice plane_impact(size_t width) const
  {
    if(start.x == end.x)  // Vertical
    {
      return std::slice(start.x + width * std::min(start.y, end.y),
                        abs_diff(start.y, end.y) + 1, width);
    }
    else if(start.y == end.y)  // Horizontal
    {
      return std::slice(start.y * width + std::min(start.x, end.x),
                        abs_diff(start.x, end.x) + 1, 1);
    }

    // Diagonal
    auto distance = start.distance(end);
    auto slope = start.slope(end);
    auto const& lowest = start.y < end.y ? start : end;
    return std::slice(lowest.x + lowest.y * width, distance, width - slope);
  }
};

inline std::istream& operator>>(std::istream& is, Line& line)
{
  is >> line.start;
  is.ignore(3, '>');
  is >> line.end;
  is >> std::ws;
  return is;
}

class Plane
{
 public:
  explicit Plane(std::vector<Line> lines) : _lines(std::move(lines))
  {
    // Used for part 1 only.
    // _lines.erase(std::remove_if(_lines.begin(), _lines.end(),
    //                             [](auto const& line) {
    //                               return line.start.x != line.end.x &&
    //                                      line.start.y != line.end.y;
    //                             }),
    //              _lines.end());

    for(auto const& line : _lines)
    {
      _width = std::max({_width, line.start.x, line.end.x});
      _height = std::max({_height, line.start.y, line.end.y});
    }
    _width += 1;
    _height += 1;

    _plane = std::valarray<uint32_t>(size());
    for(auto const& line : _lines)
    {
      auto impact = line.plane_impact(_width);
      std::valarray<uint32_t> s = _plane[impact];
      s += 1;
      _plane[impact] = s;
    }
  }

  uint32_t size() const { return _width * _height; }

  uint32_t count() const { return _lines.size(); }

  void print() const
  {
    for(auto col = 0; col < _height; ++col)
    {
      for(auto row = 0; row < _width; ++row)
      {
        std::cout << _plane[col * _width + row] << " ";
      }
      std::cout << std::endl;
    }
  }

  uint32_t count_where(std::function<bool(uint32_t)> const& pred) const
  {
    uint32_t result = 0;
    for(auto v : _plane)
    {
      if(pred(v)) result++;
    }

    return result;
  }

 private:
  uint32_t _width = 0;
  uint32_t _height = 0;
  std::vector<Line> _lines;
  std::valarray<uint32_t> _plane;
};

inline Plane parse_plane()
{
  auto input = open_input("./inputs/5-1.txt");
  std::string line;
  std::vector<Line> lines;
  while(std::getline(input, line))
  {
    std::istringstream iss(line);
    iss >> lines.emplace_back();
  }

  return Plane(std::move(lines));
}

}  // namespace aoc
