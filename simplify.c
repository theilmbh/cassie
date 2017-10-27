/*
 * =====================================================================================
 *
 *       Filename:  simplify.c
 *
 *    Description:  Automatic Algebraic Simplification
 *
 *        Version:  1.0
 *        Created:  10/27/2017 11:44:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cmplr.h"
#include "simplify_rne.h"
#include "symbol.h"
#include "simplify.h"

int is_integer_val(Node * u, int val) 
{
    return ((u->type == INT) && (u->value == val));
}

int is_constant(Node * u)
{
    return ((u->type == INT) || (u->type == FRAC));
}

Node * automatic_simplify(Node * u)
{
    if ((u->type == INT) || (u->type == VAR)) {
        return u;
    }

    if (u->type == FRAC) {
        return simplify_rational_number(u);
    }

    int i;
    for (i = 0; i < u->n_args; i++) {
        /* Recursively simplify the arguments */
        u->args[i] = automatic_simplify(u->args[i]);
    }

    switch (u->type) {
        case BIN_OP_PLUS:
            return simplify_sum(u);
            break;
        case POWER:
            return simplify_power(u);
            break;
        case BIN_OP_TIMES:
            return simplify_product(u);
            break;
        case BIN_OP_DIVIDE:
            return simplify_quotient(u);
            break;
    }
}

Node * simplify_sum(Node * u)
{
    return u;
}

Node * simplify_product(Node * u)
{
    /* Check for undefined and 0 */
    int i;
    for (i = 0; i < u->n_args; i++) {
        if (u->args[i]->type == UNDEFINED) {
            return &UNDEFINED_NODE;
        }
        if (u->args[i]->type == INT && u->args[i]->value == 0) {
            return integer_node(0);
        }
    }

    /* Unary Product */
    if (u->n_args == 1) {
        return u->args[0];
    }

    Node * v = simplify_product_rec(u);
    if (v->n_args == 0) {
        return integer_node(1);
    }
    if (v->n_args == 1) {
        return v->args[0];
    }

    return v;
}

Node * simplify_product_rec(Node * u)
{
    if (u->n_args == 2) {
        if ((is_constant(u->args[0])) && (is_constant(u->args[1]))) {
            Node * P = simplify_RNE(u);
            if (P->value == 1) {
                u->n_args = 0;
                u->args[0] = NULL;
                u->args[1] = NULL;
                return u;
            } else { 
                u->n_args = 1;
                u->args[0] = P;
                u->args[1] = NULL;
                return u;
            }
        }

        if (is_integer_val(u->args[0], 1)) {
            u->n_args = 1;
            u->args[0] = u->args[1];
            u->args[1] = NULL;
            return u;
        }
        if (is_integer_val(u->args[1], 1)) {
            u->n_args = 1;
            u->args[1] = NULL;
            return u;
        }
    }
    return u;
}

Node * simplify_quotient(Node * u)
{
    return u;
}

Node * simplify_power(Node * u)
{
    Node * v = base(u);
    Node * w = exponent(u);

    /* Undefined Transformation */
    if ((v->type == UNDEFINED) || (w->type == UNDEFINED)) {
        return &UNDEFINED_NODE;
    }

    if (v->type == INT) {
        int val = v->value;
        if (val == 1) {
            return integer_node(1);
        }
        if (val == 0) {
            return integer_node(0);
        }
    }

    if (w->type == INT) {
        return simplify_integer_power(v, w);
    }
    
    return u;
}

Node * base(Node * u)
{
    if (u->type == BIN_OP_POWER) {
        return u->args[0];
    }

    if ((u->type == FRAC) || (u->type == INT)) {
        return &UNDEFINED_NODE;
    }

    return u;
}

Node * exponent(Node * u)
{
    if (u->type == BIN_OP_POWER) {
        return u->args[1];
    }

    if ((u->type == FRAC) || (u->type == INT)) {
        return &UNDEFINED_NODE;
    }

    return integer_node(1);
}

Node * simplify_integer_power(Node * v, Node * w)
{
    if ((v->type == INT) || (v->type == FRAC)) {
        return simplify_RNE(pow_node(v, w));
    }

    int n = w->value;
    if (n == 0) {
        return integer_node(1);
    }
    if (n == 1) {
        return v;
    }

    if (v->type == BIN_OP_POWER) {
        Node * r = base(v);
        Node * s = exponent(v);
        Node * p = simplify_product(times_node(s, w));
        if (p->type == INT) {
            return simplify_integer_power(r, p);
        }
        return pow_node(r, p);
    }
    /* SINTPOW-5 Goes here */
    if (v->type == BIN_OP_TIMES) {
        int i;
        for (i = 0; i < v->n_args; i++) {
            v->args[i] = simplify_integer_power(v->args[i], w);
        }
        return simplify_product(v);
    }

    return pow_node(v, w);
}
