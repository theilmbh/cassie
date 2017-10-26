/*
 * =====================================================================================
 *
 *       Filename:  algebra.h
 *
 *    Description:  Declarations for algebraic manipulations of AST
 *
 *        Version:  1.0
 *        Created:  09/20/2017 03:22:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "cmplr.h"

/* Function Declarations */

/* Rewrite a - b as a + (-1)*b */
Node *rewrite_minus(Node *);

/* Move integer coefficients to the left of variables */
Node *reorder_coeff(Node *);

/* Apply Distributive Law to expand expressions */
Node *expand(Node *);

/* Evaluate the numerical value of an expression tree */
int evaluate(Node *);

#endif
