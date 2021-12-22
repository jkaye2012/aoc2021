#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <unordered_set>
#include <utility>

#include "util.h"

namespace aoc
{
template <typename T>
concept Rollable = requires(T die, uint64_t turns)
{
  // clang-format off
 { die.roll(turns) } -> std::convertible_to<uint64_t>;
  // clang-format on
};

template <typename T>
concept Numeric = requires()
{
  // clang-format off
 { std::numeric_limits<T>::is_specialized } -> std::convertible_to<bool>;
  // clang-format on
};

template <uint64_t MaxValue>
struct DeterministicDice
{
  constexpr uint64_t roll(uint64_t turns)
  {
    if(turns == 0)
    {
      return 0;
    }
    uint64_t result = _value + 1;
    _value = result % MaxValue;
    return result + roll(turns - 1);
  }

 private:
  uint64_t _value = 0;
};

template <typename T, uint64_t N>
constexpr T fold(std::array<T, N> const& arr, T init, T binop(T const&, T const&))
{
  T result = std::move(init);
  for(auto n = 0; n < N; ++n)
  {
    result = binop(result, arr[n]);
  }
  return result;
}

template <Numeric T, uint64_t N>
constexpr T max(std::array<T, N> const& arr)
{
  T(*binop)
  (T const&, T const&) = [](T const& max, T const& curr) -> T {
    return curr > max ? curr : max;
  };
  return fold(arr, std::numeric_limits<T>::min(), binop);
}

template <Numeric T, uint64_t N>
constexpr T min(std::array<T, N> const& arr)
{
  T(*binop)
  (T const&, T const&) = [](T const& min, T const& curr) -> T {
    return curr < min ? curr : min;
  };
  return fold(arr, std::numeric_limits<T>::max(), binop);
}

template <uint64_t NumPlayers, uint64_t BoardSize, uint64_t MaxScore, Rollable Dice,
          uint64_t RollsPerTurn>
struct DiracDice
{
  constexpr DiracDice(std::array<uint64_t, NumPlayers> positions)
    : _positions(std::move(positions))
  {
  }

  constexpr uint64_t Play()
  {
    Dice dice;
    uint64_t num_turns = 0;
    while(true)
    {
      auto player = num_turns % NumPlayers;
      num_turns++;
      auto move = dice.roll(RollsPerTurn);
      _positions[player] = (_positions[player] + move) % BoardSize;
      _scores[player] += _positions[player] + 1;
      if(auto max_score = max(_scores); max_score >= 1000)
      {
        // player is winner
        return num_turns * RollsPerTurn * min(_scores);
      }
    }
  }

 private:
  uint64_t _current_player = 0;
  std::array<uint64_t, NumPlayers> _positions;
  std::array<uint64_t, NumPlayers> _scores{};
};

static constexpr std::array<uint64_t, 7> QuantumOutcomes()
{
  std::array<uint64_t, 7> outcomes{};
  for(uint64_t die1 = 0; die1 < 3; ++die1)
  {
    for(uint64_t die2 = 0; die2 < 3; ++die2)
    {
      for(uint64_t die3 = 0; die3 < 3; ++die3)
      {
        outcomes[die1 + die2 + die3]++;
      }
    }
  }

  return outcomes;
}

struct QuantumDiceGame
{
  using Universes = std::array<std::map<uint64_t, uint64_t>, 10>;

  QuantumDiceGame(uint64_t player1, uint64_t player2)
  {
    _player_1[player1].insert({0, 1});
    _player_2[player2].insert({0, 1});
  }

  uint64_t QuantumChampionWins()
  {
    auto [player1, player2] = WinsPerPlayer();
    return std::max(player1, player2);
  }

 private:
  std::pair<uint64_t, uint64_t> WinsPerPlayer()
  {
    std::pair<uint64_t, uint64_t> wins_per_player{0, 0};
    while(PossibilitiesExistFor(_player_1) || PossibilitiesExistFor(_player_2))
    {
      auto mult = RunPlayer(_player_1, wins_per_player.first);
      ExpandUniverses(_player_2, mult);
      mult = RunPlayer(_player_2, wins_per_player.second);
      ExpandUniverses(_player_1, mult);
    }

    return wins_per_player;
  }

  double RunPlayer(Universes& player, size_t& win_tracker)
  {
    Universes new_universes;
    for(auto universe_position = 0; universe_position < 10; ++universe_position)
    {
      for(auto const& universe : player[universe_position])
      {
        for(auto roll = 3; roll < 10; ++roll)
        {
          auto num_outcomes = _outcomes[roll - 3] * universe.second;
          auto new_position = (universe_position + roll) % 10;
          auto score = universe.first + new_position + 1;
          if(score >= 21)
          {
            win_tracker += num_outcomes;
          }
          else
          {
            new_universes[new_position][score] += num_outcomes;
          }
        }
      }
    }

    auto multiplier =
        1.0 * TotalUniversesFor(new_universes) / TotalUniversesFor(_player_1);
    player = new_universes;
    return multiplier;
  }

  bool PossibilitiesExistFor(Universes const& us) const
  {
    for(auto const& up : us)
    {
      if(!up.empty()) return true;
    }
    return false;
  }

  void ExpandUniverses(Universes& us, double mult)
  {
    for(auto& up : us)
    {
      for(auto& u : up)
      {
        u.second = std::round(u.second * mult);
      }
    }
  }

  uint64_t TotalUniversesFor(Universes const& us) const
  {
    uint64_t total = 0;
    for(auto const& up : us)
    {
      for(auto const& [_, count] : up)
      {
        total += count;
      }
    }
    return total;
  }

 private:
  static constexpr std::array<uint64_t, 7> _outcomes = QuantumOutcomes();
  Universes _player_1{};
  Universes _player_2{};
};

}  // namespace aoc
