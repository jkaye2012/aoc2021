#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <valarray>
#include <vector>

#include "parse.h"
#include "util.h"

namespace aoc
{
static constexpr std::array<std::pair<int8_t, int8_t>, 9> neighbor_offsets()
{
  std::array<std::pair<int8_t, int8_t>, 9> offsets;
  auto n = 0;
  for(auto y = -1; y <= 1; ++y)
  {
    for(auto x = -1; x <= 1; ++x)
    {
      offsets[n++] = {x, y};
    }
  }
  return offsets;
}

static constexpr std::array<std::pair<int32_t, int32_t>, 9> neighbors_of(int32_t x,
                                                                         int32_t y)
{
  std::array<std::pair<int32_t, int32_t>, 9> neighbors;
  auto n = 0;
  for(auto const& offset : neighbor_offsets())
  {
    neighbors[n++] = {x + offset.first, y + offset.second};
  }
  return neighbors;
}

struct InfiniteSpace
{
  InfiniteSpace() = default;
  InfiniteSpace(bool identity) : _identity(identity) {}

  bool Identity() const { return _identity; }

 private:
  bool _identity = false;
};

struct Image
{
  Image(size_t width, size_t height, bool identity)
    : _width(width), _height(height), _identity(identity), _image(_width * _height)
  {
  }

  Image(size_t width, size_t height, std::valarray<bool> image)
    : _width(width), _height(height), _identity(false), _image(std::move(image))
  {
  }

  bool GetPixel(int32_t x, int32_t y) const
  {
    if(x < 0 || y < 0 || x >= _width || y >= _height)
    {
      return _identity;
    }

    return At(x, y);
  }

  size_t LitPixels() const
  {
    size_t count = 0;
    for(auto b : _image)
    {
      if(b) ++count;
    }
    return count;
  }

  Image Translate(std::valarray<bool> const& enhancement_algo) const
  {
    Image translated(_width + 2, _height + 2, NextIdentity(enhancement_algo));
    std::bitset<9> window;
    for(auto y = -1; y <= static_cast<int32_t>(_height); ++y)
    {
      for(auto x = -1; x <= static_cast<int32_t>(_width); ++x)
      {
        auto n = 0;
        for(auto const& neighbor : neighbors_of(x, y))
        {
          window.set(8 - n++, GetPixel(neighbor.first, neighbor.second));
        }
        translated.At(x + 1, y + 1) = enhancement_algo[window.to_ulong()];
      }
    }
    return translated;
  }

  void Print() const
  {
    for(auto y = 0; y < _height; ++y)
    {
      for(auto x = 0; x < _width; ++x)
      {
        std::cout << (At(x, y) ? '#' : '.');
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

 private:
  bool& At(int32_t width, int32_t height) { return _image[height * _width + width]; }
  bool At(int32_t width, int32_t height) const { return _image[height * _width + width]; }

  bool NextIdentity(std::valarray<bool> const& enhancement_algo) const
  {
    return _identity ? enhancement_algo[511] : enhancement_algo[0];
  }

  size_t _width;
  size_t _height;
  bool _identity = false;
  std::valarray<bool> _image;
};

struct ImageProcessor
{
  ImageProcessor(Image image, std::valarray<bool> enhancement_algo)
    : _latest_image(std::move(image)), _enhancement_algo(std::move(enhancement_algo))
  {
  }

  Image const& Enhance(size_t times)
  {
    for(auto n = 0; n < times; ++n)
    {
      _latest_image = _latest_image.Translate(_enhancement_algo);
    }

    return _latest_image;
  }

  void PrintAlgo() const
  {
    for(auto e : _enhancement_algo)
    {
      std::cout << (e ? '#' : '.');
    }
    std::cout << std::endl << std::endl;
  }

 private:
  Image _latest_image;
  std::valarray<bool> _enhancement_algo;
};

inline ImageProcessor parse_image()
{
  auto input = open_input("./inputs/20-1.txt");
  size_t width = 0, height = 0;
  std::string line;
  if(!std::getline(input, line))
    throw std::out_of_range("Failed to parse enhancement algo");
  std::valarray<bool> enhancement_algo(line.size());
  for(auto n = 0; n < line.size(); ++n)
  {
    enhancement_algo[n] = line[n] == '#' ? true : false;
  }

  std::vector<bool> pixels;
  while(std::getline(input, line))
  {
    if(line.empty()) continue;
    height++;
    if(width == 0) width = line.size();
    for(auto c : line)
    {
      pixels.push_back(c == '#' ? true : false);
    }
  }
  std::valarray<bool> initial_image(width * height);
  for(auto n = 0; n < pixels.size(); ++n)
  {
    initial_image[n] = pixels[n];
  }

  return ImageProcessor(Image(width, height, std::move(initial_image)),
                        std::move(enhancement_algo));
}
}  // namespace aoc
