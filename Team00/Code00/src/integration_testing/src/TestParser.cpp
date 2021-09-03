#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("Tokenizer Parser Integration Test 1") {
  string program =
      "procedure Example {\n"
      "  x = 1; }\n"
      "\n";

  vector<string> expectedTokens = {"procedure", "Example", "{", "x",
                                   "=",         "1",       ";", "}"};

  std::vector<std::string> tokens = Tokenizer::TokenizeProgramString(program);
  REQUIRE(!tokens.empty());
  REQUIRE(tokens.size() == 8);
  REQUIRE(tokens == expectedTokens);

  // =======================

  FactorAST* f1 = new FactorAST(1);
  ExprAST* t1 = new ExprAST(f1);
  ExprAST* expr = new ExprAST(t1);

  AssignStmtAST* assign = new AssignStmtAST(1, "x", expr);
  vector<StmtAST*> stmtList;
  stmtList.push_back(assign);
  ProcedureAST* procedure = new ProcedureAST("Example", stmtList);
  vector<ProcedureAST*> procedureList;
  procedureList.push_back(procedure);
  ProgramAST* e = new ProgramAST(procedureList);  // expected

  // returned
  ProgramAST* r = Parser().Parse(Tokenizer::TokenizeProgramString(program));

  // ==========================

  REQUIRE(!r->ProcedureList.empty());
  REQUIRE(r->ProcedureList.size() == 1);

  // procedure Example {
  REQUIRE(r->ProcedureList[0]->ProcName == e->ProcedureList[0]->ProcName);

  // x=1
  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  AssignStmtAST* ea =
      dynamic_cast<AssignStmtAST*>(e->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo ==
          e->ProcedureList[0]->StmtList[0]->StmtNo);
  REQUIRE(ra->StmtNo == ea->StmtNo);
  REQUIRE(ra->VarName == ea->VarName);

  // Expr
  ExprAST* rExpr = ra->Expr;
  ExprAST* eExpr = ea->Expr;
  REQUIRE(rExpr->HasOnlyOneNode());
  REQUIRE(rExpr->HasOnlyOneNode() == eExpr->HasOnlyOneNode());
  REQUIRE(rExpr->Sign == eExpr->Sign);
  REQUIRE(rExpr->Sign == "");

  // Term
  ExprAST* rTerm = rExpr->LeftNode;
  ExprAST* eTerm = eExpr->LeftNode;
  REQUIRE(rTerm->HasOnlyOneNode());
  REQUIRE(rTerm->HasOnlyOneNode() == eTerm->HasOnlyOneNode());
  REQUIRE(rTerm->Sign == eTerm->Sign);
  REQUIRE(rTerm->Sign == "");

  // Factor
  FactorAST* rFactor = dynamic_cast<FactorAST*>(rTerm->LeftNode);
  FactorAST* eFactor = dynamic_cast<FactorAST*>(eTerm->LeftNode);
  REQUIRE(rFactor->IsConstValue());
  REQUIRE(rFactor->IsConstValue() == eFactor->IsConstValue());
  REQUIRE(rFactor->ConstValue == 1);
  REQUIRE(rFactor->ConstValue == eFactor->ConstValue);
}

TEST_CASE("Tokenizer Parser Integration Test 2") {
  string program =
      "procedure Example {\n"
      "  x = 1-2+3; }\n"
      "\n";

  vector<string> expectedTokens = {"procedure", "Example", "{", "x", "=", "1",
                                   "-",         "2",       "+", "3", ";", "}"};

  std::vector<std::string> tokens = Tokenizer::TokenizeProgramString(program);
  REQUIRE(!tokens.empty());
  REQUIRE(tokens.size() == 12);
  REQUIRE(tokens == expectedTokens);

  // =======================

  FactorAST* f1 = new FactorAST(1);
  FactorAST* f2 = new FactorAST(2);
  FactorAST* f3 = new FactorAST(3);

  ExprAST* t1 = new ExprAST(f1);
  ExprAST* t2 = new ExprAST(f2);
  ExprAST* e1 = new ExprAST("-", t1, t2);

  ExprAST* t3 = new ExprAST(f3);
  ExprAST* expr = new ExprAST("+", e1, t3);

  AssignStmtAST* assign = new AssignStmtAST(1, "x", expr);
  vector<StmtAST*> stmtList;
  stmtList.push_back(assign);
  ProcedureAST* procedure = new ProcedureAST("Example", stmtList);
  vector<ProcedureAST*> procedureList;
  procedureList.push_back(procedure);
  ProgramAST* e = new ProgramAST(procedureList);  // expected

  // returned
  ProgramAST* r = Parser().Parse(Tokenizer::TokenizeProgramString(program));

  // ==========================

  REQUIRE(!r->ProcedureList.empty());
  REQUIRE(r->ProcedureList.size() == 1);

  // procedure Example {
  REQUIRE(r->ProcedureList[0]->ProcName == e->ProcedureList[0]->ProcName);

  // x=1-2+3
  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  AssignStmtAST* ea =
      dynamic_cast<AssignStmtAST*>(e->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo ==
          e->ProcedureList[0]->StmtList[0]->StmtNo);
  REQUIRE(ra->StmtNo == ea->StmtNo);
  REQUIRE(ra->VarName == ea->VarName);

  // (1-2) + (3)
  ExprAST* re123 = ra->Expr;
  ExprAST* ee123 = ea->Expr;
  REQUIRE(re123->HasOnlyOneNode() == false);
  REQUIRE(re123->Sign == ee123->Sign);
  REQUIRE(re123->Sign == "+");

  // (3)
  ExprAST* re3 = re123->RightNode;
  ExprAST* ee3 = ee123->RightNode;
  FactorAST* rf3 = dynamic_cast<FactorAST*>(re3->LeftNode);
  FactorAST* ef3 = dynamic_cast<FactorAST*>(ee3->LeftNode);
  REQUIRE(re3->HasOnlyOneNode());
  REQUIRE(re3->HasOnlyOneNode() == ee3->HasOnlyOneNode());
  REQUIRE(rf3->IsConstValue());
  REQUIRE(rf3->IsConstValue() == ef3->IsConstValue());
  REQUIRE(rf3->ConstValue == 3);
  REQUIRE(rf3->ConstValue == ef3->ConstValue);

  // (1-2)
  ExprAST* re12 = re123->LeftNode;
  ExprAST* ee12 = ee123->LeftNode;
  REQUIRE(re12->HasOnlyOneNode() == false);
  REQUIRE(re12->Sign == ee12->Sign);
  REQUIRE(re12->Sign == "-");

  // (1)
  ExprAST* re1 = re12->LeftNode;
  ExprAST* ee1 = ee12->LeftNode;
  FactorAST* rf1 = dynamic_cast<FactorAST*>(re1->LeftNode);
  FactorAST* ef1 = dynamic_cast<FactorAST*>(ee1->LeftNode);
  REQUIRE(re1->HasOnlyOneNode());
  REQUIRE(re1->HasOnlyOneNode() == ee1->HasOnlyOneNode());
  REQUIRE(rf1->IsConstValue());
  REQUIRE(rf1->IsConstValue() == ef1->IsConstValue());
  REQUIRE(rf1->ConstValue == 1);
  REQUIRE(rf1->ConstValue == ef1->ConstValue);

  // (2)
  ExprAST* re2 = re12->RightNode;
  ExprAST* ee2 = ee12->RightNode;
  FactorAST* rf2 = dynamic_cast<FactorAST*>(re2->LeftNode);
  FactorAST* ef2 = dynamic_cast<FactorAST*>(ee2->LeftNode);
  REQUIRE(re2->HasOnlyOneNode());
  REQUIRE(re2->HasOnlyOneNode() == ee2->HasOnlyOneNode());
  REQUIRE(rf2->IsConstValue());
  REQUIRE(rf2->IsConstValue() == ef2->IsConstValue());
  REQUIRE(rf2->ConstValue == 2);
  REQUIRE(rf2->ConstValue == ef2->ConstValue);
}
