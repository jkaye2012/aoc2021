#include <iostream>
#include <ostream>

#include "include/vents.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_plane();
  std::cout << v.count_where([](auto s) { return s > 1; }) << std::endl;
}
