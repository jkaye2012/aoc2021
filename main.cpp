#include <chrono>
#include <iostream>
#include <ostream>

#include "include/pathing.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_cave_system();
  auto t1 = std::chrono::high_resolution_clock::now();
  auto result = v.unique_paths().size();
  auto t2 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  std::cout << "Result: " << result << " in " << duration.count() << " microseconds"
            << std::endl;
}
