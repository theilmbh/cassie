#ifndef PRETTYPRINT_H
#define PRETTYPRINT_H

#include "cmplr.h"

#define MAXLINE 128

struct pp_line {
    char text[81];
    int  y_pos;
};

struct pp_buf {
    struct pp_line lines[MAXLINE];
    int n_lines;
};

struct pp_buf * create_pp_buffer();
void   pp_print_char(struct pp_buf * buf, char ch, int x, double y);
void   pp_print_buffer(struct pp_buf * buf);
void   pp_print_expression(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_line * buf);

void   pp_print_product(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_line * buf);

void   pp_print_power(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_line * buf);

void   pp_print_fraction(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_line * buf);

void   pp_print_integer(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_line * buf);

void   pp_print_var(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_line * buf);


#endif
