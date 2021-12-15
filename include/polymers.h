#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <valarray>

#include "parse.h"
#include "util.h"

namespace aoc
{
using PolymerTemplate = std::string;
using InsertionRules = std::unordered_map<std::pair<char, char>, char, pair_hash>;
using Step = std::unordered_map<std::pair<char, char>, size_t, pair_hash>;

struct PolymerFormula
{
  PolymerTemplate Template;
  InsertionRules Rules;

  Step run_steps(size_t steps)
  {
    Step step;
    for(auto it = Template.begin(); it != Template.end() - 1; ++it)
    {
      step[{*it, *(it + 1)}]++;
    }
    for(auto i = 0; i < steps; ++i)
    {
      run_step(step);
    }

    return step;
  }

  size_t score(size_t steps)
  {
    auto step = run_steps(steps);
    std::unordered_map<char, size_t> occurs;
    for(auto const& [pair, count] : step)
    {
      occurs[pair.first] += count;
    }
    occurs[*(Template.end() - 1)]++;
    auto max = std::max_element(
        occurs.begin(), occurs.end(),
        [](auto const& s, auto const& f) { return s.second < f.second; });
    auto min = std::min_element(
        occurs.begin(), occurs.end(),
        [](auto const& s, auto const& f) { return s.second < f.second; });
    ;
    return max->second - min->second;
  }

 private:
  void run_step(Step& step)
  {
    _temp.clear();
    std::swap(_temp, step);
    for(auto const& [p, count] : _temp)
    {
      auto inserted = Rules.at(p);
      step[{p.first, inserted}] += count;
      step[{inserted, p.second}] += count;
    }
  }

  Step _temp;
};

inline PolymerFormula parse_polymer()
{
  auto input = open_input("./inputs/14-1.txt");
  PolymerFormula formula;
  std::string line;
  if(!std::getline(input, formula.Template))
    throw std::out_of_range("Failed to parse template");
  input.ignore(1, '\n');

  std::array<char, 2> key;
  while(std::getline(input, line))
  {
    std::pair<char, char> key;
    char value;
    std::istringstream iss(line);
    iss.get(key.first);
    iss.get(key.second);
    iss.ignore(4);
    iss.get(value);
    formula.Rules.try_emplace(std::move(key), value);
  }

  return formula;
}

}  // namespace aoc
