#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace aoc
{
constexpr size_t DiagnosticArity = 12;

using Diagnostic = std::bitset<DiagnosticArity>;
using Diagnostics = std::vector<Diagnostic>;

struct InterpretLifeSupport
{
  InterpretLifeSupport(Diagnostics diags,
                       std::function<bool(size_t, size_t)> const& criteria)
    : _diags(diags), _criteria(criteria)
  {
  }

  uint32_t operator()(size_t bit)
  {
    if(_diags.size() == 1) return _diags.begin()->to_ulong();

    size_t total = 0;
    for(auto const& diag : _diags)
    {
      total += diag[bit];
    }
    auto criteria = _criteria(total, _diags.size());
    _diags.erase(std::remove_if(_diags.begin(), _diags.end(),
                                [bit, criteria](auto d) { return d[bit] != criteria; }),
                 _diags.end());
    return (*this)(bit - 1);
  }

 private:
  Diagnostics _diags;
  std::function<bool(size_t, size_t)> _criteria;
};

class DiagnosticInterpreter
{
 public:
  explicit DiagnosticInterpreter(Diagnostics diags) : _diags(std::move(diags))
  {
    read_power_consumption();
    read_life_support();
  }

  uint32_t power_consumption() const { return _gammaRate * _epsilonRate; }

  uint32_t life_support_rating() const { return _oxygenRating * _scrubberRating; }

 private:
  void read_power_consumption()
  {
    std::array<size_t, DiagnosticArity> bits{};
    for(auto const& diag : _diags)
    {
      for(size_t i = 0; i < DiagnosticArity; ++i)
      {
        bits[i] += diag[i];
      }
    }

    auto numDiags = _diags.size();
    auto minBits = numDiags * 0.5;
    std::bitset<DiagnosticArity> gamma;
    for(size_t i = 0; i < DiagnosticArity; ++i)
    {
      gamma[i] = bits[i] >= minBits ? 1 : 0;
    }

    auto epsilon = ~gamma;
    _gammaRate = gamma.to_ulong();
    _epsilonRate = epsilon.to_ulong();
  }

  void read_life_support()
  {
    _oxygenRating = InterpretLifeSupport(_diags, [](auto bitTotal, auto numDiags) {
      return bitTotal >= numDiags * 0.5 ? 1 : 0;
    })(DiagnosticArity - 1);
    _scrubberRating = InterpretLifeSupport(_diags, [](auto bitTotal, auto numDiags) {
      return bitTotal < numDiags * 0.5 ? 1 : 0;
    })(DiagnosticArity - 1);
  }

 private:
  Diagnostics _diags;
  uint32_t _gammaRate;
  uint32_t _epsilonRate;
  uint32_t _oxygenRating;
  uint32_t _scrubberRating;
};

inline Diagnostics parse_diagnostics()
{
  std::ifstream input("./inputs/3-1.txt");
  if(!input.is_open()) throw std::out_of_range("Failed to open input file");

  Diagnostics diags;
  std::string line;
  while(std::getline(input, line))
  {
    std::istringstream iss(line);
    iss >> diags.emplace_back();
  }

  return diags;
}
}  // namespace aoc
