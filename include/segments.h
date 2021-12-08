#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <functional>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

#include "parse.h"

namespace aoc
{
struct Segment
{
  Segment() = default;
  Segment(std::bitset<8> bs) : _signals(bs) {}

  void enable_signals(std::string const& signals)
  {
    for(auto c : signals)
    {
      size_t bit = c - 'a';
      _signals.set(bit);
    }
  }

  size_t num_signals() const { return _signals.count(); }
  auto to_ulong() const { return _signals.to_ulong(); }

  Segment sxor(Segment const& other) const { return Segment(_signals ^ other._signals); }
  Segment sor(Segment const& other) const { return Segment(_signals | other._signals); }

 private:
  // a - g, bit set if enabled
  std::bitset<8> _signals;
};

inline bool operator<(Segment const& fst, Segment const& snd)
{
  return fst.to_ulong() < snd.to_ulong();
}

static const std::set<size_t> UniqueSegments{2, 3, 4, 7};

struct Note
{
  void add_input(std::string input)
  {
    auto& s = _inputs.emplace_back();
    s.enable_signals(input);
  }

  void add_output(std::string output)
  {
    auto& s = _outputs.emplace_back();
    s.enable_signals(output);
  }

  size_t unique_output_segments() const
  {
    return std::count_if(_outputs.begin(), _outputs.end(), [](auto const& o) {
      auto res = UniqueSegments.contains(o.num_signals());
      return res;
    });
  }

  uint32_t output_value()
  {
    deduce_segment_values();
    auto exp = std::pow(10, _outputs.size() - 1);
    uint32_t output = 0;
    for(auto const& o : _outputs)
    {
      output += _segment_values.at(o) * exp;
      exp /= 10;
    }

    return output;
  }

 private:
  void deduce_segment_values()
  {
    auto const& one =
        deduce_single(1, [](auto const& s) { return s.num_signals() == 2; });
    auto const& four =
        deduce_single(4, [](auto const& s) { return s.num_signals() == 4; });
    auto const& seven =
        deduce_single(7, [](auto const& s) { return s.num_signals() == 3; });
    auto const& eight =
        deduce_single(8, [](auto const& s) { return s.num_signals() == 7; });
    auto const& nine = deduce_single(
        9, [&four](auto const& s) { return four.sxor(s).num_signals() == 2; });
    auto const& three = deduce_single(
        3, [&one](auto const& s) { return one.sxor(s).num_signals() == 3; });
    auto const& zero = deduce_single(
        0, [&one, &nine](auto const& s) { return one.sxor(s).num_signals() == 4; });
    auto const& six =
        deduce_single(6, [](auto const& s) { return s.num_signals() == 6; });
    auto const& five = deduce_single(
        5, [&nine](auto const& s) { return nine.sxor(s).num_signals() == 1; });
    auto const& two = deduce_single(2, [this](auto const& s) { return true; });
  }

  Segment const& deduce_single(uint32_t value,
                               std::function<bool(Segment const&)> const& pred)
  {
    auto realpred = [&pred, this](auto const& s) {
      return _segment_values.count(s) == 0 && pred(s);
    };
    auto it = std::find_if(_inputs.begin(), _inputs.end(), realpred);
    _segment_values[*it] = value;
    return *it;
  }

 private:
  std::vector<Segment> _inputs;
  std::vector<Segment> _outputs;
  std::map<Segment, uint32_t> _segment_values;
};

struct Notes
{
  Note& add_note() { return _notes.emplace_back(); }

  size_t unique_output_segments() const
  {
    return std::accumulate(_notes.begin(), _notes.end(), 0, [](auto res, auto const& n) {
      return res + n.unique_output_segments();
    });
  }

  uint32_t total()
  {
    return std::accumulate(_notes.begin(), _notes.end(), 0,
                           [](auto t, auto& note) { return t + note.output_value(); });
  }

  void print()
  {
    for(auto& note : _notes)
    {
      std::cout << note.output_value() << std::endl;
    }
  }

 private:
  std::vector<Note> _notes;
};

inline Notes parse_segment_notes()
{
  auto input = open_input("./inputs/8-1.txt");
  std::string line;
  Notes notes;
  std::string entry;
  while(std::getline(input, line))
  {
    auto& s = notes.add_note();
    std::istringstream iss(line);
    for(auto i = 0; i < 10; ++i)
    {
      std::getline(iss, entry, ' ');
      s.add_input(entry);
    }
    iss.ignore(2, ' ');
    for(auto i = 0; i < 4; ++i)
    {
      std::getline(iss, entry, ' ');
      s.add_output(entry);
    }
  }

  return notes;
}
}  // namespace aoc
