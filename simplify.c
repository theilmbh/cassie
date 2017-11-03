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
#include <string.h>
#include <stdlib.h>
#include "cmplr.h"
#include "simplify_rne.h"
#include "symbol.h"
#include "simplify.h"
#include "derivative.h"

int tree_equals(Node * ast1, Node * ast2)
{
    /* Determines if two asts are equivalent, recursively */

    /* If the head is different type, then return false */
    if (ast1->type != ast2->type) {
        return 0;
    }

    if (ast1->n_args != ast2->n_args) {
        return 0;
    }

    /* If integers, check that value is the same */
    if (ast1->type == INT) {
        return (ast1->value == ast2->value);
    }

    if (ast1->type == FRAC) {
        return ((numerator_fun(ast1) == numerator_fun(ast2)) &&
                (denominator_fun(ast1) == denominator_fun(ast2)));
    }

    if (ast1->type == VAR) {
        if (!strncmp(ast1->name, ast2->name, MAXIDENT)) {
            return 1;
        } else {
            return 0;
        }
    }

    int i;
    for (i = 0; i < ast1->n_args; i++) {
        if (!tree_equals(ast1->args[i], ast2->args[i])) {
            return 0;
        }
    }
    return 1; 
}

Node * first(Node * u)
{
    /* Returns a node of the same type as u 
     * containing just the first argument */

    Node * out = u->args[0];
    return out;
}

Node * rest(Node * u)
{

    /* Returns a node, same type as u, conatining all but first arg */
    Node * out = times_node(NULL, NULL);
    int i;
    for (i=1; i < u->n_args; i++) {
        attach_argument(out, u->args[i]);
    }
    out->type = u->type;
    return out;
}

Node * adjoin(Node * f, Node * u)
{
    /* adds f as an argument to the front of u */
    Node * out = times_node(NULL, NULL);
    int i;
    attach_argument(out, f);
    for (i = 0; i < u->n_args; i++) {
        attach_argument(out, u->args[i]);
    }
    out->type = u->type;
    free(u);
    return out;
}

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
        case BIN_OP_MINUS:
            return simplify_minus(u);
            break;
        case FUNC:
            return simplify_func(u);
            break;
    }
}

Node * simplify_func(Node * u)
{
    if (!strncmp(u->name, "Plus", MAXIDENT) ) {
        Node * out = plus_node(NULL, NULL);
        for (int i = 0; i < u->n_args; i++) {
            attach_argument(out, u->args[i]);
        }
        free(u);
        return simplify_sum(out);
    }

    if (!strcmp(u->name, "D")) {
        return derivative_evaluate(u);
    }
    return u;
}

Node * simplify_sum(Node * u)
{
    int i;
    for (i = 0; i < u->n_args; i++) {
        if (u->args[i]->type == UNDEFINED) {
            return &UNDEFINED_NODE;
        }
    }

    if (u->n_args == 1) {
        return u->args[0];
    }

    Node * v = simplify_sum_rec(u);
    if (v->n_args == 0) {
        return integer_node(0);
    }
    if (v->n_args == 1) {
        return v->args[0];
    }
    return v;
}

Node * coefficient(Node * u)
{
    if (u->type == BIN_OP_TIMES) {
        if (is_constant(u->args[0])) {
            return u->args[0];
        } else {
            return integer_node(1);
        }
    }

    if (is_constant(u)) {
        return &UNDEFINED_NODE;
    }

    return integer_node(1);
}

Node * termlike(Node * u)
{
    if (u->type == BIN_OP_TIMES) {
        if (is_constant(u->args[0])) {
            return rest(u);
        } else {
            return u;
        }
    }

    if (is_constant(u)) {
        return &UNDEFINED_NODE;
    }

    return times_node(u, NULL);

}

Node * simplify_sum_rec(Node * u)
{
    /* 2 args, no sum */
    Node ** args = u->args;
    if ((u->n_args == 2) && (args[0]->type != BIN_OP_PLUS) &&
            (args[1]->type != BIN_OP_PLUS))
    {
        if (is_constant(args[0]) && is_constant(args[1])) {
            Node * P = simplify_RNE(u);
            if (is_integer_val(P, 0)) {
                return plus_node(NULL, NULL);
            } else {
                return plus_node(P, NULL);
            }
        }

        if (is_integer_val(args[0], 0)) {
            return plus_node(args[1], NULL);
        }

        if (is_integer_val(args[1], 0)) {
            return plus_node(args[0], NULL);
        }
        
       if (tree_equals(termlike(args[0]), termlike(args[1]))) {
           /* Like terms, collect coefficients */
           Node * a = coefficient(args[0]);
           Node * b = coefficient(args[1]);
           Node * S = simplify_sum(plus_node(a, b));
           Node * P = simplify_product(times_node(S, termlike(args[0])));
           if (is_integer_val(P, 0)) {
               return plus_node(NULL, NULL);
           } else {
               return plus_node(P, NULL);
           }
       } 

       if (ast_order(args[1], args[0])) {
           return plus_node(args[1], args[0]);
       } else {
           return u;
       }
    }

    if ((u->n_args == 2) && (args[0]->type == BIN_OP_PLUS || 
                args[1]->type == BIN_OP_PLUS)) 
    {
        Node * temp;
        if (args[1]->type != BIN_OP_PLUS) {
            temp = plus_node(args[1], NULL);
            return merge_sums(args[0], temp);
        }
        if (args[0]->type != BIN_OP_PLUS) {
            temp = plus_node(args[0], NULL);
            return merge_sums(temp, args[1]);
        }
        return merge_sums(args[0], args[1]);
    }

    Node * w = simplify_sum_rec(rest(u));
    Node * u1 = first(u);
    if (u1->type == BIN_OP_PLUS) {
        return merge_sums(u1, w);
    } else {
        return merge_sums(plus_node(u1, NULL), w);
    }

    return u;
}

Node * merge_sums(Node * p, Node * q)
{
    if (q->n_args == 0) {
        return p;
    }

    if (p->n_args == 0) {
        return q;
    }

    Node *p1, *q1, *h, *temp;
    p1 = first(p);
    q1 = first(q);
    h = simplify_sum_rec(plus_node(p1, q1));

    if (h->n_args == 0) {
        return merge_sums(rest(p), rest(q));
    }

    if (h->n_args == 1) {
        return adjoin(h->args[0], merge_sums(rest(p), rest(q)));
    }

    if (tree_equals(h->args[0], p1)) {
        temp = merge_sums(rest(p), q);
        return adjoin(p1, temp);
    } else {
        temp = merge_sums(p, rest(q));
        return adjoin(q1, temp);
    }
    
}

Node * simplify_minus(Node * u)
{
    Node * r = simplify_product(times_node(integer_node(-1), u->args[1]));
    return simplify_sum(plus_node(u->args[0], r));
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
    if (u->n_args == 2 &&
        u->args[0]->type != BIN_OP_TIMES &&
        u->args[1]->type != BIN_OP_TIMES) {
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
        /*  print_expression(base(u->args[0]));
        printf("\n");   
        print_expression(base(u->args[1]));
        printf("\n\n");*/

        if (tree_equals(base(u->args[0]), base(u->args[1]))) {
            Node * S = simplify_sum(plus_node(exponent(u->args[0]), 
                                              exponent(u->args[1])));
            Node * P = simplify_power(pow_node(base(u->args[0]), S));

            if (is_integer_val(P, 1)) {
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

        /* SPRDREC 1-4 */
        if (ast_order(u->args[1], u->args[0])) {
            Node * temp = u->args[1];
            u->args[1] = u->args[0];
            u->args[0] = temp;
            return u;
        }
        return u;
    }

    /* SPRDREC-2 */
    if ((u->n_args == 2) && (u->args[0]->type == BIN_OP_TIMES || 
                             u->args[1]->type == BIN_OP_TIMES   )) {
        Node * temp;
        if (u->args[1]->type != BIN_OP_TIMES ) {
            temp = times_node(u->args[1], NULL);
            return merge_products(u->args[0], temp);
        }
        if (u->args[0]->type != BIN_OP_TIMES) {
            temp = times_node(u->args[0], NULL);
            return merge_products(temp, u->args[1]);
        }

        return merge_products(u->args[0], u->args[1]);
    }
    
    Node * w = simplify_product_rec(rest(u));
    Node * u1 = first(u);
    if (u1->type == BIN_OP_TIMES) {
        return merge_products(u1, w);
    } else {
        return merge_products(times_node(u1, NULL), w);
    }
    return u;
}

Node * merge_products(Node * p, Node * q)
{
    if (q->n_args == 0) {
        return p;
    }

    if (p->n_args == 0) {
        return q;
    }

    Node *p1, *q1, *h, *temp;
    p1 = first(p);
    q1 = first(q);
    h = simplify_product_rec(times_node(p1, q1));

    if (h->n_args == 0) {
        return merge_products(rest(p), rest(q));
    }

    if (h->n_args == 1) {
        return adjoin(h->args[0], merge_products(rest(p), rest(q)));
    }

    if (tree_equals(h->args[0], p1)) {
        temp = merge_products(rest(p), q);
        return adjoin(p1, temp);
    } else {
        temp = merge_products(p, rest(q));
        return adjoin(q1, temp);
    }
}

Node * simplify_quotient(Node * u)
{
    /* rewrite a/b as a*b^-1 and simplify */
    Node * q = simplify_power(pow_node(u->args[1], integer_node(-1)));
    return simplify_product(times_node(u->args[0], q));
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

int constant_compare(Node * u, Node * v)
{
    /* computes u < v for u, v int or frac */

    int out = (numerator_fun(u)*denominator_fun(v)) < 
              (numerator_fun(v)*denominator_fun(u));
    return out;
}

int ast_order(Node * u, Node * v)
{
    /* The ast order relation on terms */
    int out; 
    /* Rule 1 - constants are in numerical order */
    if (is_constant(u) && is_constant(v)) {
        return constant_compare(u, v);
    }

    /* Rule 2 - Lexicographic order for variables */
    if (u->type == VAR && v->type == VAR) {
        if (strncmp(u->name, v->name, MAXIDENT) < 0) {
            return 1;
        } else {
            return 0;
        }
    }

    /* Rule 3 - Products and sums */
    if ((u->type == BIN_OP_TIMES && v->type == BIN_OP_TIMES) || 
        (u->type == BIN_OP_PLUS  && v->type == BIN_OP_PLUS )  ) {
        
        int m = u->n_args - 1;
        int n = v->n_args - 1; 
        int min_arg = (m < n) ? m : n;
        if (!tree_equals(u->args[m], v->args[n])) {
            return ast_order(u->args[m], v->args[n]);
        }

        int i;
        for (i = 0; i <= min_arg; i++) {
            if(!tree_equals(u->args[m-i], v->args[n-i])) {
                return ast_order(u->args[m-i], v->args[n-i]); 
            }
        }
        return (m < n);
    }

    /* Rule 4 - Powers */
    if (u->type == BIN_OP_POWER && v->type == BIN_OP_POWER) {
        if (!tree_equals(base(u), base(v))) {
            return ast_order(base(u), base(v));
        } else {
            return ast_order(exponent(u), exponent(v));
        }
    }

   /* Rule 7 - Fractions and other */
    if (is_constant(u) && !is_constant(v)) {
        return 1;
    }

    /* Rule 8 */
    if (u->type == BIN_OP_TIMES && v->type != BIN_OP_TIMES) {   
        Node * temp = times_node(v, NULL);
        out = ast_order(u, temp);
        free(temp);
        return out; 
    }

    /* Rule 9 */
    if (u->type == BIN_OP_POWER && v->type != BIN_OP_POWER) {
        Node * temp = pow_node(v, integer_node(1));
        out = ast_order(u, temp);
        free(temp);
        return out;
    }

    /* Rule 10 */
    if (u->type == BIN_OP_PLUS && v->type != BIN_OP_PLUS) {
        Node * temp = plus_node(v, NULL);
        out = ast_order(u, temp);
        free(temp);
        return out;
    }

    /* Rule 13 */
    return (!ast_order(v, u));
}
