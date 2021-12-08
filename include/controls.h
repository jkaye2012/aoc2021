#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "parse.h"

namespace aoc
{
enum class Direction
{
  Forward = 1,
  Up = 2,
  Down = 3
};

inline std::istream& operator>>(std::istream& is, Direction& dir)
{
  std::string rawDir;
  if(!std::getline(is, rawDir, ' ')) throw "Failed to read raw direction";

  if(rawDir == "forward")
    dir = Direction::Forward;
  else if(rawDir == "up")
    dir = Direction::Up;
  else if(rawDir == "down")
    dir = Direction::Down;
  else
    throw "Failed to parse direction";

  is >> std::ws;
  return is;
}

struct Command
{
  Direction direction;
  uint32_t magnitude;
};

inline std::istream& operator>>(std::istream& is, Command& cmd)
{
  is >> cmd.direction;
  is >> cmd.magnitude;
  is >> std::ws;
  return is;
}

struct Coordinates
{
  int32_t x = 0;
  int32_t y = 0;
  int32_t aim = 0;

  void process_command(Command const& cmd)
  {
    switch(cmd.direction)
    {
      case Direction::Forward:
        x += cmd.magnitude;
        y += aim * cmd.magnitude;
        break;
      case Direction::Up:
        aim -= cmd.magnitude;
        break;
      case Direction::Down:
        aim += cmd.magnitude;
        break;
    }
  }
};

using Commands = std::vector<Command>;

inline Commands parse_commands()
{
  std::ifstream input = open_input("./inputs/2-1.txt");

  Commands commands;
  std::string line;
  while(std::getline(input, line))
  {
    std::istringstream iss(line);
    iss >> commands.emplace_back();
  }

  return commands;
}

inline int run_commands()
{
  auto cmds = parse_commands();
  Coordinates coords;

  for(auto const& cmd : cmds)
  {
    coords.process_command(cmd);
  }

  return coords.x * coords.y;
}
}  // namespace aoc
