#include <iostream>
#include <ostream>

#include "include/bingo.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_bingo();
  std::cout << v.PlayGame() << std::endl;
}
