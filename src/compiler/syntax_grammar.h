#ifndef COMPILER_SYNTAX_GRAMMAR_H_
#define COMPILER_SYNTAX_GRAMMAR_H_

#include <vector>
#include <string>
#include <set>
#include "compiler/rules/symbol.h"
#include "compiler/rules/metadata.h"
#include "compiler/grammar.h"

namespace tree_sitter {

struct ProductionStep {
  ProductionStep(const rules::Symbol &, int, rules::Associativity);
  bool operator==(const ProductionStep &) const;

  rules::Symbol symbol;
  int precedence;
  rules::Associativity associativity;
};

typedef std::vector<ProductionStep> Production;

struct SyntaxVariable {
  SyntaxVariable(const std::string &, VariableType,
                 const std::vector<Production> &);
  SyntaxVariable(const std::string &, const std::string &, VariableType,
                 const std::vector<Production> &);

  std::string internal_name;
  std::string external_name;
  std::vector<Production> productions;
  VariableType type;
};

typedef std::set<rules::Symbol> ConflictSet;

struct SyntaxGrammar {
  const std::vector<Production> &productions(const rules::Symbol &) const;

  std::vector<SyntaxVariable> variables;
  std::set<rules::Symbol> extra_tokens;
  std::set<ConflictSet> expected_conflicts;
};

}  // namespace tree_sitter

#endif  // COMPILER_SYNTAX_GRAMMAR_H_
