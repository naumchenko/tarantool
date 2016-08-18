/*
 * *No header guard*: the header is allowed to be included twice
 * with different sets of defines.
 */
/*
 * Copyright 2010-2016, Tarantool AUTHORS, please see AUTHORS file.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the
 *		following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *		copyright notice, this list of conditions and the following
 *		disclaimer in the documentation and/or other materials
 *		provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


/**
 * Additional user defined name that appended to prefix 'heap'
 *	for all names of structs and functions in this header file.
 * All names use pattern: heap<HEAP_NAME>_<name of func/struct>
 * May be empty, but still have to be defined (just #define HEAP_NAME)
 * Example:
 * #define HEAP_NAME _test
 * ...
 * struct heap_test_core some_heap;
 * heap_test_init(&some_heap, ...);
 */
#ifndef HEAP_NAME
#error "HEAP_NAME must be defined"
#endif

/**
 * Data comparing function. Takes 3 parameters - heap, node1, node2,
 * where heap is pointer onto core structure and node1, node2
 * are two pointers on nodes in your structure.
 * For example you have such type:
 *	 struct my_type {
 *	 	int value;
 *	 	struct heap_<HEAP_NAME>_node vnode;
 *	 };
 * Then node1 and node2 will be pointers on field vnode of two
 * my_type instances.
 * The function below is example of valid comparator by value:
 *
 * int test_type_less(const struct heap_<HEAP_NAME>_core *heap,
 *			const struct heap_<HEAP_NAME>_node *a,
 *			const struct heap_<HEAP_NAME>_node *b) {
 *
 *	const struct my_type *left = (struct my_type *)((char *)a -
 *					offsetof(struct my_type, vnode));
 *	const struct my_type *right = (struct my_type *)((char *)b -
 *					offsetof(struct my_type, vnode));
 *	return left->value < right->value;
 * }
 *
 * HEAP_LESS is less function that is important!
 */
#ifndef HEAP_LESS
#error "HEAP_LESS must be defined"
#endif

/**
 * Tools for name substitution:
 */
#ifndef CONCAT4
#define CONCAT4_R(a, b, c, d) a##b##c##d
#define CONCAT4(a, b, c, d) CONCAT4_R(a, b, c, d)
#endif

#ifdef _
#error '_' must be undefinded!
#endif
#ifndef HEAP
#define HEAP(name) CONCAT4(heap, HEAP_NAME, _, name)
#endif

/* Structures. */

/**
 * Main structure for holding heap.
 */
struct HEAP(core) {
 struct HEAP(node) *root; /* pointer onto root of the heap */
};

/**
 * Heap entry structure.
 */
struct HEAP(node) {
 uint64_t size; /*size of subtree*/
 struct HEAP(node) *left;
 struct HEAP(node) *right;
 struct HEAP(node) *parent;
};

/**
 * Heap iterator structure.
 */
struct HEAP(iterator) {
	struct HEAP(node) *current_node;
	int depth; // current depth in tree
	uint64_t mask; // mask of left/right choices
};


/* Extern API that is the most usefull part. */

/**
 * Init heap.
 */
 static inline void
 HEAP(init)(struct HEAP(core) *heap);

/**
 * Returns size of according to root.
 */
static inline uint64_t
HEAP(size)(struct HEAP(core) *heap);

/**
 * Returns root node of heap.
 */
static inline struct HEAP(node) *
HEAP(get_root)(struct HEAP(node) *node);

/**
 * Erase min value.
 */
static inline struct HEAP(node) *
HEAP(pop)(struct HEAP(core) *heap);

/**
 * Insert value.
 */
static inline void
HEAP(insert)(struct HEAP(core) *heap, struct HEAP(node) *nd);

/**
 * Delete node from heap.
 */
static inline void
HEAP(delete)(struct HEAP(core) *heap, struct HEAP(node) *value_node);

/**
 * Heapify tree after update of value under value_node pointer.
 */
static inline void
HEAP(update)(struct HEAP(core) *heap, struct HEAP(node) *value_node);

/**
 * Heap iterator init.
 */
static inline void HEAP(iterator_init)
(struct HEAP(core) *heap, struct HEAP(iterator) *it);

/**
 * Heap iterator next.
 */
static inline struct HEAP(node) *
HEAP(iterator_next) (struct HEAP(iterator) *it);

/**
 * Debug functions. They are usually useless,
 * but aplicable for testing.
 */

/**
 * Debug function. Check heap invariants for pair node, parent.
 */
static inline bool
HEAP(check_local_invariants) (struct HEAP(core) *heap,
				struct HEAP(node) *node,
				struct HEAP(node) *parent);

/*
 * Debug function. Check heap invariants for all nodes.
 */
static inline bool
HEAP(check_invariants)(struct HEAP(core) *heap,
			struct HEAP(node) *node,
			struct HEAP(node) *parent);


/* Routines. Functions below are useless for ordinary user. */

/**
 * Init heap node.
 */
static inline void
HEAP(init_node)(struct HEAP(node) *node);

/**
 * Swap two parent and son.
 */
static inline void
HEAP(swap_parent_and_son)(struct HEAP(core) *heap,
				struct HEAP(node) *parent,
				struct HEAP(node) *son);

/**
 * Update parent field of children.
 */
static inline void
HEAP(push_info_to_children)(struct HEAP(node) *node);

/**
 * Update left or right field of parent.
 */
static inline void
HEAP(push_info_to_parent)(struct HEAP(node) *parent, struct HEAP(node) *son);

/**
 * Cut leaf. Node is a pointer to leaf.
 */
static void
HEAP(cut_leaf)(struct HEAP(node) *node);

/**
 * Get first not full, i.e. first node with less that 2 sons.
 */
static struct HEAP(node)*
HEAP(get_first_not_full)(struct HEAP(node) *root);

/**
 * Check that current tree is full binary tree.
 */
static bool
HEAP(is_full)(const struct HEAP(node) *root);

/**
 * Get last node, i.e. the most right in bottom layer.
 */
static struct HEAP(node) *
HEAP(get_last)(struct HEAP(node) *root);

/**
 * Sift up current node.
 */
static void
HEAP(sift_up)(struct HEAP(core) *heap, struct HEAP(node) *node);

/**
 * Sift down current node.
 */
static void
HEAP(sift_down)(struct HEAP(core) *heap, struct HEAP(node) *node);

/**
 * Increment size in every node on path to root.
 */
static void
HEAP(inc_size)(struct HEAP(node) *node);

/**
 * Decrement size in every node on path to root.
 */
static void
HEAP(dec_size)(struct HEAP(node) *node);


/* Function defenitions */

/**
 * Init heap node.
 */
static inline void
HEAP(init_node)(struct HEAP(node) *node) {
	node->size = 1;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
}

/**
 * Init heap.
 */
 static inline void
 HEAP(init)(struct HEAP(core) *heap) {
	heap->root = NULL;
 }

/**
 * Returns size of according to root.
 */
static inline uint64_t
HEAP(size)(struct HEAP(core) *heap) {
	assert(heap);
	return (heap->root ? heap->root->size : 0);
}

/**
 * Returns size of subtree according to choldren sizes.
 */
static inline uint64_t
HEAP(get_size_from_children)(struct HEAP(node) *node) {
	if (node == NULL) {
		return 0;
	}
	uint64_t size = 1;
	if (node->left)
		size += node->left->size;
	if (node->right)
		size += node->right->size;

	return size;
}

/**
 * Returns root node of heap.
 */
static inline struct HEAP(node) *
HEAP(get_root)(struct HEAP(node) *node) {
	assert(node);
	while (node->parent) {
		node = node->parent;
	}

	return node;
}

/**
 * Check that current tree is full binary tree.
 */
static inline bool
HEAP(is_full)(const struct HEAP(node) *root) {
	assert(root);
	/*check that size + 1 is 2^n for some n*/
	return ((root->size + 1) & root->size) == 0;
}

/**
 * Update parent field of children.
 */
static inline void
HEAP(push_info_to_children)(struct HEAP(node) *node) {
	assert(node);
	if (node->left)
		node->left->parent = node;

	if (node->right)
		node->right->parent = node;

}

/**
 * Update left or right field of parent.
 */
static inline void
HEAP(push_info_to_parent)(struct HEAP(node) *parent, struct HEAP(node) *son) {
	assert(parent);
	assert(son);
	if (!parent->parent)
		return;
	struct HEAP(node) *pparent = parent->parent;
	if (pparent->left == son)
		pparent->left = parent;
	else
		pparent->right = parent;
}

/**
 * Cut leaf. Node is a pointer to leaf.
 */
static inline void
HEAP(cut_leaf)(struct HEAP(node) *node) {
	assert(node);
	assert(node->left == NULL);
	assert(node->right == NULL);

	if (node->parent == NULL)
		return;

	struct HEAP(node) *parent = node->parent;
	if (parent->left == node) {
		parent->left = NULL;
		return;
	}

	if (parent->right == node) {
		parent->right = NULL;
		return;
	}

	/* unreachable */
	assert(false);
}

/**
 * Swap two connected(i.e parent and son) nodes.
 */
static inline void
HEAP(swap_parent_and_son)(struct HEAP(core) *heap,
				struct HEAP(node) *parent,
				struct HEAP(node) *son) {
	assert(parent && son && son->parent == parent);
	struct HEAP(node) tmp = *parent;
	*parent = *son;
	*son = tmp;
	if (son->left == son)
	  son->left = parent;
	else
	  son->right = parent;
	HEAP(push_info_to_children)(parent);
	HEAP(push_info_to_children)(son);
	HEAP(push_info_to_parent)(son, parent);

	if (son->parent == NULL)
		heap->root = son; /* save root */
}

/**
 * Get first not full, i.e. first node with less that 2 sons.
 */
static struct HEAP(node) *
HEAP(get_first_not_full)(struct HEAP(node) *root) {
	assert(root);

	bool is_full_left, is_full_right;
	while (root->right) {
		is_full_left = HEAP(is_full)(root->left);
		is_full_right = HEAP(is_full)(root->right);

		/* heap is always complete tree */
		assert(is_full_left || is_full_right);

		if (is_full_left && is_full_right) {
			if (root->left->size == root->right->size)
				root = root->left;
			else
				root = root->right;
		} else {
			if (is_full_left)
				root = root->right;
			else
				root = root->left;
		}
	}

	return root;
}

/**
 * Get last node, i.e. the most right in bottom layer.
 */
static struct HEAP(node) *
HEAP(get_last)(struct HEAP(node) *root) {
	assert(root);

	bool is_full_left, is_full_right;
	while (root->right) {
		is_full_left = HEAP(is_full)(root->left);
		is_full_right = HEAP(is_full)(root->right);

		assert(is_full_left || is_full_right); /* heap is always complete tree */

		if (is_full_left && is_full_right) {
			if (root->left->size == root->right->size)
				root = root->right;
			else
				root = root->left;
		} else {
			if (is_full_left)
				root = root->right;
			else
				root = root->left;
		}
	}

	return (root->left ? root->left : root);
}


/**
 * Sift up current node.
 */
static void
HEAP(sift_up)(struct HEAP(core) *heap, struct HEAP(node) *node) {
	(void) heap;
	assert(node);
	struct HEAP(node) *parent = node->parent;
	while (parent && HEAP_LESS(heap, node, parent)) {
		HEAP(swap_parent_and_son)(heap, parent, node);
		parent = node->parent;
	}
}

/**
 * Sift down current node.
 */
static void
HEAP(sift_down)(struct HEAP(core) *heap, struct HEAP(node) *node) {
	(void) heap;
	assert(node);
	struct HEAP(node) *left = node->left;
	struct HEAP(node) *right = node->right;
	struct HEAP(node) *min_son;
	if (left && right)
		min_son = (HEAP_LESS(heap, left, right) ? left : right);

	while (left && right && HEAP_LESS(heap, min_son, node)) {
		HEAP(swap_parent_and_son)(heap, node, min_son);
		left = node->left;
		right = node->right;

		if (left && right)
			min_son = (HEAP_LESS(heap, left, right) ? left : right);
	}

	if ((left || right) && HEAP_LESS(heap, left, node)) {
		assert(left); /*left is not null because heap is complete tree*/
		assert(right == NULL);
		if (HEAP_LESS(heap, left, node))
			HEAP(swap_parent_and_son)(heap, node, left);
	}
}

/**
 * Increment size in every node on path to root.
 */
static void
HEAP(inc_size)(struct HEAP(node) *node) {
	while (node->parent) {
		node = node->parent;
		node->size++;
	}
}

/**
 * Decrement size in every node on path to root.
 */
static void
HEAP(dec_size)(struct HEAP(node) *node) {
	while (node->parent) {
		node = node->parent;
		node->size--;
	}
}


/**
 * Insert value.
 */
static inline void
HEAP(insert)(struct HEAP(core) *heap, struct HEAP(node) *node) {
	(void) heap;
	assert(heap);

	if (node == NULL)
		return;

	HEAP(init_node)(node);
	if (heap->root == NULL) {
		/* save new root */
		heap->root = node;
		return;
	}

	struct HEAP(node) *first_not_full = HEAP(get_first_not_full)(heap->root);
	node->parent = first_not_full;
	if (first_not_full->left)
		first_not_full->right = node;
	else
		first_not_full->left = node;
	HEAP(inc_size)(node); /* update sizes */

	HEAP(sift_up)(heap, node); /* heapify */

}

/**
 * Erase min value. Returns delete value.
 */
static inline struct HEAP(node) *
HEAP(pop)(struct HEAP(core) *heap) {
	(void) heap;
	assert(heap);
	struct HEAP(node) *res = heap->root;
	HEAP(delete)(heap, heap->root);
	return res;
}

/*
 * Delete node from heap.
 */
static inline void
HEAP(delete)(struct HEAP(core) *heap, struct HEAP(node) *value_node) {
	(void) heap;
	assert(heap);
	struct HEAP(node) *root = heap->root;
	struct HEAP(node) *last_node = HEAP(get_last)(root);

	/* check that we try to delete last node */
	if (last_node == root) {
		assert(last_node == value_node);
		/* save new root */
		heap->root = NULL;
		return;
	}

	assert(last_node->left == NULL);
	assert(last_node->right == NULL);

	HEAP(dec_size)(last_node); /* update sizes */

	/* cut leaf */
	HEAP(cut_leaf)(last_node);

	/* insert last_node as root */
	last_node->parent = value_node->parent;
	last_node->left = value_node->left;
	last_node->right = value_node->right;
	last_node->size = HEAP(get_size_from_children)(last_node);
	if (last_node->parent == NULL)
		heap->root = last_node; /* save root */

	HEAP(push_info_to_parent)(last_node, value_node);
	HEAP(push_info_to_children)(last_node);

	/* delte root from tree */
	value_node->left =  NULL;
	value_node->right =  NULL;
	value_node->parent = NULL;
	value_node->size = 1;

	/*heapify */
	HEAP(update)(heap, last_node);
}

/**
 * Heapify tree after update of value under value_node pointer.
 */
static inline void
HEAP(update)(struct HEAP(core) *heap, struct HEAP(node) *value_node) {
	(void) heap;
	assert(heap);
	/* heapify */
	HEAP(sift_down)(heap, value_node);
	HEAP(sift_up)(heap, value_node);
}


/**
 * Debug function. Check heap invariants for pair node, parent.
 */
static inline bool
HEAP(check_local_invariants) (struct HEAP(core) *heap,
				struct HEAP(node) *parent,
				struct HEAP(node) *node) {
	(void) heap;
	assert(node);

	if (parent != node->parent)
		return false;
	if (parent && parent->left != node && parent->right != node)
		return false;

	if (node->size != HEAP(get_size_from_children)(node))
		return false;
	if (node->right && node->left &&
			!HEAP(is_full)(node->right) &&
			!HEAP(is_full)(node->left))
		return false;

	if (node->left && HEAP_LESS(heap, node->left, node))
		return false;

	if (node->right && HEAP_LESS(heap, node->right, node))
		return false;

	return true;
}

/**
 * Debug function. Check heap invariants for all nodes.
 */
static inline bool
HEAP(check_invariants)(struct HEAP(core) *heap,
			struct HEAP(node) *parent,
			struct HEAP(node) *node) {
	(void) heap;
	if (!node)
		return true;

	if (!HEAP(check_local_invariants)(heap, parent, node))
		return false;

	bool check_left = HEAP(check_invariants)(heap, node, node->left);
	bool check_right = HEAP(check_invariants)(heap, node, node->right);

	return (check_right && check_left);
}

/**
 * Heap iterator init.
 */
static inline void HEAP(iterator_init)
(struct HEAP(core) *heap, struct HEAP(iterator) *it) {
	(void) heap;
	it->current_node = heap->root;
	it->mask = 0;
	it->depth = 0;
}

/**
 * Heap iterator next.
 */
static inline struct HEAP(node) *
HEAP(iterator_next) (struct HEAP(iterator) *it) {
	struct HEAP(node) *cnode = it->current_node;

	if (!cnode)
		return NULL;

	if (cnode && cnode->left) {
		it->mask = it->mask & (~ (1 << it->depth));
		it->depth++;
		it->current_node = cnode->left;
		return cnode;
	}

	while (((it->mask & (1 << it->depth)) ||
			it->current_node->right == NULL) &&
			it->depth) {
		it->depth--;
		it->current_node = it->current_node->parent;
	}

	if (it->depth == 0 && (it->mask & 1 || it->current_node == NULL)) {
		it->current_node = NULL;
		return cnode;
	}

	it->current_node = it->current_node->right;
	it->mask = it->mask | (1 << it->depth);
	it->depth++;

	return cnode;
}
