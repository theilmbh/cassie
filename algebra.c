/*
 * =====================================================================================
 *
 *       Filename:  algebra.c
 *
 *    Description:  Algebraic manipulations of Abstract Syntax Trees
 *
 *        Version:  1.0
 *        Created:  09/20/2017 03:23:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmplr.h"
#include "algebra.h"
#include "symbol.h"

/* Abstract Syntax Tree Rewriting Functions */
Node *rewrite_minus(Node * ast)
{
    /* Rewrite a - b as a + -1*b */
    if (ast == NULL) {
        return NULL;
    }

    if (ast->type == INT || ast->type == VAR) {
	return ast;
    }

    if (ast->type == BIN_OP_MINUS) {
	return plus_node(rewrite_minus(ast->left),
			 times_node(integer_node(-1),
				    rewrite_minus(ast->right)));
    } else {
	ast->left = rewrite_minus(ast->left);
	ast->right = rewrite_minus(ast->right);
	return ast;
    }
}

Node *reorder_coeff(Node * ast)
{
    /* Reorders coefficients so that integers are on left */
    Node *tmp;
    if (ast == NULL) {
        return NULL;
    }

    if (ast->type == INT || ast->type == VAR) {
	return ast;
    }

    if (ast->type == BIN_OP_TIMES
	&& ast->right->type == INT && ast->left->type != INT) {
	tmp = reorder_coeff(ast->left);
	ast->left = ast->right;
	ast->right = tmp;
	return ast;
    }
    ast->left = reorder_coeff(ast->left);
    ast->right = reorder_coeff(ast->right);
    return ast;
}

Node *expand(Node * ast)
{

    Node *out, *al;

    /* if node is int or var, just return it */
    if (ast->type == VAR || ast->type == INT) {
	return ast;
    }

    /* If node is add, expand children */
    if (ast->type == BIN_OP_PLUS || ast->type == BIN_OP_MINUS) {
	ast->left = expand(ast->left);
	ast->right = expand(ast->right);
	return ast;
    }

    /* if node is multiply, rewrite */
    if (ast->type == BIN_OP_TIMES) {
	if (ast->right->type == BIN_OP_PLUS) {
	    al = expand(ast->left);
	    out =
		plus_node(expand(times_node(al, expand(ast->right->left))),
			  expand(times_node
				 (al, expand(ast->right->right))));
	    return out;
	}
	if (ast->left->type == BIN_OP_PLUS) {
	    al = expand(ast->right);
	    out =
		plus_node(expand(times_node(expand(ast->left->left), al)),
			  expand(times_node
				 (expand(ast->left->right), al)));
	    return out;
	}

	/* Expand Children */
	ast->left = expand(ast->left);
	ast->right = expand(ast->right);

	/* check to see if we can distribute further */
	if (ast->left->type == BIN_OP_PLUS
	    || ast->right->type == BIN_OP_PLUS) {
	    return expand(ast);
	}
	/* return expanded node */
	return ast;
    }

    /* If we have a parenthetical expression,
     * expand the right branch.  The left is NULL */
    if (ast->type == PAREN) {
        ast->right = expand(ast->right);
    }

    /* fall through */
    return ast;
}

int evaluate(Node * ast)
{
    /* evaluate an expression */
    int v1;
    Symbol *sym;
    if (ast->type == VAR) {
	if(sym = find_symbol(ast)) {
            v1 = evaluate(sym->expr);
        }

    }

    if (ast->type == INT) {
	return ast->value;
    }

    if (ast->type == PAREN) {
        return evaluate(ast->right);
    }

    switch (ast->type) {
    case BIN_OP_PLUS:
	v1 = evaluate(ast->left) + evaluate(ast->right);
	break;
    case BIN_OP_TIMES:
	v1 = evaluate(ast->left) * evaluate(ast->right);
	break;
    case BIN_OP_MINUS:
	v1 = evaluate(ast->left) - evaluate(ast->right);
	break;
    }
    return v1;
}


