#include <chrono>
#include <iostream>
#include <ostream>

#include "include/transparent.h"
#include "include/util.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_manual();
  auto t1 = std::chrono::high_resolution_clock::now();
  auto p = v.apply_folds();
  auto t2 = std::chrono::high_resolution_clock::now();
  p.print();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  std::cout << "Result: " << p.num_marks() << " in " << duration.count()
            << " microseconds" << std::endl;
}
