#pragma once

#include <string>

namespace qpp {
enum TokenType { NAME_OR_KEYWORD, KEYWORD, INTEGER, CHAR_SYMBOL };

struct QueryToken {
  const TokenType tokenType;
  const std::string value;

  bool operator==(const QueryToken& other) const {
    return tokenType == other.tokenType && value == other.value;
  }
};
}  // namespace qpp
