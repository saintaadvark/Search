#include <stdbool.h>
#include <assert.h>
#define IS_LEFTCHILD 1
#define IS_RIGHTCHILD 0
enum nodetype {treeroot, lchild, rchild};

enum nodetype classifychild(vertex* child)
{
	vertex* parent = child->parent;
	if (parent) {
		if (child == parent->left)
			return lchild;

		else if (child == parent->right)
			return rchild;
	}
	return treeroot;
}

void updateparent(vertex* newchild, vertex* oldchild)
{
	assert(newchild->parent == oldchild->parent);
	switch (classifychild(oldchild)) {
		case treeroot:
			root = newchild;
			break;					
		case lchild:
			newchild->parent->left = newchild;
			break;
		case rchild:
			newchild->parent->right = newchild;
			break;
		default: 
			assert(0);
			break;
	}
}
