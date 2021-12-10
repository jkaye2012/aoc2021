#pragma once

#include <algorithm>
#include <array>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <variant>
#include <vector>

#include "parse.h"

namespace aoc
{
using Token = char;
static const std::map<Token, Token> CloseTokens{
    {')', '('}, {']', '['}, {'}', '{'}, {'>', '<'}};
static const std::map<Token, size_t> InvalidScores{
    {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};
static const std::map<Token, size_t> AutocompleteScores{
    {'(', 1}, {'[', 2}, {'{', 3}, {'<', 4}};

struct Chunk
{
  Chunk() = default;
  explicit Chunk(Token tok) : _tok(tok) {}

  void add_chunk(Chunk c) { _chunks.emplace_back(std::move(c)); }
  bool terminated_by(Token tok) const { return tok == _tok; }

  Token _tok;

 private:
  std::list<Chunk> _chunks;
};

struct Incomplete
{
  std::stack<Chunk> missing;
};

struct Invalid
{
  Token last;
};

using ParseResult = std::variant<Chunk, Incomplete, Invalid>;
using Line = std::list<ParseResult>;

static inline ParseResult build_chunk(std::string::iterator& it,
                                      std::string::iterator const& end)
{
  std::stack<Chunk> toks;
  toks.emplace(*it++);
  Chunk c;
  for(; it != end && !toks.empty(); ++it)
  {
    auto t = *it;
    if(CloseTokens.contains(t))
    {
      c = toks.top();
      toks.pop();
      if(!c.terminated_by(CloseTokens.at(t))) return {Invalid{t}};
      if(!toks.empty()) toks.top().add_chunk(std::move(c));
    }
    else
    {
      toks.emplace(t);
    }
  }
  if(!toks.empty()) return {Incomplete{std::move(toks)}};
  return {c};
}

struct Lines
{
  Line& add_line() { return _lines.emplace_back(); }

  size_t corrupted_score() const
  {
    size_t res = 0;
    for(auto const& line : _lines)
    {
      auto corrupted = std::find_if(line.begin(), line.end(), [](auto const& pr) {
        return std::holds_alternative<Invalid>(pr);
      });
      if(corrupted != line.end())
        res += InvalidScores.at(std::get<Invalid>(*corrupted).last);
    }
    return res;
  }

  size_t autocomplete_score() const
  {
    std::vector<size_t> scores;
    for(auto const& line : _lines)
    {
      auto corrupted = std::find_if(line.begin(), line.end(), [](auto const& pr) {
        return std::holds_alternative<Invalid>(pr);
      });
      if(corrupted != line.end()) continue;

      size_t res = 0;
      auto incomplete = std::find_if(line.begin(), line.end(), [](auto const& pr) {
        return std::holds_alternative<Incomplete>(pr);
      });
      while(incomplete != line.end())
      {
        auto toks = std::get<Incomplete>(*incomplete++).missing;
        while(!toks.empty())
        {
          auto tok = toks.top();
          auto score = AutocompleteScores.at(tok._tok);
          res *= 5;
          res += score;
          toks.pop();
        }
      }

      scores.push_back(res);
    }
    std::sort(scores.begin(), scores.end());
    return scores[scores.size() / 2];
  }

 private:
  std::list<Line> _lines;
};

static inline Lines parse_navigation()
{
  auto input = open_input("./inputs/10-1.txt");
  std::string line;
  Lines lines;
  while(std::getline(input, line))
  {
    auto& l = lines.add_line();
    auto it = line.begin();
    while(it != line.end())
    {
      l.push_back(build_chunk(it, line.end()));
    }
  }

  return lines;
}
}  // namespace aoc
