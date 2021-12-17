#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <limits>
#include <numeric>
#include <span>
#include <stdexcept>
#include <variant>
#include <vector>

#include "parse.h"

namespace aoc
{
static constexpr auto NumBitsPerHex = 4;

template <size_t N>
std::bitset<N> bits_from_span(std::span<bool const, N> bits)
{
  std::bitset<N> bs;
  for(auto b : bits)
  {
    bs <<= 1;
    bs.set(0, b);
  }
  return bs;
}

template <size_t N>
inline size_t bits_to_ulong(std::span<bool const, N> bits)
{
  auto bs = bits_from_span(bits);
  return bs.to_ulong();
}

template <size_t N, size_t M>
inline std::bitset<N + M> append(std::bitset<N> const& fst, std::bitset<M> const& snd)
{
  std::bitset<N + M> bs;
  bs <<= fst;
  bs <<= snd;
  return bs;
}

template <size_t N>
std::array<bool, N> array_from_vec(size_t offset, std::vector<bool> const& bits)
{
  std::array<bool, N> result{};
  for(auto i = offset; i < N + offset; ++i)
  {
    result[i - offset] = bits[i];
  }
  return result;
}

template <size_t N>
std::bitset<N> bitset_from_vec(size_t offset, std::vector<bool> const& bits)
{
  auto arr = array_from_vec<N>(offset, bits);
  return bits_from_span(std::span<bool const, N>(arr));
}

template <size_t N>
size_t ulong_from_vec(size_t offset, std::vector<bool> const& bits)
{
  auto bs = bitset_from_vec<N>(offset, bits);
  return bs.to_ulong();
}

enum class PacketType
{
  Sum,
  Product,
  Minimum,
  Maximum,
  Literal,
  GreaterThan,
  LessThan,
  EqualTo
};

inline PacketType parse_packet_type(std::span<bool const, 3> bits)
{
  switch(bits_to_ulong(bits))
  {
    case 0:
      return PacketType::Sum;
    case 1:
      return PacketType::Product;
    case 2:
      return PacketType::Minimum;
    case 3:
      return PacketType::Maximum;
    case 4:
      return PacketType::Literal;
    case 5:
      return PacketType::GreaterThan;
    case 6:
      return PacketType::LessThan;
    case 7:
      return PacketType::EqualTo;
    default:
      throw std::out_of_range("unknown packet type");
  }
}

struct PacketHeader
{
  static PacketHeader parse(std::span<bool const, 6> bits)
  {
    PacketHeader header;
    header.Version = bits_to_ulong(bits.subspan<0, 3>());
    header.Type = parse_packet_type(bits.subspan<3>());
    return header;
  }

  size_t Version;
  PacketType Type;
};

struct Packet;

std::pair<size_t, std::variant<size_t, std::vector<Packet>>> build_payload(
    PacketType type, std::vector<bool> const& bits, size_t off);

struct Packet
{
  explicit Packet(std::vector<bool> const& bits, size_t offset = 0)
    : Header(
          PacketHeader::parse(std::span<bool const, 6>(array_from_vec<6>(offset, bits))))
  {
    auto [sz, p] = build_payload(Header.Type, bits, offset);
    Payload = std::move(p);
    Length = 6 + sz;
  }

  size_t LiteralValue() const { return std::get<size_t>(Payload); }

  std::vector<Packet> const& Subpackets() const { return std::get<1>(Payload); }

  size_t VersionSum() const
  {
    size_t sum = Header.Version;
    if(auto const* subpackets = std::get_if<std::vector<Packet>>(&Payload))
    {
      for(auto const& p : *subpackets)
      {
        sum += p.VersionSum();
      }
    }
    return sum;
  }

  size_t Value() const
  {
    switch(Header.Type)
    {
      case PacketType::Sum:
        return std::accumulate(Subpackets().begin(), Subpackets().end(), 0ul,
                               [](auto sum, auto const& p) { return sum + p.Value(); });
      case PacketType::Product:
        return std::accumulate(Subpackets().begin(), Subpackets().end(), 1ul,
                               [](auto prod, auto const& p) { return prod * p.Value(); });
      case PacketType::Minimum:
        return std::accumulate(
            Subpackets().begin(), Subpackets().end(), std::numeric_limits<size_t>::max(),
            [](auto min, auto const& p) { return std::min(min, p.Value()); });
      case PacketType::Maximum:
        return std::accumulate(
            Subpackets().begin(), Subpackets().end(), std::numeric_limits<size_t>::min(),
            [](auto max, auto const& p) { return std::max(max, p.Value()); });
      case PacketType::Literal:
        return LiteralValue();
      case PacketType::GreaterThan:
        return Subpackets()[0].Value() > Subpackets()[1].Value() ? 1 : 0;
      case PacketType::LessThan:
        return Subpackets()[0].Value() < Subpackets()[1].Value() ? 1 : 0;
      case PacketType::EqualTo:
        return Subpackets()[0].Value() == Subpackets()[1].Value() ? 1 : 0;
    }
  }

  PacketHeader Header;
  std::variant<size_t, std::vector<Packet>> Payload;
  size_t Length;
};

inline std::pair<size_t, std::variant<size_t, std::vector<Packet>>> build_payload(
    PacketType type, std::vector<bool> const& bits, size_t off)
{
  std::pair<size_t, std::variant<size_t, std::vector<Packet>>> result;
  switch(type)
  {
    case(PacketType::Literal):
    {
      size_t offset = off + 1;
      std::bitset<4> chunk;
      size_t value = 0;
      do
      {
        offset += 5;
        chunk = bitset_from_vec<4>(offset + 1, bits);
        value *= 16;
        value += chunk.to_ulong();
      } while(bits[offset]);

      result.first = offset - off - 1;
      result.second.emplace<size_t>(value);
      break;
    }
    default:
    {
      size_t offset = off + 6;
      std::vector<Packet> subpackets;
      if(bits[offset])
      {
        auto num_packets = ulong_from_vec<11>(offset + 1, bits);
        result.first = 12;
        for(auto n = 0; n < num_packets; ++n)
        {
          auto const& packet = subpackets.emplace_back(bits, offset + result.first);
          result.first += packet.Length;
        }
      }
      else
      {
        auto packet_length = ulong_from_vec<15>(offset + 1, bits);
        result.first = 16;
        while(result.first - 16 < packet_length)
        {
          auto const& packet = subpackets.emplace_back(bits, offset + result.first);
          result.first += packet.Length;
        }
      }

      result.second.emplace<1>(std::move(subpackets));
      break;
    }
  }

  return result;
}

inline std::bitset<NumBitsPerHex> parse_char(char c)
{
  if(c <= '9') return std::bitset<NumBitsPerHex>(c - '0');
  return std::bitset<NumBitsPerHex>(c - 'A' + 10);
}

inline Packet parse_hex()
{
  auto input = open_input("./inputs/16-1.txt");
  std::array<char, 1> digit;
  std::vector<bool> bits;
  while(input.read(digit.data(), 1) && digit[0] != '\n')
  {
    auto bs = parse_char(digit[0]);
    for(auto i = NumBitsPerHex - 1; i >= 0; --i)
    {
      bits.push_back(bs[i]);
    }
  }
  return Packet(bits);
}
}  // namespace aoc
