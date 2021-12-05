#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <span>
#include <system_error>
#include <vector>

#include "util.h"

namespace aoc
{
using SonarReadings = std::vector<uint32_t>;

inline SonarReadings parse_sonar_readings()
{
  std::ifstream input("./inputs/1-1.txt", std::ios::binary);
  if(!input.is_open())
  {
    throw std::system_error(std::make_error_code(std::errc::io_error),
                            "Failed to open input file");
  }
  std::istream_iterator<uint32_t> it(input);
  SonarReadings result(it, std::istream_iterator<uint32_t>());
  return result;
}

class Sonar
{
 public:
  explicit Sonar(SonarReadings readings) : _readings(std::move(readings)) {}

  uint32_t depth_increases() const
  {
    if(_readings.size() <= 1) return 0;
    auto result = 0;
    auto fst = _readings.begin();
    for(auto snd = std::next(fst); snd != _readings.end(); ++fst, ++snd)
    {
      if(*fst < *snd) ++result;
    }

    return result;
  }

  uint32_t depth_increases(size_t windowSize) const
  {
    auto result = 0;
    auto view = std::span(_readings);
    for(size_t i = 0; i < _readings.size() - windowSize; ++i)
    {
      auto fst = aoc::sum(view.subspan(i, windowSize));
      auto snd = aoc::sum(view.subspan(i + 1, windowSize));
      if(fst < snd) result++;
    }
    return result;
  }

 private:
  SonarReadings _readings;
};

inline int run_sonar()
{
  Sonar sonar(parse_sonar_readings());
  return sonar.depth_increases(3);
}
}  // namespace aoc
