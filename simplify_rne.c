/*
 * =====================================================================================
 *
 *       Filename:  simplify_rne.c
 *
 *    Description:  Simplify Rational Number Expressions
 *
 *        Version:  1.0
 *        Created:  10/26/2017 02:23:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

Node * simplify_rational_number(Node * u)
{
    Node * out = u;
    /* Simplifies an integer or quotient of integers (fraction) */
    if (!u) {
        return u;
    }

    if (u->type == INT) {
        return u;
    }

    if (u->type == FRAC) {
        Node * n = u->args[0];
        Node * d = u->args[1];
        if ((n->type != INT) || (d->type != INT)) {
            return u;
        } 

        if (n->value % d->value == 0) {
            out = integer_node(n->value / d->value)
            free(u);
            free(n);
            free(d);
        } else {
            int g = int_gcd(n->value, d->value);
            if (d->value > 0) {
                out = frac_node(integer_node(n->value / g),
                                  integer_node(d->value / g));
            } else {
                out = frac_node(integer_node(-1*(n->value) / g), 
                                               -1*(d->value) / g);
            }
            free(u);
            free(n);
            free(d);
        }
    }
    return out;

}

Node * simplify_RNE(Node * u)
{
    Node * out = simplify_RNE_rec(u);
    if (out->type == UNDEFINED) {
        return &UNDEFINED;
    }
    return simplify_rational_number(out);
}

Node * simplify_RNE_rec(Node * u)
{
    Node *v, *w, *out;

    if (u->type == INT) {
        return u;
    }
    
    if (u->type == FRAC) {
        if (u->args[1]->value == 0) {
            return &UNDEFINED;
        }
        return u;
    }

    if (u->n_args == 1) {
        out = simplify_RNE_rec(u->args[0]);
        if (out->type == UNDEFINED) {
            return &UNDEFINED;
        }
        return out;
    }

    if (u->n_args == 2) {
        switch (u->type) {
            case BIN_OP_PLUS:
                v = simplify_RNE_rec(u->args[0]);
                w = simplify_RNE_rec(u->args[1]);
                return evaluate_sum(v, w);
                break;
            case BIN_OP_TIMES:
                v = simplify_RNE_rec(u->args[0]);
                w = simplify_RNE_rec(u->args[1]);
                return evaluate_product(v, w);
                break;
            case BIN_OP_DIVIDE:
                v = simplify_RNE_rec(u->args[0]);
                w = simplify_RNE_rec(u->args[1]);
                return evaluate_quotient(v, w);
                break;
        }
    }

    if (u->type = BIN_OP_POWER) {
        v = simplify_RNE_rec(u->args[0]);
        return evaluate_power(v, u->args[1]);
    }

}

int numerator_fun(Node * u)
{
    if (u->type == INT) {
        return u->value;
    }

    if (u->type == FRAC) {
        return u->args[0]->value;
    }
}

int denominator_fun(Node * u)
{
    if (u->type == INT) {
        return u->value;
    }

    if (u->type == FRAC) {
        return u->args[1]->value;
    }
}

Node * evaluate_quotient(Node * v, Node * w)
{
    if (numerator_fun(w) == 0) {
        return &UNDEFINED;
    }
    Node * num;
    Node * denom;

    num = integer_node(numerator_fun(v)*denominator_fun(w));
    denom = integer_node(numerator_fun(w)*denominator_fun(v));
    return frac_node(num, denom);
}

Node * evaluate_product(Node * v, Node * w)
{

}

Node * evaluate_sum(Node * v, Node * w)
{
    
}

Node * evaulate_power(Node * v, Node * w)
{

}
