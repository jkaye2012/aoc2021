#pragma once

#include <cmath>
#include <cstddef>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
inline int32_t subtract(std::pair<int32_t, int32_t> const& p)
{
  return p.second - p.first + 1;
}

struct Cuboid
{
  using AxisRange = std::pair<int32_t, int32_t>;

  size_t Size() const
  {
    return subtract(x_range) * subtract(y_range) * subtract(z_range);
  }

  std::pair<size_t, Cuboid> OverlapWith(Cuboid const& other) const
  {
    auto [x_size, x_overlap] = AxisOverlap(x_range, other.x_range);
    auto [y_size, y_overlap] = AxisOverlap(y_range, other.y_range);
    auto [z_size, z_overlap] = AxisOverlap(z_range, other.z_range);
    auto size = x_size * y_size * z_size;
    return {size, Cuboid{on, x_overlap, y_overlap, z_overlap}};
  }

  std::pair<size_t, AxisRange> AxisOverlap(AxisRange const& fst,
                                           AxisRange const& snd) const
  {
    auto const& [smaller, larger] =
        fst.first <= snd.first ? std::make_pair(fst, snd) : std::make_pair(snd, fst);
    if(smaller.second < larger.first)
    {
      return {0, {0, 0}};
    }
    else if(smaller.second > larger.second)
    {
      return {subtract(larger), larger};
    }
    else
    {
      return {smaller.second - larger.first + 1, {larger.first, smaller.second}};
    }
  }

  bool on;
  AxisRange x_range;
  AxisRange y_range;
  AxisRange z_range;
};

struct ContiguousRegion
{
  void Merge(Cuboid const& candidate)
  {
    if(_cuboids.empty())
    {
      _cuboids.push_back(candidate);
      return;
    }

    if(candidate.on)
    {
      for(auto const& overlap : _overlaps)
      {
        if(auto [size, _] = candidate.OverlapWith(overlap); size > 0)
        {
          _overlap_size -= size;
        }
      }

      for(auto const& cuboid : _cuboids)
      {
        if(auto [size, overlap] = candidate.OverlapWith(cuboid); size > 0)
        {
          _overlaps.emplace_back(std::move(overlap));
          _overlap_size += size;
        }
      }

      _cuboids.push_back(candidate);
    }
  }

  size_t Size() const
  {
    auto size = 0;
    for(auto const& c : _cuboids)
    {
      size += c.Size();
    }

    size -= _overlap_size;
    size -= _num_off;
    return size;
  }

  std::vector<Cuboid> _cuboids;
  std::vector<Cuboid> _overlaps;
  std::vector<Cuboid> _off;
  size_t _overlap_size = 0;
  size_t _num_off = 0;
};

inline size_t OverlapReduce(std::vector<Cuboid> const& overlaps, Cuboid const& overlap)
{
  std::vector<Cuboid> os;
  size_t remaining = overlap.Size();
  for(auto const& existing : overlaps)
  {
    auto [size, o] = overlap.OverlapWith(existing);
    remaining -= size;
    os.emplace_back(std::move(o));
  }
  return remaining;
}

struct ThirdTry
{
  std::vector<Cuboid> _cuboids;
  size_t _total_on = 0;

  size_t On() const { return _total_on; }

  void Process(Cuboid const& cuboid)
  {
    if(_cuboids.empty())
    {
      _cuboids.push_back(cuboid);
      _total_on += cuboid.Size();
      return;
    }

    size_t remaining = cuboid.Size();
    std::vector<Cuboid> overlaps;
    for(auto it = _cuboids.rbegin(); it != _cuboids.rend(); ++it)
    {
      if(remaining == 0) break;
      auto [overlap_size, overlap] = it->OverlapWith(cuboid);
      auto size = overlap_size > 0 ? OverlapReduce(overlaps, overlap) : 0;
      if(size > 0)
      {
        remaining -= size;
        overlaps.emplace_back(std::move(overlap));
        if(cuboid.on && !overlap.on)
        {
          _total_on += size;
        }
        else if(!cuboid.on && overlap.on)
        {
          _total_on -= size;
        }
      }
    }
    if(cuboid.on)
    {
      _total_on += remaining;
    }
    std::cout << "size: " << cuboid.Size() << ", on: " << cuboid.on
              << ", total: " << _total_on << std::endl;

    _cuboids.push_back(cuboid);
  }
};

struct Reactor
{
  Cuboid& AddInstruction() { return _instructions.emplace_back(); }

  void ProcessInstructions()
  {
    for(auto it = _instructions.begin(); it != _instructions.end(); ++it)
    {
      if(!it->on) continue;
      _total_on += it->Size() - FutureOverlaps2(it, _instructions.end());
      std::cout << _total_on << std::endl;
    }
  }

  size_t CountCubes() const { return _total_on; }

  size_t FutureOverlaps2(std::vector<Cuboid>::iterator const& current,
                         std::vector<Cuboid>::iterator const& end)
  {
    std::vector<Cuboid> overlaps;
    size_t overlap_count = 0;
    for(auto futureIt = current + 1; futureIt != end; ++futureIt)
    {
      auto [size, overlap] = current->OverlapWith(*futureIt);
      if(size > 0)
      {
        overlap_count += size;
        overlaps.emplace_back(std::move(overlap));
      }
    }
    if(overlap_count > 0)
    {
      overlap_count -= FutureOverlaps2(overlaps.begin(), overlaps.end());
    }
    return overlap_count;
  }

  size_t FutureOverlaps(std::vector<Cuboid>::iterator const& current)
  {
    std::vector<Cuboid> overlaps;
    size_t overlap_count = 0;
    for(auto futureIt = current + 1; futureIt != _instructions.end(); ++futureIt)
    {
      auto [size, overlap] = current->OverlapWith(*futureIt);
      if(size > 0)
      {
        auto repeated_overlaps = RepeatedOverlaps(overlap, overlaps);
        if(repeated_overlaps != size)
        {
          overlap_count += size - repeated_overlaps;
          overlaps.emplace_back(std::move(overlap));
        }
      }
    }
    return overlap_count;
  }

  size_t RepeatedOverlaps(Cuboid const& overlap, std::vector<Cuboid> const& overlaps)
  {
    size_t repeated = 0;
    for(auto const& o : overlaps)
    {
      auto [size, _] = overlap.OverlapWith(o);
      repeated += size;
    }

    return std::min(overlap.Size(), repeated);
  }

 private:
  size_t _total_on = 0;
  std::vector<Cuboid> _instructions;
};

inline std::ostream& operator<<(std::ostream& os, Cuboid const& instr)
{
  os << (instr.on ? "on" : "off") << " " << instr.x_range << " " << instr.y_range << " "
     << instr.z_range;
  return os;
}

inline Reactor parse_reactor()
{
  auto input = open_input("inputs/22-1.txt");
  Reactor reactor;
  std::string line, token;
  while(std::getline(input, line))
  {
    std::istringstream iss(line);
    auto& instruction = reactor.AddInstruction();
    std::getline(iss, token, ' ');
    instruction.on = token == "on";
    iss.ignore(2);
    iss >> instruction.x_range.first;
    iss.ignore(2);
    iss >> instruction.x_range.second;
    iss.ignore(3);
    iss >> instruction.y_range.first;
    iss.ignore(2);
    iss >> instruction.y_range.second;
    iss.ignore(3);
    iss >> instruction.z_range.first;
    iss.ignore(2);
    iss >> instruction.z_range.second;
  }

  return reactor;
}
}  // namespace aoc
