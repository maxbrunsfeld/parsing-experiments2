#include "compiler/lex_table.h"

namespace tree_sitter {
    using std::string;
    using std::to_string;
    using std::map;
    using std::set;
    using rules::Symbol;
    using rules::CharacterSet;

    LexAction::LexAction() : LexAction(LexActionTypeError, -1, Symbol("")) {}

    LexAction::LexAction(LexActionType type, size_t state_index, Symbol symbol) :
        type(type),
        symbol(symbol),
        state_index(state_index) {}

    LexAction LexAction::Error() {
        return LexAction(LexActionTypeError, -1, Symbol(""));
    }

    LexAction LexAction::Advance(size_t state_index) {
        return LexAction(LexActionTypeAdvance, state_index, Symbol(""));
    }

    LexAction LexAction::Accept(Symbol symbol) {
        return LexAction(LexActionTypeAccept, -1, symbol);
    }

    bool LexAction::operator==(const LexAction &other) const {
        return
        (type == other.type) &&
        (state_index == other.state_index) &&
        (symbol == other.symbol);
    }

    bool LexAction::operator<(const LexAction &other) const {
        if (type < other.type) return true;
        if (type > other.type) return false;
        if (state_index < other.state_index) return true;
        if (state_index > other.state_index) return false;
        return (symbol < other.symbol);
    }

    std::ostream& operator<<(std::ostream &stream, const LexAction &action) {
        switch (action.type) {
            case LexActionTypeError:
                return stream << string("#<error>");
            case LexActionTypeAccept:
                return stream << string("#<accept ") + action.symbol.name + ">";
            case LexActionTypeAdvance:
                return stream << string("#<advance ") + to_string(action.state_index) + ">";
        }
    }

    set<CharacterSet> LexState::expected_inputs() const {
        set<CharacterSet> result;
        for (auto &pair : actions)
            result.insert(pair.first);
        return result;
    }

    LexStateId LexTable::add_state() {
        states.push_back(LexState());
        return states.size() - 1;
    }

    LexState & state(LexTable *table, LexStateId id) {
        if (id < 0)
            return table->error_state;
        else
            return table->states[id];
    }

    void LexTable::add_action(LexStateId id, CharacterSet match, LexAction action) {
        state(this, id).actions[match] = action;
    }

    void LexTable::add_default_action(LexStateId id, LexAction action) {
        state(this, id).default_action = action;
    }

    const LexStateId LexTable::ERROR_STATE_ID = -1;
}