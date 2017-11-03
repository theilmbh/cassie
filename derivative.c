/*
 * =====================================================================================
 *
 *       Filename:  derivative.c
 *
 *    Description:  Compute Derivatives of expressions
 *
 *        Version:  1.0
 *        Created:  11/03/2017 01:41:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmplr.h"
#include "simplify.h"
#include "derivative.h"

Node * derivative_node(Node * u, Node * x)
{
    Node * d = calloc(1, sizeof(Node));
    d->type = FUNC;
    strcpy(d->name, "D");
    attach_argument(d, u);
    attach_argument(d, x);
    return d;
}

Node * derivative_evaluate(Node * d)
{
    /* Check types */
    Node * u = d->args[0];
    Node * x = d->args[1];

    if (x->type != VAR) {
        printf("Derivative:  Must differentiate with respect to a variable\n");
        return d;
    }
    Node * out = derivative_do_evaluate(u, x);

    return out;
}

Node * derivative_do_evaluate(Node * u, Node * x)
{
    Node * out;
    char * d_var_name = x->name;
    int i;
    if (is_constant(u)) {
        return integer_node(0);
    }

    if (u->type == VAR) {
        if (!strcmp(d_var_name, u->name)) {
            return integer_node(1);
        } else {
            return integer_node(0);
        }
    }


    if (u->type == BIN_OP_PLUS) {
        out = plus_node(NULL, NULL);
        for (i = 0; i < u->n_args; i++) {
            attach_argument(out, derivative_do_evaluate(u->args[i], x));
        }
        return automatic_simplify(out);
    }

    if (u->type == BIN_OP_POWER) {
        if (is_constant(u->args[1])) {
            out = times_node(u->args[1], pow_node(u->args[0], minus_node(u->args[1], integer_node(1))));
            out = times_node(derivative_do_evaluate(u->args[0], x), out);
            return automatic_simplify(out);
        } else {
            out = derivative_node(u, x);
        }
    }

    if (u->type == BIN_OP_TIMES) {
        if (u->n_args == 0) {
            return u;
        }
        if (u->n_args == 1) {
            return automatic_simplify(derivative_do_evaluate(u->args[0], x));
        }
        Node * du = derivative_do_evaluate(u->args[0], x);
        Node * dv = derivative_do_evaluate(rest(u), x); 
        return automatic_simplify(plus_node(times_node(du, rest(u)), times_node(u->args[0], dv)));
    }

    return out;

}
