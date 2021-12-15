#include <chrono>
#include <iostream>
#include <ostream>

#include "include/polymers.h"
#include "include/util.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_polymer();
  auto t1 = std::chrono::high_resolution_clock::now();
  auto p = v.score(40);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << std::endl;
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  std::cout << "Result: " << p << " in " << duration.count() << " microseconds"
            << std::endl;
}
