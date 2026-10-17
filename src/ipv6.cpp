#include "ipv6.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace ccna {
namespace {

bool parse_hex_word(std::string_view token, std::uint16_t& value) {
  if (token.empty() || token.size() > 4) return false;
  unsigned int result = 0;
  for (const char ch : token) {
    if (!std::isxdigit(static_cast<unsigned char>(ch))) return false;
    unsigned int digit = 0;
    if (ch >= '0' && ch <= '9') digit = static_cast<unsigned int>(ch - '0');
    else if (ch >= 'a' && ch <= 'f') digit = static_cast<unsigned int>(ch - 'a' + 10);
    else digit = static_cast<unsigned int>(ch - 'A' + 10);
    result = (result << 4U) | digit;
  }
  value = static_cast<std::uint16_t>(result);
  return true;
}

bool parse_ipv4_tail(std::string_view text, std::array<std::uint8_t, 4>& out) {
  std::size_t start = 0;
  for (int i = 0; i < 4; ++i) {
    const std::size_t end = text.find('.', start);
    const bool last = end == std::string_view::npos;
    const std::size_t stop = last ? text.size() : end;
    if (stop == start) return false;
    unsigned int value = 0;
    for (std::size_t p = start; p < stop; ++p) {
      const char ch = text[p];
      if (ch < '0' || ch > '9') return false;
      value = value * 10U + static_cast<unsigned int>(ch - '0');
      if (value > 255U) return false;
    }
    out[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(value);
    if (last) return i == 3;
    start = end + 1;
  }
  return false;
}

bool parse_side(const std::string& side, std::vector<std::uint16_t>& words) {
  if (side.empty()) return true;
  std::size_t start = 0;
  while (start < side.size()) {
    const std::size_t end = side.find(':', start);
    const std::size_t stop = end == std::string::npos ? side.size() : end;
    const std::string_view token(side.data() + start, stop - start);
    std::uint16_t word = 0;
    if (!parse_hex_word(token, word)) return false;
    words.push_back(word);
    if (end == std::string::npos) break;
    start = end + 1;
    if (start == side.size()) return false;
  }
  return true;
}

} // namespace

std::optional<IPv6Address> parse_ipv6(const std::string& text) {
  if (text.empty() || text.find('%') != std::string::npos) return std::nullopt;

  std::string input = text;
  std::array<std::uint8_t, 4> ipv4{};
  const std::size_t dot = input.find('.');
  if (dot != std::string::npos) {
    const std::size_t colon = input.rfind(':');
    if (colon == std::string::npos || !parse_ipv4_tail(std::string_view(input).substr(colon + 1), ipv4)) {
      return std::nullopt;
    }
    input.replace(colon + 1, input.size() - colon - 1, "v4v4");
  }

  const std::size_t compression = input.find("::");
  if (compression != std::string::npos && input.find("::", compression + 2) != std::string::npos) {
    return std::nullopt;
  }

  std::vector<std::uint16_t> left;
  std::vector<std::uint16_t> right;
  if (compression == std::string::npos) {
    if (input.find("v4v4") != std::string::npos) {
      const std::size_t pos = input.find("v4v4");
      if (pos != input.size() - 4) return std::nullopt;
      const std::string prefix = input.substr(0, pos);
      if (!prefix.empty() && prefix.back() != ':') return std::nullopt;
      const std::string clean = prefix.empty() ? std::string{} : prefix.substr(0, prefix.size() - 1);
      if (!parse_side(clean, left)) return std::nullopt;
      left.push_back(static_cast<std::uint16_t>((ipv4[0] << 8U) | ipv4[1]));
      left.push_back(static_cast<std::uint16_t>((ipv4[2] << 8U) | ipv4[3]));
    } else if (!parse_side(input, left)) {
      return std::nullopt;
    }
    if (left.size() != 8) return std::nullopt;
  } else {
    const std::string left_text = input.substr(0, compression);
    const std::string right_text = input.substr(compression + 2);
    if (!parse_side(left_text, left)) return std::nullopt;
    if (right_text == "v4v4") {
      right.push_back(static_cast<std::uint16_t>((ipv4[0] << 8U) | ipv4[1]));
      right.push_back(static_cast<std::uint16_t>((ipv4[2] << 8U) | ipv4[3]));
    } else if (!parse_side(right_text, right)) {
      return std::nullopt;
    }
    if (left.size() + right.size() >= 8) return std::nullopt;
  }

  std::vector<std::uint16_t> words;
  words.reserve(8);
  if (compression == std::string::npos) {
    words = left;
  } else {
    words.insert(words.end(), left.begin(), left.end());
    words.insert(words.end(), 8 - left.size() - right.size(), 0);
    words.insert(words.end(), right.begin(), right.end());
  }

  IPv6Address result;
  for (std::size_t i = 0; i < 8; ++i) {
    result.bytes[i * 2] = static_cast<std::uint8_t>(words[i] >> 8U);
    result.bytes[i * 2 + 1] = static_cast<std::uint8_t>(words[i] & 0xffU);
  }
  return result;
}

std::string format_ipv6(const IPv6Address& address) {
  std::array<std::uint16_t, 8> words{};
  for (std::size_t i = 0; i < 8; ++i) {
    words[i] = static_cast<std::uint16_t>((static_cast<std::uint16_t>(address.bytes[i * 2]) << 8U) |
                                          address.bytes[i * 2 + 1]);
  }

  std::size_t best_start = 8;
  std::size_t best_len = 0;
  for (std::size_t i = 0; i < 8;) {
    if (words[i] != 0) { ++i; continue; }
    const std::size_t start = i;
    while (i < 8 && words[i] == 0) ++i;
    if (i - start > best_len && i - start >= 2) {
      best_start = start;
      best_len = i - start;
    }
  }

  std::ostringstream out;
  out << std::hex << std::nouppercase;
  for (std::size_t i = 0; i < 8;) {
    if (i == best_start) {
      out << (i == 0 ? "::" : ":");
      i += best_len;
      continue;
    }
    if (i != 0 && i != best_start + best_len) out << ':';
    out << words[i];
    ++i;
  }
  return out.str();
}

IPv6Address mask_from_ipv6_prefix(int prefix_len) {
  if (prefix_len < 0 || prefix_len > 128) throw std::invalid_argument("IPv6 prefix length must be 0..128");
  IPv6Address mask;
  for (int bit = 0; bit < prefix_len; ++bit) {
    mask.bytes[static_cast<std::size_t>(bit / 8)] |= static_cast<std::uint8_t>(0x80U >> (bit % 8));
  }
  return mask;
}

IPv6Cidr make_ipv6_cidr(const IPv6Address& address, int prefix_len) {
  const auto mask = mask_from_ipv6_prefix(prefix_len);
  IPv6Cidr result;
  result.prefix_len = prefix_len;
  for (std::size_t i = 0; i < 16; ++i) {
    result.network.bytes[i] = address.bytes[i] & mask.bytes[i];
    result.last.bytes[i] = result.network.bytes[i] | static_cast<std::uint8_t>(~mask.bytes[i]);
  }
  return result;
}

bool ipv6_contains(const IPv6Cidr& cidr, const IPv6Address& address) {
  return make_ipv6_cidr(address, cidr.prefix_len).network == cidr.network;
}

} // namespace ccna
