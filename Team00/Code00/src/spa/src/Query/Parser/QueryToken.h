#pragma once

#include <string>

namespace qpp {
enum TokenType { NAME, KEYWORD, INTEGER, CHAR_SYMBOL };

struct QueryToken {
  const TokenType tokenType;
  const std::string value;
};
}  // namespace qpp
