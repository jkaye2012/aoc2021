#include <chrono>
#include <iostream>
#include <ostream>

#include "include/dumbo_octo.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_dumbo();
  v.print();
  auto t1 = std::chrono::high_resolution_clock::now();
  auto result = v.run_until_convergence();
  auto t2 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  std::cout << "Result: " << result << " in " << duration.count() << " microseconds"
            << std::endl;
}
