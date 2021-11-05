#pragma once

#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Evaluator/QueryEvaluator.h>
#include <Query/Optimizer/QueryOptimizer.h>
#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class TestQueryUtil {
 public:
  static void AddSuchThatClause(std::vector<query::ConditionClause>&,
                                RelationshipType, query::ParamType, std::string,
                                query::ParamType, std::string);
  static void AddPatternClause(std::vector<query::ConditionClause>&,
                               query::Synonym, query::ParamType, std::string,
                               query::PatternExpr);
  static void AddWithClause(std::vector<query::ConditionClause>&,
                            query::ParamType, std::string, query::ParamType,
                            std::string);
  static query::ConditionClause BuildSuchThatClause(RelationshipType,
                                                    query::ParamType,
                                                    std::string,
                                                    query::ParamType,
                                                    std::string);
  static query::ConditionClause BuildPatternClause(query::Synonym,
                                                   query::ParamType,
                                                   std::string,
                                                   query::PatternExpr);
  static query::ConditionClause BuildWithClause(query::ParamType, std::string,
                                                query::ParamType, std::string);
  static std::vector<std::vector<int>> EvaluateQuery(
      PKB* pkb, std::vector<query::ConditionClause> clauses,
      query::SelectType selectType,
      std::unordered_map<query::SYN_NAME, DesignEntity> synonymMap,
      std::vector<query::Synonym> selectSynonyms);
  static std::set<int> GetUniqueSelectSingleQEResults(
      std::vector<std::vector<int>> results);
};
