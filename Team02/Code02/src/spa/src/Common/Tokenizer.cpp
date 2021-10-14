#include "Tokenizer.h"

#include <Common/Global.h>

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

using namespace std;

set<char> Tokenizer::SINGLE_WIDTH_SYMBOL_SET = {
    '{', '}', '(', ')',       // brackets
    '+', '-', '*', '/', '%',  // arith
    ';',                      // semi colon
};
set<char> Tokenizer::DOUBLE_WIDTH_SYMBOL_SET = {
    '&',  // &&
    '|',  // ||

    '!',  // !=
    '>',  // >=
    '<',  // <=
    '=',  // ==
};

vector<string> Tokenizer::TokenizeProgramString(string program) {
  stringstream ss;
  ss.str(program);
  return tokenize(ss);
}

vector<string> Tokenizer::TokenizeFile(string filename) {
  DMOprintInfoMsg("File to parse: " + filename);

  ifstream fh(filename);
  stringstream ss;
  if (fh) {
    ss << fh.rdbuf();
    fh.close();
  } else {
    throw runtime_error("[Tokenizer] Failed to open SIMPLE program file: " +
                        filename);
  }
  return tokenize(ss);
}

vector<string> Tokenizer::tokenize(stringstream& ss) {
  vector<string> tokens;

  while (true) {
    while (isspace(ss.peek())) ss.get();

    if (ss.peek() == EOF) {
      break;
    } else if (isDigit(ss.peek())) {
      tokens.push_back(number(ss));
    } else if (isLetter(ss.peek())) {
      tokens.push_back(name(ss));
    } else if (isSpecialSymbol(ss.peek())) {
      tokens.push_back(specialSymbol(ss));
    } else {
      throw runtime_error("[Tokenizer] Unrecognized token: " +
                          string(1, ss.get()));
    }
  }

  return tokens;
}

bool Tokenizer::isDigit(char c) { return '0' <= c && c <= '9'; }

bool Tokenizer::isLetter(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool Tokenizer::isSingleWidthSymbol(char c) {
  return SINGLE_WIDTH_SYMBOL_SET.find(c) != SINGLE_WIDTH_SYMBOL_SET.end();
}

bool Tokenizer::isDoubleWidthSymbol(char c) {
  return DOUBLE_WIDTH_SYMBOL_SET.find(c) != DOUBLE_WIDTH_SYMBOL_SET.end();
}

bool Tokenizer::isSpecialSymbol(char c) {
  // comparison symbol can be length of 2, requires some hack
  return isSingleWidthSymbol(c) || isDoubleWidthSymbol(c);
}

string Tokenizer::number(stringstream& ss) {
  string res;
  while (isDigit(ss.peek())) {
    res.push_back(ss.get());
  }
  return res;
}

string Tokenizer::name(stringstream& ss) {
  string res;
  // first char of a name must be a letter
  res.push_back(ss.get());
  // subsequent char can be either digit or letter
  while (isLetter(ss.peek()) || isDigit(ss.peek())) {
    res.push_back(ss.get());
  }
  return res;
}

string Tokenizer::specialSymbol(stringstream& ss) {
  string res;
  if (isDoubleWidthSymbol(ss.peek())) {
    char firstChar = ss.get();
    res.push_back(firstChar);
    switch (firstChar) {
      // category: following char must be the same as the 1st one,
      // otherwise emit error
      case '&':  // &&
      case '|':  // ||
        if (ss.peek() != firstChar) {
          stringstream exMsg;
          exMsg << "[Tokenizer] Expected next char to be '" << firstChar
                << "' but encoutered '" << static_cast<char>(ss.peek()) << "'";
          throw runtime_error(exMsg.str());

        } else {
          char nextChar = ss.get();
          res.push_back(nextChar);
          return res;
        }
        break;

      // category: following char could be =,
      // but doesn't have to be
      case '!':  // !=
      case '>':  // >=
      case '<':  // <=
      case '=':  // ==
        if (ss.peek() != '=') {
          return res;
        } else {
          char nextChar = ss.get();
          res.push_back(nextChar);
          return res;
        }
        break;

      default:
        DMOprintErrMsgAndExit(
            "[Tokenizer] reach switch case default branch that shouldn't be "
            "reached");

        stringstream exMsg;
        exMsg << "[Tokenizer] Expected conditional expression symbols but "
                 "encoutered '"
              << firstChar << "'";
        throw runtime_error(exMsg.str());
    }
  }

  // assert (isSingleWidthSymbol(ss.peek()) == True)
  res.push_back(ss.get());
  return res;
}
