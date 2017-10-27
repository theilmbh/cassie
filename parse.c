#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmplr.h"
#include "algebra.h"
#include "symbol.h"

/* Current Token */
Token *tok;

/* Source file */
FILE *source;

void match(token_t type)
{
    /* Accept a token matching a specific type */
    if (tok->type == type) {
	read_one_token(tok, source);
    } else {
	printf("Syntax Error\n");
	exit(-1);
    }
}

/* ABSTRACT SYNTAX TREE FUNCTIONS */

void attach_argument(Node * N, Node * arg)
{
    N->args[N->n_args] = arg;
    N->n_args++;
}

/* Negate a node
 * by multiplying by -1 */
Node *negate(Node * n)
{
    return times_node(integer_node(-1), n);
}

Node *paren_node(Node * expr) 
{
    Node *out = malloc(sizeof(Node));
    out->type = PAREN;
    out->value = 0;
    out->left = NULL;
    out->right = expr;
    return out;
}

Node *plus_node(Node * l, Node * r)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = BIN_OP_PLUS;
    out->value = 0;
    out->left = l;
    out->right = r;
    attach_argument(out, l);
    attach_argument(out, r);
    return out;
}

/* Node *minus_node(Node * l, Node * r)
{
    Node *out = malloc(sizeof(Node));
    out->type = BIN_OP_MINUS;
    out->value = 0;
    out->left = l;
    out->right = r;
    return out;
}*/

Node *minus_node(Node * l, Node * r)
{
    return plus_node(l, times_node(integer_node(-1), r));
    if (r->type == BIN_OP_PLUS || r->type == BIN_OP_MINUS ) {
        return plus_node(l, plus_node(negate(r->left), r->right));
    } else {
        return plus_node(l, negate(r));
    }
}

Node *times_node(Node * l, Node * r)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = BIN_OP_TIMES;
    out->value = 0;
    out->left = l;
    out->right = r;
    attach_argument(out, l);
    attach_argument(out, r);
    return out;
}

Node *divide_node(Node * l, Node * r)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = BIN_OP_DIVIDE;
    out->value = 0;
    out->left = l;
    out->right = r;
    attach_argument(out, l);
    attach_argument(out, r);
    return out;
}

Node *frac_node(Node * n, Node * d)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = FRAC;
    attach_argument(out, n);
    attach_argument(out, d);
    return out;
}

Node *pow_node(Node * l, Node * r)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = BIN_OP_POWER;
    out->value = 0;
    out->left = l;
    out->right = r;
    attach_argument(out, l);
    attach_argument(out, r);
    return out;
}


Node *integer_node(int val)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = INT;
    out->value = val;
    out->left = NULL;
    out->right = NULL;
    return out;
}

Node *var_node(char *var_name)
{
    Node *out = calloc(1, sizeof(Node));
    out->type = VAR;
    strcpy(out->name, var_name);
    out->left = NULL;
    out->right = NULL;
    return out;
}

/* Grammar functions corresponding to Nonterminals */
Node *factor()
{
    // printf("factor\n");
    Node *out = NULL;
    /* Just a plain integer */
    if (tok->type == INTEGER) {
	out = integer_node(tok->val.int_val);
	match(INTEGER);
	return out;
    } else if (tok->type == L_PAREN) {
	/* parenthetical expression */
	match(L_PAREN);
	out = expression();
	match(R_PAREN);
	return out;
    } else if (tok->type == IDENT) {
	/* Variable */
	out = var_node(tok->val.ident);
	match(IDENT);
	return out;
    } else {
	/* IDK what we got */
	printf("Syntax Error\n");
	return NULL;
    }
}

Node *power()
{
    Node *out = NULL;
    out = factor();
    if (tok->type == OP && tok->val.op == POWER) {
        match(OP);
        out = pow_node(out, factor());
    }
    return out;
}

Node *term()
{
    /* parse a term */
    Node *out = NULL;

    /* First, parse any factors */
    out = power();

    /* Times operation */
    if (tok->type == OP && tok->val.op == MULTIPLY) {
	match(OP);
	out = times_node(out, term());
	return out;
    }

    /* Divide operation */
    if (tok->type == OP && tok->val.op == DIVIDE) {
	match(OP);
        Node *denom = term();
        if ((out->type == INT) && (denom->type == INT)) {
            out = frac_node(out, denom);
        } else {
            out = divide_node(out, denom);
        }
	return out;
    }
    return out;
}

Node *expression()
{

    /* Parse an expression */
    Node *out = NULL;
    Node *ph = NULL;
    ph = term();

    /* Plus operation */
    if (tok->type == OP && tok->val.op == PLUS) {
	match(OP);
	out = plus_node(ph, expression());
	return out;
    }

    /* Minus Operation */
    if (tok->type == OP && tok->val.op == MINUS) {
	match(OP);
	out = minus_node(ph, expression());
	return out;
    }
    return ph;
}

void assignment()
{
    Node *var, *expr;
    match(KW_INT);
    if (tok->type == IDENT) {
	var = var_node(tok->val.ident);
	match(IDENT);
	match(ASSIGN_OP);
	expr = expression();
	match(SEMICOLON);
	add_symbol(var, expr);
    }
}

Node *statement()
{

    Node *out = NULL;
    if (tok->type == KW_INT) {
	assignment();
	out = statement();
    } else {
	out = expression();
    }
    if (tok->type == ENDA) {
        return out;
    }
    return out;
}

/* Main Parsing Routine */
Node *parse(FILE * src)
{
    Node *ast;
    Token mytok;
    tok = &mytok;
    source = src;

    /* Load the lookahead with the first token */
    read_one_token(tok, source);
    // print_token(tok);

    /* Let's go! */
    ast = statement();
    return ast;
}

Node *attach_variables(Node * ast)
{
    if (ast == NULL) {
        return NULL;
    }

    if (ast->type == VAR) {
	Symbol *sym = find_symbol(ast);
	return attach_variables(sym->expr);
    }

    if (ast->type == INT) {
	return ast;
    }

    ast->left = attach_variables(ast->left);
    ast->right = attach_variables(ast->right);
    return ast;
}

