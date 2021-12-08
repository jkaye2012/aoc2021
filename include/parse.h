#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace aoc
{
inline std::ifstream open_input(std::string const& name)
{
  std::ifstream input(name);
  if(!input.is_open()) throw std::out_of_range("Failed to open input file");
  return input;
}
}  // namespace aoc
