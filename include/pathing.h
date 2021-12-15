#pragma once

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parse.h"

namespace aoc
{
enum class CaveType
{
  Small,
  Large
};

inline CaveType cave_type_from_name(std::string name)
{
  return std::isupper(name.at(0)) ? CaveType::Large : CaveType::Small;
}

struct Cave
{
  Cave(CaveType type, std::string name) : Type(type), Name(std::move(name)) {}

  void add_path(std::string const& other) { Adjacent.push_back(other); }

  CaveType Type;
  std::string Name;
  std::vector<std::string> Adjacent;
};

struct Path
{
  std::vector<std::string> Caves;
  std::unordered_set<std::string> Visited;
  bool SmallRevisited = false;
};

struct CaveSystem
{
  static constexpr auto Start = "start";
  static constexpr auto End = "end";

  Cave const& start() const { return _caves.at(Start); }
  Cave const& end() const { return _caves.at(End); }

  size_t num_caves() const { return _caves.size(); }

  void add_path(std::string const& start, std::string const& end)
  {
    auto& s = get_cave(start);
    auto& e = get_cave(end);
    s.add_path(e.Name);
    e.add_path(s.Name);
  }

  std::vector<Path> unique_paths() const { return sub_paths(start(), {}); }

 private:
  std::vector<Path> sub_paths(Cave const& cave, Path path) const
  {
    if(cave.Name == End)
    {
      path.Caves.emplace_back(End);
      return {std::move(path)};
    }
    if(path.Visited.contains(cave.Name))
    {
      if(path.SmallRevisited || cave.Name == Start)
        return {};
      else
        path.SmallRevisited = true;
    }
    if(cave.Type == CaveType::Small) path.Visited.insert(cave.Name);

    path.Caves.push_back(cave.Name);
    std::vector<Path> paths;
    for(auto adjacent : cave.Adjacent)
    {
      auto const& sp = sub_paths(_caves.at(adjacent), path);
      paths.insert(paths.end(), sp.begin(), sp.end());
    }
    return paths;
  }

  Cave& get_cave(std::string const& cave)
  {
    auto it = _caves.find(cave);
    if(it != _caves.end()) return it->second;

    auto [inserted, _] = _caves.emplace(cave, Cave{cave_type_from_name(cave), cave});
    return inserted->second;
  }

  std::unordered_map<std::string, Cave> _caves;
};

inline CaveSystem parse_cave_system()
{
  auto input = open_input("./inputs/12-1.txt");
  CaveSystem system;
  std::string line;
  while(std::getline(input, line))
  {
    std::string start, end;
    std::istringstream iss(line);
    if(!std::getline(iss, start, '-'))
      throw std::out_of_range("Failed to parse start of path");
    iss >> end;
    system.add_path(start, end);
  }

  return system;
}

}  // namespace aoc
