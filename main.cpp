#include <iostream>
#include <ostream>

#include "include/lanternfish.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_school();
  std::cout << v.pass_days(256) << std::endl;
}
