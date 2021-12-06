#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <valarray>
#include <vector>

namespace aoc
{
using Moves = std::vector<uint32_t>;
using CardMatrix = std::valarray<uint32_t>;

class BingoCard
{
 public:
  static constexpr size_t CardSize = 5;
  static constexpr size_t CardLength = CardSize * CardSize;

  [[nodiscard]] std::optional<uint32_t> CheckWin(std::set<uint32_t> const& moves,
                                                 uint32_t mostRecentMove) const
  {
    std::valarray<uint32_t> contains(CardSize * CardSize);
    for(auto i = 0; i < CardLength; ++i)
    {
      contains[i] = moves.contains(_numbers[i]);
    }
    for(auto i = 0; i < CardSize; ++i)
    {
      std::valarray<uint32_t> row = contains[std::slice(CardSize * i, CardSize, 1)];
      std::valarray<uint32_t> col = contains[std::slice(i, CardSize, CardSize)];
      if(row.sum() == CardSize || col.sum() == CardSize)
      {
        return {Score(moves, mostRecentMove)};
      }
    }

    return std::nullopt;
  }

 private:
  uint32_t Score(std::set<uint32_t> const& moves, uint32_t mostRecentMove) const
  {
    uint32_t score = 0;
    for(auto num : _numbers)
    {
      if(!moves.contains(num))
      {
        score += num;
      }
    }

    return score * mostRecentMove;
  }

 private:
  friend std::istream& operator>>(std::istream&, BingoCard&);

  CardMatrix _numbers = CardMatrix(CardSize * CardSize);
};

inline std::istream& operator>>(std::istream& is, BingoCard& card)
{
  for(auto i = 0; i < BingoCard::CardSize; ++i)
  {
    std::string line;
    if(!std::getline(is, line)) throw std::out_of_range("Failed to get line");
    std::istringstream iss(line);
    for(auto j = 0; j < BingoCard::CardSize; ++j)
    {
      iss >> card._numbers[j + i * BingoCard::CardSize];
      iss >> std::ws;
    }
  }

  return is;
}

class BingoGame
{
 public:
  BingoGame(Moves moves, std::vector<BingoCard> cards)
    : _moves(std::move(moves)), _cards(std::move(cards))
  {
  }

  uint32_t PlayGame()
  {
    std::set<uint32_t> moves;
    for(auto move : _moves)
    {
      moves.insert(move);
      if(_cards.size() > 1)
      {
        _cards.erase(std::remove_if(_cards.begin(), _cards.end(),
                                    [&moves, move](auto const& card) {
                                      return card.CheckWin(moves, move).has_value();
                                    }),
                     _cards.end());
      }
      else
      {
        auto won = _cards.begin()->CheckWin(moves, move);
        if(won.has_value()) return *won;
      }
    }

    throw std::out_of_range("No card won");
  }

 private:
  Moves _moves;
  std::vector<BingoCard> _cards;
};

inline BingoGame parse_bingo()
{
  std::ifstream input("./inputs/4-1.txt");
  if(!input.is_open()) throw std::out_of_range("Failed to open input file");

  std::string line;
  if(!std::getline(input, line)) throw std::out_of_range("Failed to get line");
  std::istringstream iss(line);
  Moves moves;
  while(iss)
  {
    iss >> moves.emplace_back();
    iss.ignore(1, ',');
  }

  std::vector<BingoCard> cards;
  std::string ignore;
  while(std::getline(input, ignore))
  {
    input >> cards.emplace_back();
  }

  return BingoGame(std::move(moves), std::move(cards));
}
}  // namespace aoc
