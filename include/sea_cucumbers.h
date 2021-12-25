#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <valarray>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
enum class Cucumber : uint8_t
{
  Nonexistant,
  East,
  South
};

inline std::ostream& operator<<(std::ostream& os, Cucumber const& c)
{
  switch(c)
  {
    case Cucumber::Nonexistant:
      os << '.';
      break;
    case Cucumber::East:
      os << ">";
      break;
    case Cucumber::South:
      os << "v";
      break;
  }
  return os;
}

struct FastField
{
  FastField(size_t width, size_t height)
    : _width(width), _height(height), _field(width * height)
  {
  }

  void Populate(std::vector<Cucumber> const& cucumbers)
  {
    for(size_t n = 0; n < cucumbers.size(); ++n)
    {
      int8_t val;
      switch(cucumbers[n])
      {
        case Cucumber::Nonexistant:
          val = 0;
          break;
        case Cucumber::East:
          val = 1;
          break;
        case Cucumber::South:
          val = 3;
          break;
      }
      _field[n / _width * _width + n % _width] = val;
    }
  }

  void Print() const
  {
    for(auto n = 0; n < _width * _height; ++n)
    {
      char c = 'O';
      switch(_field[n])
      {
        case 0:
          c = '.';
          break;
        case 1:
          c = '>';
          break;
        case 3:
          c = 'v';
          break;
      }

      std::cout << c;
      if((n + 1) % _width == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  std::slice_array<int8_t> row(size_t n)
  {
    return _field[std::slice(n * _width, _width, 1)];
  }

  std::slice_array<int8_t> col(size_t n)
  {
    return _field[std::slice(n, _height, _width)];
  }

  int8_t move_east()
  {
    size_t moves = 0;
    for(size_t n = 0; n < _height; ++n)
    {
      std::valarray<int8_t> r = row(n);
      r = r.cshift(-1) + r;
      r = r.apply([](int8_t v) -> int8_t { return v == 1 ? 1 : 0; });
      moves += r.sum();
      r = r.cshift(1) * -1 + r;
      row(n) += r;
    }
    return moves;
  }

  int8_t move_south()
  {
    size_t moves = 0;
    for(size_t n = 0; n < _width; ++n)
    {
      std::valarray<int8_t> r = col(n);
      r = r.cshift(-1) + r;
      r = r.apply([](int8_t v) -> int8_t { return v == 3 ? 3 : 0; });
      moves += r.sum();
      r = r.cshift(1) * -1 + r;
      col(n) += r;
    }
    return moves;
  }

  size_t move()
  {
    size_t steps = 0;
    size_t moves;
    do
    {
      Print();
      moves = 0;
      ++steps;
      moves += move_east();
      moves += move_south();
    } while(moves > 0 && steps < 2);

    return steps;
  }

 private:
  size_t _width;
  size_t _height;
  std::valarray<int8_t> _field;
};

struct CucumberField
{
  CucumberField(size_t width, size_t height)
    : _width(width), _height(height), _cucumbers(width * height)
  {
  }

  void Populate(std::vector<Cucumber> const& cucumbers)
  {
    for(size_t n = 0; n < cucumbers.size(); ++n)
    {
      _cucumbers[n / _width * _width + n % _width] = cucumbers[n];
    }
  }

  void Print() const { aoc::print_matrix(_cucumbers, _width); }

  size_t CountMoves()
  {
    size_t num_steps = 0;
    size_t num_moved = 0;
    std::valarray<uint8_t> east_moves(_width * _height), south_moves(_width * _height);
    do
    {
      east_moves = east_moves.apply([](uint8_t _) -> uint8_t { return 0; });
      south_moves = south_moves.apply([](uint8_t _) -> uint8_t { return 0; });
      num_steps++;

      for(auto h = 0; h < _height; ++h)
      {
        for(auto w = 0; w < _width; ++w)
        {
          east_moves[h * _width + w] = ShouldMove(Cucumber::East, w, h);
        }
      }
      ApplyMoves(east_moves);
      for(auto h = 0; h < _height; ++h)
      {
        for(auto w = 0; w < _width; ++w)
        {
          south_moves[h * _width + w] = ShouldMove(Cucumber::South, w, h);
        }
      }
      ApplyMoves(south_moves);
    } while(east_moves.sum() + south_moves.sum() > 0);

    return num_steps;
  }

  void ApplyMoves(std::valarray<uint8_t> const& moves)
  {
    auto prev = _cucumbers;
    for(auto h = 0; h < _height; ++h)
    {
      for(auto w = 0; w < _width; ++w)
      {
        auto idx = h * _width + w;
        if(!moves[idx]) continue;
        auto to_move = prev[idx];
        at(w, h) = Cucumber::Nonexistant;
        if(to_move == Cucumber::East)
        {
          at(w + 1, h) = to_move;
        }
        else
        {
          at(w, h + 1) = to_move;
        }
      }
    }
  }

  Cucumber& at(size_t width, size_t height)
  {
    return _cucumbers[(width % _width) + _width * (height % _height)];
  }

  Cucumber const& at(size_t width, size_t height) const
  {
    return _cucumbers[(width % _width) + _width * (height % _height)];
  }

  uint8_t ShouldMove(Cucumber direction, size_t width, size_t height) const
  {
    if(at(width, height) != direction) return false;

    Cucumber neighbor;
    if(direction == Cucumber::East)
    {
      neighbor = at(width + 1, height);
    }
    else if(direction == Cucumber::South)
    {
      neighbor = at(width, height + 1);
    }

    return neighbor == Cucumber::Nonexistant;
  }

 private:
  size_t _width;
  size_t _height;
  std::valarray<Cucumber> _cucumbers;
};

inline FastField parse_fast_cucumbers()
{
  auto input = open_input("./inputs/25-1.txt");
  std::vector<Cucumber> cucumbers;
  std::string line;
  size_t width = 0, height = 0;
  while(std::getline(input, line))
  {
    ++height;
    width = line.size();
    for(auto c : line)
    {
      Cucumber cuc;
      switch(c)
      {
        case '.':
          cuc = Cucumber::Nonexistant;
          break;
        case '>':
          cuc = Cucumber::East;
          break;
        case 'v':
          cuc = Cucumber::South;
          break;
      }
      cucumbers.push_back(cuc);
    }
  }

  FastField field(width, height);
  field.Populate(cucumbers);
  return field;
}

inline CucumberField parse_cucumbers()
{
  auto input = open_input("./inputs/25-1.txt");
  std::vector<Cucumber> cucumbers;
  std::string line;
  size_t width = 0, height = 0;
  while(std::getline(input, line))
  {
    ++height;
    width = line.size();
    for(auto c : line)
    {
      Cucumber cuc;
      switch(c)
      {
        case '.':
          cuc = Cucumber::Nonexistant;
          break;
        case '>':
          cuc = Cucumber::East;
          break;
        case 'v':
          cuc = Cucumber::South;
          break;
      }
      cucumbers.push_back(cuc);
    }
  }

  CucumberField field(width, height);
  field.Populate(cucumbers);
  return field;
}
}  // namespace aoc
