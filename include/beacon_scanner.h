#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
struct Point
{
  constexpr Point(int32_t x, int32_t y, int32_t z) : _point{x, y, z} {}
  explicit constexpr Point(std::array<int32_t, 3> point) : _point{std::move(point)} {}

  std::array<int32_t, 3> const& Coordinates() const { return _point; }
  constexpr int32_t X() const { return _point[0]; }
  constexpr int32_t Y() const { return _point[1]; }
  constexpr int32_t Z() const { return _point[2]; }

  constexpr std::array<int32_t, 3> TranslateTo(Point const& other) const
  {
    auto const& c = Coordinates();
    auto const& oc = other.Coordinates();
    return {oc[0] - c[0], oc[1] - c[1], oc[2] - c[2]};
  }

  bool operator==(Point const& other) const
  {
    return Coordinates() == other.Coordinates();
  }

  friend std::ostream& operator<<(std::ostream& os, Point const& point);

 private:
  std::array<int32_t, 3> _point;
};

inline std::ostream& operator<<(std::ostream& os, Point const& point)
{
  os << point._point;
  return os;
}

struct TranslationMatrix
{
  explicit constexpr TranslationMatrix(std::array<int32_t, 3> x_axis,
                                       std::array<int32_t, 3> y_axis,
                                       std::array<int32_t, 3> z_axis)
    : _x_axis(std::move(x_axis)), _y_axis(std::move(y_axis)), _z_axis(std::move(z_axis))
  {
  }

  constexpr Point Translate(Point const& point) const
  {
    int32_t x = point.X() * _x_axis[0];
    int32_t y = point.X() * _x_axis[1];
    int32_t z = point.X() * _x_axis[2];

    x += point.Y() * _y_axis[0];
    y += point.Y() * _y_axis[1];
    z += point.Y() * _y_axis[2];

    x += point.Z() * _z_axis[0];
    y += point.Z() * _z_axis[1];
    z += point.Z() * _z_axis[2];

    return Point(x, y, z);
  }

  std::array<int32_t, 3> _x_axis;
  std::array<int32_t, 3> _y_axis;
  std::array<int32_t, 3> _z_axis;
};

static constexpr std::array<TranslationMatrix, 24> AllTranslations = {
    TranslationMatrix{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
    TranslationMatrix{{1, 0, 0}, {0, -1, 0}, {0, 0, -1}},
    TranslationMatrix{{1, 0, 0}, {0, 0, -1}, {0, 1, 0}},
    TranslationMatrix{{1, 0, 0}, {0, 0, 1}, {0, -1, 0}},

    TranslationMatrix{{-1, 0, 0}, {0, 1, 0}, {0, 0, -1}},
    TranslationMatrix{{-1, 0, 0}, {0, -1, 0}, {0, 0, 1}},
    TranslationMatrix{{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},
    TranslationMatrix{{-1, 0, 0}, {0, 0, -1}, {0, -1, 0}},

    TranslationMatrix{{0, -1, 0}, {1, 0, 0}, {0, 0, 1}},
    TranslationMatrix{{0, 1, 0}, {1, 0, 0}, {0, 0, -1}},
    TranslationMatrix{{0, 0, -1}, {1, 0, 0}, {0, -1, 0}},
    TranslationMatrix{{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},

    TranslationMatrix{{0, 1, 0}, {-1, 0, 0}, {0, 0, 1}},
    TranslationMatrix{{0, -1, 0}, {-1, 0, 0}, {0, 0, -1}},
    TranslationMatrix{{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}},
    TranslationMatrix{{0, 0, 1}, {-1, 0, 0}, {0, -1, 0}},

    TranslationMatrix{{0, 0, -1}, {0, 1, 0}, {1, 0, 0}},
    TranslationMatrix{{0, 0, 1}, {0, -1, 0}, {1, 0, 0}},
    TranslationMatrix{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}},
    TranslationMatrix{{0, -1, 0}, {0, 0, -1}, {1, 0, 0}},

    TranslationMatrix{{0, 0, -1}, {0, -1, 0}, {-1, 0, 0}},
    TranslationMatrix{{0, 0, 1}, {0, 1, 0}, {-1, 0, 0}},
    TranslationMatrix{{0, -1, 0}, {0, 0, 1}, {-1, 0, 0}},
    TranslationMatrix{{0, 1, 0}, {0, 0, -1}, {-1, 0, 0}},
};

struct point_hash
{
  size_t operator()(Point const& p) const { return array_hash()(p.Coordinates()); }
};

using PointSet = std::unordered_set<Point, point_hash>;

struct Scanner
{
  void AddPoint(int32_t x, int32_t y, int32_t z) { _points.emplace(x, y, z); }
  PointSet const& Points() const { return _points; }
  PointSet& Points() { return _points; }

 private:
  PointSet _points;
};

struct Region
{
  explicit Region(PointSet beacons) : _beacons(std::move(beacons)) {}

  PointSet const& Beacons() const { return _beacons; }
  PointSet const& Scanners() const { return _scanners; }

  bool TryIntegrate(Scanner& scanner)
  {
    for(auto const& matrix : AllTranslations)
    {
      std::unordered_map<std::array<int32_t, 3>, size_t, array_hash> translations;
      for(auto const& point : scanner.Points())
      {
        auto translated_point = matrix.Translate(point);
        for(auto const& in_region : _beacons)
        {
          translations[translated_point.TranslateTo(in_region)]++;
        }
      }

      auto it = std::find_if(translations.begin(), translations.end(),
                             [](auto const& kvp) { return kvp.second >= 12; });
      if(it != translations.end())
      {
        auto const& translation = it->first;
        _scanners.emplace(matrix.Translate({0, 0, 0}).Coordinates() + translation);
        for(auto& point : scanner.Points())
        {
          _beacons.emplace(matrix.Translate(point).Coordinates() + translation);
        }
        return true;
      }
    }

    return false;
  }

  void Print() const
  {
    for(auto const& beacon : _beacons)
    {
      std::cout << beacon << std::endl;
    }
    std::cout << std::endl;
    for(auto const& scanner : _scanners)
    {
      std::cout << scanner << std::endl;
    }
  }

 private:
  PointSet _beacons;
  PointSet _scanners{{0, 0, 0}};
};

struct Trench
{
  void AddScanner() { _scanners.emplace_back(); }
  Scanner& CurrentScanner() { return _scanners.back(); }

  size_t NumBeacons()
  {
    auto region = run_region();
    return region.Beacons().size();
  }

  size_t ManhattanDistance()
  {
    auto region = run_region();
    size_t distance = 0;
    for(auto const& scan1 : region.Scanners())
    {
      for(auto const& scan2 : region.Scanners())
      {
        auto dist = std::abs(scan1.X() - scan2.X()) + std::abs(scan1.Y() - scan2.Y()) +
                    std::abs(scan1.Z() - scan2.Z());
        if(dist > distance) distance = dist;
      }
    }
    return distance;
  }

  void Print() const
  {
    for(auto const& scanner : _scanners)
    {
      std::cout << "--- scanner ---" << std::endl;
      for(auto const& point : scanner.Points())
      {
        std::cout << point.Coordinates() << std::endl;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

 private:
  Region run_region()
  {
    Region region(std::move(_scanners.front().Points()));
    _scanners.erase(_scanners.begin());
    std::stack<std::vector<Scanner>::iterator> integrated_scanners;
    while(!_scanners.empty())
    {
      for(auto it = _scanners.begin(); it != _scanners.end(); ++it)
      {
        if(region.TryIntegrate(*it))
        {
          integrated_scanners.push(it);
        }
      }

      if(integrated_scanners.empty())
      {
        region.Print();
        throw std::out_of_range("No scanners could be integrated, this is a bug");
      }
      while(!integrated_scanners.empty())
      {
        auto it = integrated_scanners.top();
        _scanners.erase(it);
        integrated_scanners.pop();
      }
    }

    return region;
  }

 private:
  std::vector<Scanner> _scanners;
};

inline Trench parse_scanners()
{
  auto input = open_input("inputs/19-1.txt");
  std::string line, x, y, z;
  Trench trench;
  while(std::getline(input, line))
  {
    if(line.starts_with("---"))
      trench.AddScanner();
    else if(line.empty())
      continue;
    else
    {
      std::istringstream iss(line);
      auto& scanner = trench.CurrentScanner();
      if(!iss.getline(x.data(), 6, ',') || !iss.getline(y.data(), 6, ',') ||
         !iss.getline(z.data(), 6))
      {
        throw std::out_of_range("Failed to parse point");
      }
      scanner.AddPoint(std::atoi(x.c_str()), std::atoi(y.c_str()), std::atoi(z.c_str()));
    }
  }

  return trench;
}
}  // namespace aoc
