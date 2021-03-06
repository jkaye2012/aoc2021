#include <chrono>
#include <iostream>
#include <ostream>
#include <span>
#include <string>
#include <tuple>
#include <unordered_map>

#include "include/alu.h"
#include "include/alu_input.h"
#include "include/reactor.h"
#include "include/sea_cucumbers.h"
#include "include/util.h"

using namespace std::literals::string_view_literals;

int main(int argc, char** argv)
{
  auto f = aoc::parse_fast_cucumbers();
  auto t1 = std::chrono::high_resolution_clock::now();
  auto result = f.move();
  auto t2 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  std::cout << "Result: " << result << " in " << duration.count() << " microseconds "
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
