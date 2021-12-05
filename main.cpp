#include <iostream>
#include <ostream>

#include "include/diagnostics.h"

int main(int argc, char** argv)
{
  auto v = aoc::DiagnosticInterpreter(aoc::parse_diagnostics());
  std::cout << v.life_support_rating() << std::endl;
}
