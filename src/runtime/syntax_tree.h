#ifndef RUNTIME_SYNTAX_TREE_H_
#define RUNTIME_SYNTAX_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tree_sitter/parser.h"
#include "runtime/length.h"
#include "runtime/array.h"

#define DEFAULT_TREE_BRANCHING_FACTOR 32
extern uint32_t TREE_BRANCHING_FACTOR;

typedef struct SyntaxNode SyntaxNode;
typedef struct SyntaxTree SyntaxTree;
typedef struct TreeCursorEntry TreeCursorEntry;
typedef Array(TreeCursorEntry) TreeCursorEntries;

typedef struct {
  SyntaxTree *last;
  uint32_t count;
} NodeList;

typedef struct {
  const SyntaxTree *tree;
  const SyntaxNode *node;
  uint32_t index;
} TSNode2;

typedef struct {
  TreeCursorEntries left;
  TreeCursorEntries right;
} TreeCursor;

typedef struct {
  TSSymbol symbol;
  Length padding;
  Length size;
  bool extra;
} LeafNodeParams;

typedef struct {
  Length size;
  TSSymbol symbol;
} BreakdownEntry;

typedef Array(BreakdownEntry) BreakdownResult;

typedef struct {
  TSSymbol symbol;
  uint32_t child_count;
} InternalNodeParams;

typedef struct {
  TreeCursorEntries stack;
  Array(SyntaxTree *) next_trees;
} NodeListIterator;

NodeList ts_node_list_new();
NodeList ts_node_list_copy(NodeList *);
void ts_node_list_delete(NodeList *);
void ts_node_list_push_leaf(NodeList *, LeafNodeParams);
void ts_node_list_push_parent(NodeList *, InternalNodeParams);
void ts_node_list_reuse(NodeList *, TreeCursor *);
void ts_node_list_breakdown(NodeList *, NodeListIterator *, BreakdownResult *);
SyntaxTree *ts_node_list_to_tree(NodeList *, const TSLanguage *, SyntaxTree *);
void ts_node_list_print_dot_graph(NodeList *, const TSLanguage *, FILE *);
NodeListIterator ts_node_list_iterator_new();
void ts_node_list_iterator_delete(NodeListIterator *);

bool ts_syntax_tree_delete(SyntaxTree *);
TSNode2 ts_syntax_tree_root_node(const SyntaxTree *);
SyntaxTree *ts_syntax_tree_edit(SyntaxTree *, TSInputEdit);
void ts_syntax_tree_check_invariants(const SyntaxTree *);
void ts_syntax_tree_print_dot_graph(const SyntaxTree *, const TSLanguage *, FILE *);

TreeCursor ts_tree_cursor_new(SyntaxTree *);
void ts_tree_cursor_delete(TreeCursor *);
bool ts_tree_cursor_descend(TreeCursor *);
bool ts_tree_cursor_advance(TreeCursor *);
TSNode2 ts_tree_cursor_current_node(TreeCursor *);
Length ts_tree_cursor_position(TreeCursor *);

TSPoint ts_node2_start_point(const TSNode2 *);
TSPoint ts_node2_end_point(const TSNode2 *);
TSSymbol ts_node2_symbol(const TSNode2 *);
unsigned ts_node2_child_count(const TSNode2 *);
TSNode2 ts_node2_child(const TSNode2 *, unsigned);
TSNode2 ts_node2_parent(const TSNode2 *);
bool ts_node2_has_changes(const TSNode2 *);

#ifdef __cplusplus
}
#endif

#endif  // RUNTIME_SYNTAX_TREE_H_
