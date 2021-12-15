#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <valarray>
#include <vector>

#include "parse.h"

namespace aoc
{
enum class FoldDirection
{
  Horizontal,
  Vertical
};

struct Fold
{
  FoldDirection Direction;
  size_t Coord;
};

struct Paper
{
  Paper() = default;
  Paper(size_t width, size_t height)
    : Width(width), Height(height), Size(Width * Height), Marks(Size)
  {
  }
  Paper(size_t width, size_t height, std::vector<std::pair<size_t, size_t>> const& points)
    : Paper(width, height)
  {
    for(auto const& [x, y] : points)
    {
      set(x, y);
    }
  }
  Paper(size_t width, size_t height, Paper const& originating) : Paper(width, height)
  {
    for(auto y = 0; y < Height; ++y)
    {
      for(auto x = 0; x < Width; ++x)
      {
        if(originating.set(x, y)) set(x, y);
      }
    }
  }

  Paper fold(Fold const& fold) const
  {
    Paper folded;
    if(fold.Direction == FoldDirection::Horizontal)
    {
      auto new_width = fold.Coord;
      folded = Paper(new_width, Height, *this);
      for(auto y = 0; y < Height; ++y)
      {
        for(auto x = new_width; x < Width; ++x)
        {
          if(!set(x, y)) continue;
          folded.set(new_width - (x - new_width), y);
        }
      }
    }
    else
    {
      auto new_height = fold.Coord;
      folded = Paper(Width, new_height, *this);
      for(auto y = new_height; y < Height; ++y)
      {
        for(auto x = 0; x < Width; ++x)
        {
          if(!set(x, y)) continue;
          folded.set(x, new_height - (y - new_height));
        }
      }
    }
    return folded;
  }

  void print() const
  {
    auto w = 0;
    for(auto val : Marks)
    {
      std::cout << (val ? '1' : ' ');
      if(++w == Width)
      {
        w = 0;
        std::cout << std::endl;
      }
    }
    std::cout << std::endl;
  }

  void set(size_t x, size_t y) { Marks[y * Width + x] = true; }
  bool set(size_t x, size_t y) const { return Marks[y * Width + x]; }

  size_t num_marks() const
  {
    size_t num = 0;
    for(auto m : Marks)
      if(m) ++num;
    return num;
  }

  size_t Width;
  size_t Height;
  size_t Size;
  std::valarray<bool> Marks;
};

struct Manual
{
  Paper apply_folds() const { return apply_folds(Folds.size()); }

  Paper apply_folds(size_t num) const
  {
    size_t count = 0;
    auto p = this->Paper;
    for(auto const& fold : Folds)
    {
      if(num <= count++) break;
      p = p.fold(fold);
    }

    return p;
  }

  Paper Paper;
  std::vector<Fold> Folds;
};

inline Manual parse_manual()
{
  auto input = open_input("./inputs/13-1.txt");
  std::string line, tok;
  size_t x, y, fold;
  size_t width = 0, height = 0;
  std::vector<std::pair<size_t, size_t>> coords;
  while(std::getline(input, line))
  {
    if(line.empty()) break;
    std::istringstream iss(line);
    if(!std::getline(iss, tok, ','))
      throw std::out_of_range("Failed to parse x coordinate");
    x = std::atoi(tok.c_str());
    iss >> y;
    width = std::max(width, x);
    height = std::max(height, y);
    coords.emplace_back(x, y);
  }
  width++;
  height++;

  std::vector<Fold> folds;
  while(std::getline(input, line))
  {
    auto sub = line.substr(11);
    auto direction = sub.at(0);
    auto coord = sub.substr(2);
    fold = std::atoi(coord.c_str());
    folds.emplace_back(Fold{
        direction == 'x' ? FoldDirection::Horizontal : FoldDirection::Vertical, fold});
  }

  return Manual{Paper(width, height, coords), std::move(folds)};
}
}  // namespace aoc
