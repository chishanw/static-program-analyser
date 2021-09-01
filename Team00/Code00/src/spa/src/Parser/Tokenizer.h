//
// Created by Guofeng Tang on 1/9/21.
//

#ifndef AUTOTESTER_TOKENIZER_H
#define AUTOTESTER_TOKENIZER_H

#include <set>
#include <string>
#include <vector>

class Tokenizer {
 public:
  static std::vector<std::string> TokenizeFile(std::string filename);
  static std::vector<std::string> TokenizeProgramString(
      std::string program);  // for testing purpose

 private:
  static std::set<char> SPACE_SET;
  static std::set<char> SINGLE_WIDTH_SYMBOL_SET;
  static std::set<char> DOUBLE_WIDTH_SYMBOL_SET;

  static bool isSpace(char);
  static bool isDigit(char);
  static bool isLetter(char);
  static bool isSpecialSymbol(char);
  static bool isSingleWidthSymbol(char);
  static bool isDoubleWidthSymbol(char);

  static std::vector<std::string> tokenize(std::stringstream&);
  static std::string number(std::stringstream&);
  static std::string name(std::stringstream&);
  static std::string specialSymbol(std::stringstream&);
};

#endif  // AUTOTESTER_TOKENIZER_H
