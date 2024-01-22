/* keyword_tree: An implementation in keyword trees to allow for the
 * Aho-Corasick algorithm.
 *
 * Copyright (C) 2024 Andrew D. Smith
 *
 * Authors: Andrew D. Smith
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */


#include "keyword_tree.h"
#include "dynamic_array.h"


#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct kw_node {
  /* All kw_node instances need a "letter", but only those with path
     label corresponding to one of the patterns needs to have "num"
     set. I am using the convention that num > 0 to indicate that a
     node corresponds to the end of a pattern.
   */
  char letter;
  int num;
  struct kw_node *failure_link;
  struct kw_node *output_link;
  struct kw_node *parent;
  struct kw_node **child;
};


struct kw_tree {
  /* Any valid kw_tree will have an allocated root node, and if
     nothing else, it corresponds to a keyword set containing one
     keyword equal to the empty string. */
  struct kw_node *root;
};


kw_node *kw_node_init(const char letter) {
  kw_node *k = calloc(1, sizeof(kw_node));
  k->letter = letter;
  return k;
}


kw_node **init_children(void) {
  return calloc(alphabet_size, sizeof(kw_node *));
}


static inline bool
has_child(const kw_node *v, const char c) {
  return v->child != NULL && v->child[dna2int[(int)c]] != NULL;
}


void kw_node_free(kw_node *subtree_root) {
  if (subtree_root == NULL) return;

  if (subtree_root->child != NULL) {
    for (int i = 0; i < alphabet_size; ++i)
      kw_node_free(subtree_root->child[i]);
    free(subtree_root->child);
    subtree_root->child = NULL;
  }
  free(subtree_root);
  subtree_root = NULL;
}


int kw_node_subtree_size(kw_node *subtree_root) {
  int count = 0;
  if (subtree_root->child != NULL) {
    for (int i = 0; i < alphabet_size; ++i)
      if (subtree_root->child[i])
        count += kw_node_subtree_size(subtree_root->child[i]);
  }
  return count + 1;  // plus one for the current node
}


void kw_node_insert(kw_node *subtree_root, const char *pattern,
                    const int index) {

  // check for end of string here, and set the number for the pattern
  // if we have reached the end
  if (pattern[0] == '\0') {
    subtree_root->num = index;
    return;
  }

  // if needed, initialize the child pointers
  if (subtree_root->child == NULL)
    subtree_root->child = init_children();

  // get the numerical index for the letter
  const int i = dna2int[(int)pattern[0]];

  // if needed, initialize child for the corresponding letter
  if (subtree_root->child[i] == NULL)
    subtree_root->child[i] = kw_node_init(pattern[0]);

  // keep inserting suffixes recursively
  kw_node_insert(subtree_root->child[i], pattern + 1, index);
}


void kw_node_set_parents(kw_node *subtree_root) {

  // if we are at a leaf, return right away
  if (subtree_root->child == NULL) return;

  // iterate over child nodes, and set pointer of each
  for (int i = 0; i < alphabet_size; ++i)
    if (subtree_root->child[i] != NULL) {
      kw_node_set_parents(subtree_root->child[i]);
      subtree_root->child[i]->parent = subtree_root;
    }
}


kw_tree *kw_tree_init(void) {
  // allocate the tree and initialize the root node as empty
  kw_tree *t = calloc(1, sizeof(kw_tree));
  t->root = kw_node_init('\0');
  return t;
}


void kw_tree_free(kw_tree *t) {
  if (t == NULL) return;  // nothing to free

  // if the root is allocated, free the nodes recurisvely
  if (t->root != NULL)
    kw_node_free(t->root);

  // free the tree
  free(t);
  t = NULL;
}


void kw_tree_insert(kw_tree *t, const char *pattern, const int index) {
  // insert below the root
  kw_node_insert(t->root, pattern, index);
}


int kw_tree_size(kw_tree *t) {
  int size = 0;
  if (t->root != NULL)
    size = kw_node_subtree_size(t->root);
  return size;
}


void kw_node_set_failure_link(kw_node *root, kw_node *v) {
  // this function is analogous to the content of the main look in the
  // KMP preprocessing.

  // we already defaulted to the root, so we don't need to do anything
  // for nodes just below the root
  if (v->parent == root) return;

  const char c = v->letter;

  kw_node *w = v->parent->failure_link;
  while (!has_child(w, c) && w != root)
    w = w->failure_link;

  if (has_child(w, c))
    v->failure_link = w->child[dna2int[(int)c]];
  else
    v->failure_link = root;
}


void kw_tree_set_links(kw_tree *t) {

  // make sure all parent pointers are set
  kw_node_set_parents(t->root);

  // get the number of nodes in the tree
  const int n_nodes = kw_tree_size(t);

  // allocate an array for a queue to hold pointers to all nodes
  kw_node **queue = calloc(n_nodes, sizeof(kw_node*));

  // add every node to the queue so we can do level-order (BFS)
  // traversal of the tree when setting failure links
  int head = 0;
  int tail = 0;
  queue[tail++] = t->root;  // ==> queue[0]==t->root
  while (head != tail) {
    kw_node *top = queue[head++];
    if (top->child != NULL)
      for (int i = 0; i < alphabet_size; ++i)
        if (top->child[i] != NULL)
          queue[tail++] = top->child[i];
  }

  // the root will keep its NULL failure link value and all others
  // will point to a valid node, which will default to the root
  queue[0]->failure_link = NULL; // t->root->failure_link == NULL
  for (int i = 1; i < n_nodes; ++i)
    queue[i]->failure_link = queue[0];

  // set each failure link in amortized constant time
  for (int i = 1; i < n_nodes; ++i)
    kw_node_set_failure_link(t->root, queue[i]);

  // set each output link for each node in constant time each
  queue[0]->output_link = NULL; // t->root->output_link == NULL
  for (int i = 1; i < n_nodes; ++i) {
    // kw_node_set_output_link(queue[i]);
    kw_node *w = queue[i]->failure_link;
    if (w->num > 0)
      queue[i]->output_link = w;
    else
      queue[i]->output_link = w->output_link;
  }

  free(queue);
}


dynamic_array *kw_tree_search(const kw_tree *t, const char *T) {

  kw_node *w = t->root;

  const int n = strlen(T);

  dynamic_array *da = da_init();
  for (int i = 0; i < n; ++i) {

    while (w != t->root && !has_child(w, T[i]))
      w = w->failure_link;

    if (has_child(w, T[i]))
      w = w->child[dna2int[(int)T[i]]];

    if (w->num > 0)
      da_push(da, w->num);

    kw_node *p = w->output_link;
    while (p != NULL) {
      da_push(da, p->num);
      p = p->output_link;
    }
  }
  return da;
}
