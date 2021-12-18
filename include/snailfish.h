#pragma once

#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stack>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

#include "parse.h"

namespace aoc
{
struct Reduction
{
  struct Reduced
  {
  };

  struct Split
  {
  };

  struct Exploded
  {
    std::optional<size_t> Left;
    std::optional<size_t> Right;
    size_t Depth;
  };

  bool IsReduced() const { return Result.index() == 0; }
  bool IsSplit() const { return Result.index() == 1; }
  bool IsExplosion() const { return Result.index() == 0; }

  static Reduction DidReduce() { return Reduction{Reduced()}; }
  static Reduction DidSplit() { return Reduction{Split()}; }
  static Reduction DidExplode(size_t left, size_t right, size_t depth)
  {
    return Reduction{Exploded{{left}, {right}, depth}};
  }

  void print() const
  {
    std::string action;
    switch(Result.index())
    {
      case 0:
        action = "reduce";
        break;
      case 1:
        action = "split";
        break;
      case 2:
        action = "explode";
        break;
    }
    std::cout << "After " << action << ": ";
  }

  std::variant<Reduced, Split, Exploded> Result;
};

struct Number
{
  using Child = std::unique_ptr<std::variant<size_t, Number>>;

  static Child MakeChild(std::variant<size_t, Number>&& child)
  {
    return std::make_unique<std::variant<size_t, Number>>(std::move(child));
  }

  Number() : _depth(0) {}
  explicit Number(size_t depth) : _depth(depth) {}
  Number(size_t left, size_t right, size_t depth)
    : _left(MakeChild(left)), _right(MakeChild(right)), _depth(depth)
  {
  }
  Number(Number const& other)
    : _left(std::make_unique<std::variant<size_t, Number>>(*other._left)),
      _right(std::make_unique<std::variant<size_t, Number>>(*other._right)),
      _depth(other._depth)
  {
  }
  Number(Number&&) = default;
  Number& operator=(Number&&) = default;

  void IncrementDepth()
  {
    _depth++;
    if(auto left = std::get_if<Number>(&*_left))
    {
      left->IncrementDepth();
    }
    if(auto right = std::get_if<Number>(&*_right))
    {
      right->IncrementDepth();
    }
  }

  void Add(Number&& l, Number&& r)
  {
    l.IncrementDepth();
    r.IncrementDepth();
    _left = MakeChild(std::move(l));
    _right = MakeChild(std::move(r));
  }

  void print() const
  {
    std::cout << "[";
    if(auto n = std::get_if<size_t>(&*_left))
      std::cout << *n;
    else
      std::get<Number>(*_left).print();
    std::cout << ",";
    if(auto n = std::get_if<size_t>(&*_right))
      std::cout << *n;
    else
      std::get<Number>(*_right).print();
    std::cout << "]";
    if(IsRoot()) std::cout << std::endl;
  }

  void SetNumber(Number&& num)
  {
    if(!_left)
      _left = MakeChild(std::move(num));
    else if(!_right)
      _right = MakeChild(std::move(num));
    else
      throw std::out_of_range("Tried to add too many children");
  }

  void SetValue(size_t num)
  {
    if(!_left)
      _left = MakeChild(num);
    else if(!_right)
      _right = MakeChild(num);
    else
      throw std::out_of_range("Tried to add too many children");
  }

  void ReduceFully()
  {
    Reduction reduction;
    do
    {
      reduction = Reduce();
      // reduction.print();
      // print();
    } while(!reduction.IsReduced());
  }

  Reduction Reduce()
  {
    Reduction reduction;
    reduction = ReduceExplosion();
    if(reduction.IsReduced())
    {
      reduction = ReduceSplit();
    }

    return reduction;
  }

  Reduction ReduceSplit()
  {
    auto left_result = ReduceChildSplit(_left);
    if(!left_result.IsReduced()) return left_result;
    return ReduceChildSplit(_right);
  }

  Reduction ReduceExplosion()
  {
    auto left_result = ReduceChildExplosion(_left);
    if(auto* explosion = std::get_if<Reduction::Exploded>(&left_result.Result))
    {
      if(explosion->Right.has_value())
      {
        if(explosion->Depth == _depth + 1)
        {
          _left->emplace<size_t>(0);
        }
        RightExplosion(explosion->Right.value());
        explosion->Right.reset();
      }
      return left_result;
    }

    auto right_result = ReduceChildExplosion(_right);
    if(auto* explosion = std::get_if<Reduction::Exploded>(&right_result.Result))
    {
      if(explosion->Left.has_value())
      {
        if(explosion->Depth == _depth + 1)
        {
          _right->emplace<size_t>(0);
        }
        LeftExplosion(explosion->Left.value());
        explosion->Left.reset();
      }
      return right_result;
    }

    // Fully reduced
    return right_result;
  }

  bool IsRoot() const { return _depth == 0; }

  size_t Magnitude() const
  {
    return 3 * ChildMagnitude(_left) + 2 * ChildMagnitude(_right);
  }

  size_t ChildMagnitude(Child const& child) const
  {
    if(auto val = std::get_if<size_t>(&*child))
    {
      return *val;
    }

    return std::get<Number>(*child).Magnitude();
  }

 private:
  std::optional<Reduction> Explosion() const
  {
    if(_depth < 4) return std::nullopt;
    return Reduction::DidExplode(std::get<size_t>(*_left), std::get<size_t>(*_right),
                                 _depth);
  }

  void RightExplosion(size_t num)
  {
    if(auto right_num = std::get_if<size_t>(&*_right))
    {
      *right_num += num;
    }
    else
    {
      auto& right = std::get<Number>(*_right);
      right.AddLeft(num);
    }
  }

  void AddLeft(size_t num)
  {
    if(auto left = std::get_if<size_t>(&*_left))
    {
      *left += num;
    }
    else
    {
      std::get<Number>(*_left).AddLeft(num);
    }
  }

  void LeftExplosion(size_t num)
  {
    if(auto left_num = std::get_if<size_t>(&*_left))
    {
      *left_num += num;
    }
    else
    {
      auto& left = std::get<Number>(*_left);
      left.AddRight(num);
    }
  }

  void AddRight(size_t num)
  {
    if(auto right = std::get_if<size_t>(&*_right))
    {
      *right += num;
    }
    else
    {
      std::get<Number>(*_right).AddRight(num);
    }
  }

  std::optional<Reduction> Split()
  {
    auto* left_num = std::get_if<size_t>(&*_left);
    if(left_num && *left_num > 9)
    {
      size_t split_left = *left_num / 2;
      size_t split_right = std::round(*left_num / 2.0);
      _left->emplace<Number>(split_left, split_right, _depth + 1);
      return Reduction::DidSplit();
    }

    auto* right_num = std::get_if<size_t>(&*_right);
    if(right_num && *right_num > 9)
    {
      size_t split_left = *right_num / 2;
      size_t split_right = std::round(*right_num / 2.0);
      _right->emplace<Number>(split_left, split_right, _depth + 1);
      return Reduction::DidSplit();
    }

    return std::nullopt;
  }

  Reduction ReduceChildExplosion(Child& child)
  {
    if(auto* num = std::get_if<Number>(&*child); num)
    {
      auto reduction = num->ReduceExplosion();
      if(!reduction.IsReduced()) return reduction;
      if(auto explosion = num->Explosion()) return *explosion;
    }

    return Reduction::DidReduce();
  }

  Reduction ReduceChildSplit(Child& child)
  {
    if(auto* num = std::get_if<Number>(&*child); num)
    {
      auto reduction = num->ReduceSplit();
      if(!reduction.IsReduced()) return reduction;
      if(auto split = num->Split()) return *split;
    }
    else
    {
      auto val = std::get<size_t>(*child);
      if(val > 9)
      {
        size_t split_left = val / 2;
        size_t split_right = std::round(val / 2.0);
        child->emplace<Number>(split_left, split_right, _depth + 1);
        return Reduction::DidSplit();
      }
    }

    return Reduction::DidReduce();
  }

 private:
  size_t _depth = 0;
  Child _left;
  Child _right;
};

inline Number parse_single_number(std::string_view num_str)
{
  Number number;
  std::stack<Number> nums;
  size_t depth = 0;
  for(auto c : num_str)
  {
    switch(c)
    {
      case '[':
      {
        nums.emplace(depth++);
        break;
      }
      case ',':
      {
        break;
      }
      case ']':
      {
        --depth;
        number = std::move(nums.top());
        nums.pop();
        if(!nums.empty())
        {
          auto& curr = nums.top();
          curr.SetNumber(std::move(number));
        }
        break;
      }
      default:
      {
        auto& curr = nums.top();
        auto num = c - '0';
        curr.SetValue(num);
      }
    }
  }
  return number;
}

inline std::vector<Number> parse_numbers()
{
  auto input = aoc::open_input("./inputs/18-1.txt");
  std::vector<Number> numbers;
  std::string line;
  while(std::getline(input, line))
  {
    numbers.emplace_back(parse_single_number(line));
  }
  return numbers;
}

inline std::pair<size_t, Number> magnitude(std::vector<Number> numbers)
{
  Number running_total = std::move(numbers.front());
  numbers.erase(numbers.begin());
  for(auto& number : numbers)
  {
    Number temp;
    temp.Add(std::move(running_total), std::move(number));
    temp.ReduceFully();
    running_total = std::move(temp);
  }

  running_total.ReduceFully();
  auto mag = running_total.Magnitude();
  return {mag, std::move(running_total)};
}

inline std::pair<size_t, Number> greatest_binary_magnitude(std::vector<Number> numbers)
{
  size_t mag = std::numeric_limits<size_t>::min();
  Number greatest;
  for(auto i = 0; i < numbers.size(); ++i)
  {
    for(auto j = 0; j < numbers.size(); ++j)
    {
      if(i == j) continue;
      Number one = numbers.at(i);
      Number two = numbers.at(j);
      Number added;
      added.Add(std::move(one), std::move(two));
      added.ReduceFully();
      auto m = added.Magnitude();
      if(m > mag)
      {
        mag = m;
        greatest = std::move(added);
      }
    }
  }
  return {mag, greatest};
}
}  // namespace aoc
