#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "include/chiton.h"
#include "include/util.h"

int main(int argc, char** argv)
{
  auto v = aoc::parse_chiton();
  v.replicate(5);
  auto t1 = std::chrono::high_resolution_clock::now();
  auto result = v.shortest_path();
  auto t2 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  std::cout << "Result: " << result << " in " << duration.count() << " microseconds"
            << std::endl;
  // using TupleType = std::tuple<size_t, std::string, char>;
  // std::unordered_map<TupleType, size_t, aoc::tuple_hash> example;
  // example[{1, "two", '3'}] = 4;
  // example[{0, "one", '2'}] = 0;
  // example[{2, "three", '4'}] = 5;

  // for(auto const& [tup, val] : example)
  // {
  //   std::cout << "[" << std::get<0>(tup) << ", " << std::get<1>(tup) << ", "
  //             << std::get<2>(tup) << "]: " << val << std::endl;
  // }
  // example[{0, "one", '2'}] += 3;
  // for(auto const& [tup, val] : example)
  // {
  //   std::cout << "[" << std::get<0>(tup) << ", " << std::get<1>(tup) << ", "
  //             << std::get<2>(tup) << "]: " << val << std::endl;
  // }
}
