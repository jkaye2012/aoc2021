#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "alu_input.h"
#include "parse.h"

namespace aoc
{
template <size_t N>
struct Memory
{
  explicit Memory(std::array<int, N> inp) : input(std::move(inp)) {}

  int& var(char v)
  {
    switch(v)
    {
      case 'w':
        return w;
      case 'x':
        return x;
      case 'y':
        return y;
      case 'z':
        return z;
    }

    throw std::out_of_range("Invalid variable name");
  };

  static int lit(std::string_view sv) { return std::atoi(sv.data()); };

  int val(std::string_view sv)
  {
    auto c = sv.at(0);
    if('w' <= c && 'z' >= c)
    {
      return var(c);
    }

    return lit(sv);
  };

  int next_input() { return input[input_index++]; }

  int w = 0;
  int x = 0;
  int y = 0;
  int z = 0;
  std::array<int, N> input;
  size_t input_index = 0;
};

template <size_t N>
inline void process_instruction(Memory<N>& memory, std::string_view instr)

{
  auto op = instr.substr(0, 3);
  if(op == "inp")
  {
    memory.var(instr.at(4)) = memory.next_input();
  }
  else if(op == "add")
  {
    memory.var(instr.at(4)) += memory.val(instr.substr(6));
  }
  else if(op == "mul")
  {
    memory.var(instr.at(4)) *= memory.val(instr.substr(6));
  }
  else if(op == "div")
  {
    memory.var(instr.at(4)) /= memory.val(instr.substr(6));
  }
  else if(op == "mod")
  {
    memory.var(instr.at(4)) %= memory.val(instr.substr(6));
  }
  else if(op == "eql")
  {
    int& v = memory.var(instr.at(4));
    v = v == memory.val(instr.substr(6)) ? 1 : 0;
  }
}

enum class Variable
{
  W,
  X,
  Y,
  Z
};

constexpr Variable parse_var(char v)
{
  switch(v)
  {
    case 'w':
      return Variable::W;
    case 'x':
      return Variable::X;
    case 'y':
      return Variable::Y;
    default:
      return Variable::Z;
  }
}

using Value = std::variant<Variable, int>;

constexpr Value parse_val(std::string_view v)
{
  switch(v.at(0))
  {
    case 'w':
      return Variable::W;
    case 'x':
      return Variable::X;
    case 'y':
      return Variable::Y;
    case 'z':
      return Variable::Z;
  }
  auto invert = v.at(0) == '-' ? true : false;
  if(invert) v = v.substr(1);
  int val = 0;
  for(auto c : v)
  {
    auto i = c - '0';
    val *= 10;
    val += i;
  }
  return invert ? val * -1 : val;
}

struct StaticMemory
{
  constexpr int& Access(Variable var)
  {
    switch(var)
    {
      case Variable::W:
        return W;
      case Variable::X:
        return X;
      case Variable::Y:
        return Y;
      case Variable::Z:
        return Z;
    }
  }

  constexpr int Access(Value val)
  {
    if(auto* var = std::get_if<Variable>(&val))
    {
      return Access(*var);
    }
    return std::get<int>(val);
  }

  int W = 0;
  int X = 0;
  int Y = 0;
  int Z = 0;
};

constexpr bool input_is_valid(uint64_t num)
{
  for(auto i = 0; i < 14; ++i)
  {
    if(num % 10 == 0) return false;
    num /= 10;
  }
  return true;
}

enum class OpCode
{
  Inp,
  Add,
  Mul,
  Div,
  Mod,
  Eql,
  Die
};

constexpr OpCode parse_op(std::string_view op)
{
  if(op == "inp")
  {
    return OpCode::Inp;
  }
  else if(op == "add")
  {
    return OpCode::Add;
  }
  else if(op == "mul")
  {
    return OpCode::Mul;
  }
  else if(op == "div")
  {
    return OpCode::Div;
  }
  else if(op == "mod")
  {
    return OpCode::Mod;
  }
  else if(op == "eql")
  {
    return OpCode::Eql;
  }

  return OpCode::Die;
}

struct BinOp
{
  constexpr BinOp() = default;
  constexpr BinOp(OpCode op, Variable var, Value val) : Op(op), Var(var), Val(val) {}

  constexpr void Execute(uint64_t input, StaticMemory& memory) const
  {
    switch(Op)
    {
      case OpCode::Inp:
      {
        auto div = (uint64_t)std::pow(10, memory.Access(Val));
        memory.Access(Var) = input / div % 10;
        break;
      }
      case OpCode::Add:
        memory.Access(Var) += memory.Access(Val);
        break;
      case OpCode::Mul:
        memory.Access(Var) *= memory.Access(Val);
        break;
      case OpCode::Div:
        memory.Access(Var) /= memory.Access(Val);
        break;
      case OpCode::Mod:
        memory.Access(Var) %= memory.Access(Val);
        break;
      case OpCode::Eql:
        memory.Access(Var) = memory.Access(Var) == memory.Access(Val) ? 1 : 0;
        break;
      case OpCode::Die:
        break;
    }
  }

  constexpr bool Done() const { return Op == OpCode::Die; }

  OpCode Op = OpCode::Die;
  Variable Var;
  Value Val;
};

template <size_t InputSize, size_t MaxOps>
struct Program
{
  constexpr Program(std::string_view program)
  {
    size_t index = 0;
    int input_index = InputSize - 1;
    while(!program.empty())
    {
      auto end = program.find_first_of('\n');
      auto instr = program.substr(0, end);
      auto opcode = parse_op(instr.substr(0, 3));
      auto var = parse_var(instr.at(4));
      Value val;
      if(opcode == OpCode::Inp)
      {
        val = input_index--;
      }
      else
      {
        val = parse_val(instr.substr(6, end - 6));
      }
      Ops[index++] = BinOp(opcode, var, val);
      program = program.substr(end + 1);
    }
  }

  constexpr int Execute(uint64_t input)
  {
    if(!input_is_valid(input)) return -1;
    StaticMemory mem;
    for(auto const& op : Ops)
    {
      if(op.Done()) break;
      op.Execute(input, mem);
    }

    return mem.Z;
  }

  std::array<BinOp, MaxOps> Ops{};
};

template <size_t InputSize>
constexpr int execute_alu(uint64_t input, std::string_view program = AluProgram)
{
  Program<InputSize, 256> p(program);
  return p.Execute(input);
}

inline int64_t find_max_model_number()
{
  auto input = open_input("./inputs/24-1.txt");
  std::vector<std::string> program;
  while(std::getline(input, program.emplace_back()))
  {
  }
  std::array<int, 14> model_number{9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9};
  auto result = 1;
  while(result != 0)
  {
    Memory<14> memory{model_number};
    for(auto const& instr : program)
    {
      process_instruction(memory, instr);
    }
    result = memory.z;
    if(result != 0)
    {
      for(auto i = 13; i >= 0; --i)
      {
        if(i == 6) std::cout << "million" << std::endl;
        if(model_number[i] == 1)
          model_number[i] = 9;
        else
        {
          model_number[i]--;
          break;
        }
      }
    }
  }

  int64_t r = 0;
  for(auto m : model_number)
  {
    r *= 10;
    r += m;
  }
  return r;
}
}  // namespace aoc
