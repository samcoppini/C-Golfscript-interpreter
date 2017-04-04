// set.c
// Contains functions for manipulating sets

#include <stdlib.h>
#include "golf.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

Set new_set() {
  Set set = {NULL};
  return set;
}

// Frees a tree node and all of its descendants
static void free_tree(TreeNode *node) {
  if (node == NULL)
    return;
  free_tree(node->left);
  free_tree(node->right);
  free(node);
}

void free_set(Set *set) {
  free_tree(set->root);
}

static TreeNode *new_node(Item *item) {
  TreeNode *node = malloc(sizeof(TreeNode));
  node->item = item;
  node->left = NULL;
  node->right = NULL;
  node->height = 0;
  return node;
}

// Performs a search through the set's tree and returns whether the given
// item exists in the set
bool set_has(Set *set, Item *item) {
  TreeNode *cur_node = set->root;
  while (cur_node != NULL) {
    int result = item_compare(item, cur_node->item);
    if (result < 0) {
      cur_node = cur_node->left;
    }
    else if (result > 0) {
      cur_node = cur_node->right;
    }
    else {
      return true;
    }
  }
  return false;
}

// Returns the height of a given node, even if it's null
static int get_height(TreeNode *node) {
  if (node == NULL)
    return 0;
  else
    return node->height;
}

// Sets the height of a tree node based on its children
static void set_height(TreeNode *node) {
  int left_height = get_height(node->left);
  int right_height = get_height(node->right);
  node->height = max(left_height, right_height) + 1;
}

// Rotates a tree node to the left, and returns the new top
// Only call this on a node with a right child
static TreeNode *single_rotate_left(TreeNode *cur_node) {
  TreeNode *new_root = cur_node->right;
  cur_node->right = new_root->left;
  new_root->left = cur_node;

  set_height(cur_node);
  set_height(new_root);

  return new_root;
}

// Rotates a tree node to the right, and returns the new top
// Only call this on a node with a left child
static TreeNode *single_rotate_right(TreeNode *cur_node) {
  TreeNode *new_root = cur_node->left;
  cur_node->left = new_root->right;
  new_root->right = cur_node;

  set_height(cur_node);
  set_height(new_root);

  return new_root;
}

// Performs a double left rotation, such that
//   a             b
//    \           / \
//     b   --->  a   c
//    /
//   c
// Only call this if node, node->right, and node->right->left are all non null
static TreeNode *double_rotate_left(TreeNode *cur_node) {
  cur_node->right = single_rotate_right(cur_node->right);
  return single_rotate_left(cur_node);
}

// Performs a double right rotation, such that
//    a            b
//   /            / \
//  b    --->    c   a
//   \
//    c
// Only call this if node, node->left, and node->left->right are all non null
static TreeNode *double_rotate_right(TreeNode *cur_node) {
  cur_node->left = single_rotate_left(cur_node->left);
  return single_rotate_right(cur_node);
}

// Inserts a node in an AVL tree, rotating the tree if necessary, returning
// the new top of this tree after a rotation
TreeNode *insert_node(TreeNode *cur_node, Item *item) {
  if (cur_node == NULL) {
    return new_node(item);
  }

  int result = item_compare(item, cur_node->item);

  if (result < 0) {
    cur_node->left = insert_node(cur_node->left, item);

    if (get_height(cur_node->left) - get_height(cur_node->right) == 2) {
      if (item_compare(item, cur_node->left->item) < 0)
        cur_node = single_rotate_right(cur_node);
      else
        cur_node = double_rotate_right(cur_node);
    }
  }
  else if (result > 0) {
    cur_node->right = insert_node(cur_node->right, item);

    if (get_height(cur_node->right) - get_height(cur_node->left) == 2) {
      if (item_compare(item, cur_node->right->item) > 0)
        cur_node = single_rotate_left(cur_node);
      else
        cur_node = double_rotate_left(cur_node);
    }
  }
  else {
    // The item we're adding is already in the tree
    return cur_node;
  }

  set_height(cur_node);

  return cur_node;
}

// Adds an item to a set
void set_add(Set *set, Item *item) {
  set->root = insert_node(set->root, item);
}
