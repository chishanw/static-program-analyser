#pragma once

#include <stdexcept>
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

class SyntacticErrorException : public std::runtime_error {
 public:
  explicit SyntacticErrorException(const std::string& what_arg)
      : std::runtime_error(what_arg) {}
};

class SemanticBooleanErrorException : public std::runtime_error {
 public:
  explicit SemanticBooleanErrorException(const std::string& what_arg)
      : std::runtime_error(what_arg) {}
};

class SemanticSynonymErrorException : public std::runtime_error {
 public:
  explicit SemanticSynonymErrorException(const std::string& what_arg)
      : std::runtime_error(what_arg) {}
};

}  // namespace qpp
