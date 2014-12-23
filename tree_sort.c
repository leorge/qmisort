/*
 * tree_sort.c
 *
 *	Insertion sort with median node tree.
 *
 *  Created on: 2014/12/01
 *      Author: leo
 */

#include	<assert.h>
#include	<string.h>
#include 	"sort.h"

static int 	(*comp)(const void *, const void *);

typedef struct {
#ifdef	DEBUG
	const char	*element;	// to debug
#else
	const void	*element;	// adrs. of array element
#endif
	void	*left, *right;	// NODE *
	size_t	lnum, rnum;		// number of node
} NODE;

static void	**index, **idx;

static NODE	*add_min(NODE *tree, NODE *leaf);
static NODE	*add_max(NODE *tree, NODE *leaf);
static NODE *del_max(NODE *tree, NODE **max);
static NODE *del_min(NODE *tree, NODE **min);

static NODE *get_max(NODE *tree) {
	assert(tree);
	while(tree->rnum) tree = tree->right;
	return tree;
}

static NODE *get_min(NODE *tree) {
	assert(tree);
	while(tree->lnum) tree = tree->left;
	return tree;
}

#ifdef	DEBUG
static int	depth = -1;

static void dump_tree(NODE *tree) {
	assert(tree);
	NODE	*sub, *max, *min;
	if (tree->lnum) {
		sub = tree->left;
		if (sub->lnum) {
			min = get_min((NODE *)sub->left);
			fprintf(OUT, "%s <-", (char *)min->element);
			if (sub->lnum > 1) fprintf(OUT, "-");
		}
		fprintf(OUT, " %s ", (char *)sub->element);
		if (sub->rnum) {
			max = get_max(sub->right);
			if (sub->rnum > 1) fprintf(OUT, "-");
			fprintf(OUT, "-> %s", (char *)max->element);
		}
		else fprintf(OUT, "<-");
	}
	fprintf(OUT, " (%s) ", (char *)tree->element);
	if (tree->rnum) {
		sub = tree->right;
		if (sub->lnum) {
			min = get_min(sub->left);
			fprintf(OUT, "%s <-", (char *)min->element);
			if (sub->lnum > 1) fprintf(OUT, "-");
		}
		else fprintf(OUT, "->");
		fprintf(OUT, " %s ", (char *)sub->element);
		if (sub->rnum) {
			max = get_max(sub->right);
			if (sub->rnum > 1) fprintf(OUT, "-");
			fprintf(OUT, "-> %s", (char *)max->element);
		}
	}
	fprintf(OUT, "\n");
}

static void	isvalid(NODE *tree) {	// isvalid check
	const char *msg = "isvalid error :";
	const char *null_tree = "tree is NULL";
	bool	iserr = FALSE;
	if (trace_level >= TRACE_MOVE) {
		if (tree) {
			fprintf(OUT, "isvalid( %s ) ", (char *)tree->element);
			dump_tree(tree);
		}
		else fprintf(OUT, "isvalid(NULL)\n");
	}
	if (! tree) {	// == NULL
		fprintf(OUT, "%s tree is null\n", msg);
		iserr = TRUE;
	}
	else if (! tree->element) {
		fprintf(OUT, "element is NULL\n");
		iserr = TRUE;
	}
	else if (! strcmp(tree->element, "")) {
		fprintf(OUT, "element is vacant\n");
		iserr = TRUE;
	}
	else {
		if (tree->lnum) isvalid(tree->left);	// recursion
		else if (tree->left) {
			fprintf(OUT, "%s left %s lnum = %ld\n", msg, null_tree, tree->lnum);
			iserr = TRUE;
		}
		if (tree->rnum) isvalid(tree->right);	// recursion
		else if (tree->right) {
			fprintf(OUT, "%s right %s rnum = %ld\n", msg, null_tree, tree->rnum);
			iserr = TRUE;
		}
	}
	if (iserr) {
		dump_tree(tree);
		iserr = TRUE;	// dummy statement to pause
	}
}
#endif

static NODE *del_max(NODE *tree, NODE **max) {
	assert(tree);
	assert(max);
#ifdef DEBUG
	const char	*last = tree->element;
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "del_max(%s, **Max) from", (char *)tree->element);
		dump_tree(tree);
	}
#endif
	NODE	*rtn;
	if (tree->right) {	// tree is not maximum
		if (tree->lnum > tree->rnum) {	// rotate
			assert((tree->lnum == tree->rnum + 1));
			NODE *left = del_max(tree->left, &rtn);
			assert(left);		// != NULL
			assert(rtn);		// != NULL
			rtn->lnum = tree->lnum - 1;
			rtn->left = left;
			rtn->rnum = tree->rnum;
			rtn->right = del_max(add_min(tree->right, tree), max);
		}
		else {
			tree->right = del_max(tree->right, max);
			(rtn = tree)->rnum--;	// balanced
		}
	}
	else {
		*max = tree;		// tree is min.
		rtn = tree->left;	// right leaf or NULL(empty)
		if (rtn) {	// leaf
			assert(! rtn->rnum);
			assert(! rtn->right);
			assert(! rtn->lnum);
			assert(! rtn->left);
		}
		tree->lnum = 0; tree->left = NULL;	// clear right leaf
	}
#ifdef DEBUG
	if (rtn) isvalid(rtn);
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "del_max( %s, %s ) and ", last, (char *)(*max)->element);
		if (rtn) dump_tree(rtn); else fprintf(OUT, "(NULL)\n");
	}
#endif
	return	rtn;
}

static NODE *del_min(NODE *tree, NODE **min) {	// tree->right and tree->rnum will be modified.
	assert(tree);
	assert(min);
#ifdef DEBUG
	const char	*last = tree->element;
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "del_min( %s , **min ) from ", (char *)tree->element);
		dump_tree(tree);
	}
#endif
	NODE	*rtn;
	if (tree->left) {	// tree is not minimum
		if (tree->lnum < tree->rnum) {	// rotate
			assert((tree->lnum == tree->rnum - 1));
			NODE *right = del_min(tree->right, &rtn);
			assert(right);		// != NULL
			assert(rtn);		// != NULL
			rtn->rnum = tree->rnum - 1;
			rtn->right = right;
			rtn->lnum = tree->lnum;
			rtn->left = del_min(add_max(tree->left, tree), min);
		}
		else {
			tree->left = del_min(tree->left, min);
			(rtn = tree)->lnum--;	// balanced
		}
	}
	else {
		*min = tree;		// tree is min.
		rtn = tree->right;	// right leaf or NULL(empty)
		if (rtn) {	// leaf
			assert(! rtn->rnum);
			assert(! rtn->right);
			assert(! rtn->lnum);
			assert(! rtn->left);
		}
		tree->rnum = 0; tree->right = NULL;	// clear right leaf
	}
#ifdef DEBUG
	if (rtn) isvalid(rtn);
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "del_min( %s, %s ) and ", last, (char *)(*min)->element);
		if (rtn) dump_tree(rtn); else fprintf(OUT, " (NULL)\n");
	}
#endif
	return	rtn;
}

static NODE	*add_max(NODE *tree, NODE *leaf) {
	assert(leaf);
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "add_max(tree , %s ) to", (char *)leaf->element);
		if (tree) dump_tree(tree); else fprintf(OUT, "(NULL)\n");
	}
#endif
	leaf->rnum = leaf->lnum = 0; leaf->right = leaf->left = NULL;	// clear except element
	NODE	*rtn;
	if (! tree) rtn = leaf;
	else if (tree->rnum <= tree->lnum) {	// balanced
		(rtn = tree)->rnum++;
		rtn->right = add_max(tree->right, leaf);
	}
	else {	// unbalanced
		NODE *right = del_min(add_max(tree->right, leaf), &rtn);
		rtn->rnum = tree->rnum;
		rtn->right = right;
		rtn->lnum = tree->lnum + 1;
		rtn->left = add_max(tree->left, tree);
	}
#ifdef DEBUG
	isvalid(rtn);
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "add_max(tree, %s) --> ", (char *)leaf->element);
		dump_tree(rtn);
	}
#endif
	return	rtn;
}

static NODE	*add_min(NODE *tree, NODE *leaf) {
	assert(leaf);
#ifdef DEBUG
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "add_min(tree , %s ) to", (char *)leaf->element);
		if (tree) dump_tree(tree); else fprintf(OUT, " (NULL)\n");
	}
#endif
	leaf->rnum = leaf->lnum = 0; leaf->right = leaf->left = NULL;	// clear without element
	NODE	*rtn;
	if (! tree) rtn = leaf;
	else if (tree->lnum <= tree->rnum) {	// balanced
		(rtn = tree)->lnum++;
		rtn->left = add_min(tree->left, leaf);
	}
	else {	// unbalanced
		NODE *left = del_max(add_min(tree->left, leaf), &rtn);
		rtn->lnum = tree->lnum;
		rtn->left = left;
		rtn->rnum = tree->rnum + 1;
		rtn->right = add_min(tree->right, tree);
	}
#ifdef DEBUG
	isvalid(rtn);
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "add_min(tree, %s) result ", (char *)leaf->element);
		dump_tree(rtn);
	}
#endif
	return	rtn;
}

static NODE *insert(NODE *tree, NODE *node) {
#ifdef DEBUG
	qsort_called++;
	isvalid(tree);
	isvalid(node);
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "insert( %s , %s ) to   ", (char *)tree->element, (char *)node->element);
		dump_tree(tree);
	}
#endif
	NODE	*rtn = tree, save, *left, *right;
	int 	sign = comp(node->element, tree->element);
	if (sign == 0) {	// equal element
		if (tree->lnum < tree->rnum) {	// insert before tree
			tree->left = tree->lnum++ ? add_max(tree->left, node) : node;
		}
		else {
			tree->right = tree->rnum++ ? add_min(tree->right, node) : node;
		}
	}
	else if (sign < 0) {	// smaller element
		if (tree->lnum == 0) {	// there is no left node
			(rtn = tree)->lnum = 1;		// lnum++
			tree->left = node;			// add a node
		}
		else if (tree->lnum <= tree->rnum) {	// add node to left subtree
			tree->lnum++;
			tree->left = insert(tree->left, node);
		}
		else {	// unbalanced : tree->lnum > tree->rnum
			memcpy(&save, tree, sizeof(save));
			if (! tree->rnum) {	// right is empty.
				assert(! tree->right);
				assert(tree->lnum == 1);	// left is a leaf
				assert(tree->left);
				right = tree;
				tree->lnum = tree->rnum = 0; tree->left = tree->right = NULL;
				left = save.left;
				assert(! left->lnum);
				assert(! left->left);
				assert(! left->rnum);
				assert(! left->right);
				if (comp(node->element, left->element) >= 0) {
					(rtn = node)->left = left;
				}
				else {
					rtn = add_min(left, node);
				}
			}
			else {
				right = add_min(save.right, tree);	// get root of right subtree
				left = get_max(save.left);
				if (comp(node->element, left->element) >= 0) {	// node is greater than left subtree
					(rtn = node)->left = save.left;
				}
				else {	// insert node into left subtree
					left = del_max(save.left, &rtn);
					rtn->left = insert(left, node);
				}
			}
			rtn->lnum = save.lnum;
			rtn->rnum = save.rnum + 1; rtn->right = right;
		}
	}
	else if (tree->rnum == 0) {	// there is no right node for a greater element
		(rtn = tree)->rnum = 1;	// rtn == tree
		tree->right = node;
	}
	else if (tree->rnum <= tree->lnum) {	// balanced
		tree->rnum++;	// rtn == tree
		tree->right = insert(tree->right, node);
	}
	else {	// greater element and unbalanced
		memcpy(&save, tree, sizeof(save));
		if (! tree->lnum) {	// tree->left == NULL
			left = tree;
			tree->lnum = tree->rnum = 0; tree->left = tree->right = NULL;
			right = save.right;
			if (comp(node->element, right->element) <= 0) {
				(rtn = node)->right = right;
			}
			else {
				rtn = add_max(right, node);
			}
		}
		else {	// tree->Left != NULL
			left = add_max(save.left, tree);
			right = get_min(save.right);
			if (comp(node->element, right->element) <= 0) {	// insert node before right subtree
				(rtn = node)->right = save.right;
			}
			else {	// insert node into right subtree
				right = del_min(save.right, &rtn);
				rtn->right = insert(right, node);
			}
		}
		rtn->rnum = save.rnum;
		rtn->lnum = save.lnum + 1; rtn->left = left;
	}
#ifdef	DEBUG
	isvalid(rtn);
	if (trace_level >= TRACE_DUMP) {
		fprintf(OUT, "insert( %s , %s ) done ", (char *)tree->element, (char *)node->element);
		dump_tree(rtn);
	}
#endif
	return	rtn;
}

static void	sort(NODE *tree) {
#ifdef	DEBUG
	depth++;
#endif
	if (tree->lnum) {
		assert(tree->left);
		sort(tree->left);
	}
#ifdef	DEBUG
	if (trace_level >= TRACE_DUMP) fprintf(OUT, "%d : %s\n", depth, tree->element);
#endif
	*index++ = (void *)tree->element;
	if (tree->rnum) {
		assert(tree->right);
		sort(tree->right);
	}
#ifdef	DEBUG
	depth--;
#endif
}

void tree_sort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	NODE	*nodes;
	if (nmemb <= 1) {}
	else if ((idx = index = calloc(nmemb, sizeof(void *))) == NULL ||
			(nodes = calloc(nmemb, sizeof(NODE))) == NULL)	// filled with 0
		perror("calloc() error in tree_sort()");
	else {
		comp = compar;
		char	*element = base;	// adrs. of element
		NODE	*node = nodes;		// element of nodes table
		node->element = element; element += size;
		NODE	*root = node++;		// point first nodes
		for (size_t i = 1; i < nmemb; i++, node++, element += size) {	// build a tree
#ifdef	DEBUG
			if (trace_level >= TRACE_DUMP) fprintf(OUT, "Element[%ld] = %s\n", i, element);
#endif
			node->element = element;	// others are 0
			root = insert(root, node);
		}
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) dump_tree(root);
#endif
		sort(root);
		unindex(base, idx, nmemb, size);
		free(nodes); free(idx);
	}
#ifdef	DEBUG
		if (trace_level >= TRACE_DUMP) fprintf(OUT, "tree_sort() done.\n");
#endif
}
