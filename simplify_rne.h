#ifndef SIMPLIFYRNEH
#define SIMPLIFYRNEH

#include "cmplr.h"

Node * evaluate_quotient(Node * v, Node * w);
Node * evaluate_sum(Node * v, Node * w);
Node * evaluate_product(Node * v, Node * w);
Node * evaluate_power(Node *v, int n);

Node * simplify_RNE_rec(Node * u);
Node * simplify_RNE(Node *u);

Node * simplify_rational_number(Node * u);
int integer_gcd(int a, int b);
int absval(int a);
int num_digits(int a);

#endif
