#pragma once

#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>
#include <Common/Common.h>

#include <set>
#include <string>
#include <vector>

class TestQueryOptimizerUtil {
 public:
  inline static std::string DEFAULT_S1_NAME = "s1";
  inline static std::string DEFAULT_S2_NAME = "s2";
  inline static std::string DEFAULT_A_NAME = "a";
  inline static std::string DEFAULT_V_NAME = "v";
  inline static std::string DEFAULT_W_NAME = "w";
  inline static std::string DEFAULT_IFS_NAME = "ifs";
  inline static std::string DEFAULT_C_NAME = "c";
  inline static std::string DEFAULT_N_NAME = "n";

  inline static SynonymMap DEFAULT_SYNONYM_MAP = {
      {DEFAULT_S1_NAME, query::DesignEntity::STATEMENT},
      {DEFAULT_S2_NAME, query::DesignEntity::STATEMENT},
      {DEFAULT_A_NAME, query::DesignEntity::ASSIGN},
      {DEFAULT_V_NAME, query::DesignEntity::VARIABLE},
      {DEFAULT_W_NAME, query::DesignEntity::WHILE},
      {DEFAULT_IFS_NAME, query::DesignEntity::IF},
      {DEFAULT_C_NAME, query::DesignEntity::CALL},
      {DEFAULT_N_NAME, query::DesignEntity::PROG_LINE},
  };

  inline static query::Synonym DEFAULT_S1 = {query::DesignEntity::STATEMENT,
                                             DEFAULT_S1_NAME};
  inline static query::Synonym DEFAULT_S2 = {query::DesignEntity::STATEMENT,
                                             DEFAULT_S2_NAME};
  inline static query::Synonym DEFAULT_A = {query::DesignEntity::ASSIGN,
                                            DEFAULT_A_NAME};
  inline static query::Synonym DEFAULT_V = {query::DesignEntity::VARIABLE,
                                            DEFAULT_V_NAME};
  inline static query::Synonym DEFAULT_W = {query::DesignEntity::WHILE,
                                            DEFAULT_W_NAME};
  inline static query::Synonym DEFAULT_IFS = {query::DesignEntity::IF,
                                              DEFAULT_IFS_NAME};
  inline static query::Synonym DEFAULT_C = {query::DesignEntity::CALL,
                                            DEFAULT_C_NAME};
  inline static query::Synonym DEFAULT_N = {query::DesignEntity::PROG_LINE,
                                            DEFAULT_N_NAME};

  // such that Parent(1, 2)
  inline static const query::ConditionClause DEFAULT_SUCH_THAT_LITERAL = {
      {RelationshipType::FOLLOWS_T,
       {query::ParamType::INTEGER_LITERAL, "1"},
       {query::ParamType::INTEGER_LITERAL, "2"}},
      {},
      {},
      query::ConditionClauseType::SUCH_THAT};

  // such that Follows*(s1, s2)
  inline static const query::ConditionClause DEFAULT_SUCH_THAT_SYNONYM = {
      {RelationshipType::FOLLOWS_T,
       {query::ParamType::SYNONYM, DEFAULT_S1_NAME},
       {query::ParamType::SYNONYM, DEFAULT_S2_NAME}},
      {},
      {},
      query::ConditionClauseType::SUCH_THAT};

  // pattern a (v, _"x+y"_)
  inline static const query::ConditionClause DEFAULT_PATT_A_SYNONYM = {
      {},
      {{query::DesignEntity::ASSIGN, DEFAULT_A_NAME, false, {}},
       {query::ParamType::SYNONYM, DEFAULT_V_NAME},
       {query::MatchType::SUB_EXPRESSION, "[[x]+[y]]"}},
      {},
      query::ConditionClauseType::PATTERN};

  // pattern w (v, _)
  inline static const query::ConditionClause DEFAULT_PATT_W_SYNONYM = {
      {},
      {{query::DesignEntity::WHILE, DEFAULT_W_NAME, false, {}},
       {query::ParamType::SYNONYM, DEFAULT_V_NAME},
       {}},
      {},
      query::ConditionClauseType::PATTERN};

  // pattern ifs (v, _, _)
  inline static const query::ConditionClause DEFAULT_PATT_IF_SYNONYM = {
      {},
      {{query::DesignEntity::IF, DEFAULT_IFS_NAME, false, {}},
       {query::ParamType::SYNONYM, DEFAULT_V_NAME},
       {}},
      {},
      query::ConditionClauseType::PATTERN};

  // with 1 = 2
  inline static const query::ConditionClause DEFAULT_WITH_LITERAL = {
      {},
      {},
      {{query::ParamType::INTEGER_LITERAL, "1"},
       {query::ParamType::INTEGER_LITERAL, "2"}},
      query::ConditionClauseType::WITH};

  // with s1.stmt# = n
  inline static const query::ConditionClause DEFAULT_WITH_SYNONYM_INT = {
      {},
      {},
      {{query::ParamType::ATTRIBUTE_STMT_NUM, DEFAULT_S1_NAME},
       {query::ParamType::SYNONYM, DEFAULT_N_NAME}},
      query::ConditionClauseType::WITH};

  // with c.procName = v.varName
  inline static const query::ConditionClause DEFAULT_WITH_SYNONYM_NAME = {
      {},
      {},
      {{query::ParamType::ATTRIBUTE_PROC_NAME, DEFAULT_C_NAME},
       {query::ParamType::ATTRIBUTE_VAR_NAME, DEFAULT_V_NAME}},
      query::ConditionClauseType::WITH};

  inline static const std::vector<query::ConditionClause> ALL_DEFAULT_CLAUSE = {
      TestQueryOptimizerUtil::DEFAULT_SUCH_THAT_LITERAL,
      TestQueryOptimizerUtil::DEFAULT_SUCH_THAT_SYNONYM,
      TestQueryOptimizerUtil::DEFAULT_PATT_A_SYNONYM,
      TestQueryOptimizerUtil::DEFAULT_PATT_W_SYNONYM,
      TestQueryOptimizerUtil::DEFAULT_PATT_IF_SYNONYM,
      TestQueryOptimizerUtil::DEFAULT_WITH_LITERAL,
      TestQueryOptimizerUtil::DEFAULT_WITH_SYNONYM_INT,
      TestQueryOptimizerUtil::DEFAULT_WITH_SYNONYM_NAME,
  };
};

// groups:
// 1: st literal
// 2: (s1, s2, n) st synonym, with synonym int
// 3: with literal
// 4: (a, v, w, ifs) patt a, patt w, patt ifs, with synonym name
