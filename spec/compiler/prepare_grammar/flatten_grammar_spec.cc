#include "spec_helper.h"
#include "compiler/prepare_grammar/flatten_grammar.h"
#include "compiler/prepare_grammar/initial_syntax_grammar.h"
#include "compiler/syntax_grammar.h"
#include "compiler/rules/built_in_symbols.h"
#include "helpers/rule_helpers.h"

template<typename T, typename Func>
std::vector<typename std::result_of<Func(T)>::type>
collect(const std::vector<T> &v, Func f) {
  vector<typename std::result_of<Func(T)>::type> result;
  for (const T &item : v)
    result.push_back(f(item));
  return result;
}

START_TEST

using namespace rules;
using prepare_grammar::flatten_grammar;
using prepare_grammar::InitialSyntaxGrammar;

describe("flatten_grammar", []() {
  InitialSyntaxGrammar input_grammar{{

    // Choices within rules are extracted, resulting in multiple productions.
    Variable("variable0", VariableTypeNamed, seq({
      i_sym(1),
      choice({ i_sym(2), i_sym(3) }),
      i_sym(4),
    })),

    // When multiple precedence values are nested, the inner precedence wins.
    Variable("variable1", VariableTypeNamed, seq({
      i_sym(1),
      prec_left(101, seq({
        i_sym(2),
        choice({
          prec_right(102, seq({
            i_sym(3),
            i_sym(4)
          })),
          i_sym(5),
        }),
        i_sym(6),
      })),
      i_sym(7),
    })),

    // When a precedence is applied to the end of a rule, its value is assigned
    // to the last step of the corresponding production.
    Variable("variable2", VariableTypeHidden, seq({
      prec_left(102, seq({
        i_sym(1),
        i_sym(2),
      })),
      prec_left(103, seq({
        i_sym(3),
        i_sym(4),
      })),
    }))

  }, {}, {}};

  SyntaxGrammar grammar = flatten_grammar(input_grammar);

  auto get_symbol_sequences = [&](vector<Production> productions) {
    return collect(productions, [](Production p) {
      return collect(p, [](ProductionStep e) {
        return e.symbol;
      });
    });
  };

  auto get_precedence_sequences = [&](vector<Production> productions) {
    return collect(productions, [](Production p) {
      return collect(p, [](ProductionStep e) {
        return e.precedence;
      });
    });
  };

  auto get_associativity_sequences = [&](vector<Production> productions) {
    return collect(productions, [](Production p) {
      return collect(p, [](ProductionStep e) {
        return e.associativity;
      });
    });
  };

  it("preserves the names and types of the grammar's variables", [&]() {
    AssertThat(grammar.variables[0].internal_name, Equals("variable0"));
    AssertThat(grammar.variables[1].internal_name, Equals("variable1"));
    AssertThat(grammar.variables[2].internal_name, Equals("variable2"));

    AssertThat(grammar.variables[0].type, Equals(VariableTypeNamed));
    AssertThat(grammar.variables[1].type, Equals(VariableTypeNamed));
    AssertThat(grammar.variables[2].type, Equals(VariableTypeHidden));
  });

  it("turns each variable's rule with a vector of possible symbol sequences", [&]() {
    AssertThat(
      get_symbol_sequences(grammar.variables[0].productions),
      Equals(vector<vector<Symbol>>({
        { Symbol(1), Symbol(2), Symbol(4) },
        { Symbol(1), Symbol(3), Symbol(4) }
      })));

    AssertThat(
      get_symbol_sequences(grammar.variables[1].productions),
      Equals(vector<vector<Symbol>>({
        { Symbol(1), Symbol(2), Symbol(3), Symbol(4), Symbol(6), Symbol(7) },
        { Symbol(1), Symbol(2), Symbol(5), Symbol(6), Symbol(7) }
      })));

    AssertThat(
      get_symbol_sequences(grammar.variables[2].productions),
      Equals(vector<vector<Symbol>>({
        { Symbol(1), Symbol(2), Symbol(3), Symbol(4) },
      })));
  });

  it("associates each symbol with the precedence binding it to its previous neighbor", [&]() {
    AssertThat(
      get_precedence_sequences(grammar.variables[0].productions),
      Equals(vector<vector<int>>({
        { 0, 0, 0 },
        { 0, 0, 0 }
      })));

    AssertThat(
      get_precedence_sequences(grammar.variables[1].productions),
      Equals(vector<vector<int>>({
        { 0, 101, 102, 101, 0, 0 },
        { 0, 101, 101, 0, 0 }
      })));

    AssertThat(
      get_precedence_sequences(grammar.variables[2].productions),
      Equals(vector<vector<int>>({
        { 102, 0, 103, 103 },
      })));
  });

  it("associates each symbol with the correct associativity annotation", [&]() {
    Associativity none = AssociativityNone;

    AssertThat(
      get_associativity_sequences(grammar.variables[1].productions),
      Equals(vector<vector<Associativity>>({
        { none, AssociativityLeft, AssociativityRight, AssociativityLeft, none, none },
        { none, AssociativityLeft, AssociativityLeft, none, none }
      })));
  });
});

END_TEST
