#include "rule_helpers.h"
#include <memory>
#include "compiler/rules/symbol.h"

namespace tree_sitter {
  using std::make_shared;
  using std::set;
  using std::map;
  using std::ostream;
  using std::string;
  using std::to_string;

  rule_ptr character(const set<uint32_t> &ranges) {
    return character(ranges, true);
  }

  rule_ptr character(const set<uint32_t> &chars, bool sign) {
    rules::CharacterSet result;
    if (sign) {
      for (uint32_t c : chars)
        result.include(c);
    } else {
      result.include_all();
      for (uint32_t c : chars)
        result.exclude(c);
    }
    return result.copy();
  }

  rule_ptr i_sym(size_t index) {
    return make_shared<rules::Symbol>(index);
  }

  rule_ptr i_token(size_t index) {
    return make_shared<rules::Symbol>(index, true);
  }

  rule_ptr metadata(rule_ptr rule, map<rules::MetadataKey, int> values) {
    return make_shared<rules::Metadata>(rule, values);
  }

  rule_ptr active_prec(int precedence, rule_ptr rule) {
    return std::make_shared<rules::Metadata>(rule, map<rules::MetadataKey, int>({
      { rules::PRECEDENCE, precedence },
      { rules::IS_ACTIVE, true }
    }));
  }

  bool operator==(const Variable &left, const Variable &right) {
    return left.internal_name == right.internal_name &&
      left.external_name == right.external_name &&
      left.rule->operator==(*right.rule) &&
      left.type == right.type;
  }
}
