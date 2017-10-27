/*
 * =====================================================================================
 *
 *       Filename:  simplify.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/27/2017 03:33:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include "cmplr.h"
#include "simplify_rne.h"

int is_integer_val(Node * u, int val);
int is_constant(Node * u);
Node * automatic_simplify(Node * u);
Node * simplify_sum(Node * u);
Node * simplify_product(Node * u);
Node * simplify_product_rec(Node * u);  
Node * simplify_quotient(Node * u);
Node * simplify_power(Node * u);
Node * base(Node * u);
Node * exponent(Node * u);
Node * simplify_integer_power(Node * v, Node * w);

#endif
