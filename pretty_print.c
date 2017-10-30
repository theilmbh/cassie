#include <stdlib.h>
#include <stdio.h>

#include "cmplr.h"
#include "pretty_print.h"


struct pp_buf * create_pp_buffer()
{
    struct pp_buf * buf = calloc(1, sizeof(struct pp_buf));
    return buf;
}

void   pp_print_char(struct pp_line * buf, char ch, int x, int y)
{

    int line_ind;
    struct pp_line * line = NULL;
    
    for (line_ind = 0; line_ind < buf->n_lines; line_ind++) {
        if (buf[line_ind].y_pos == y) {
            line = &buf[line_ind];
        }
    }

    if (line == NULL) {
        if (buf->n_lines == MAXLINE) return;
        line = &buf[buf->n_lines];
        line->y_pos = y;
        buf->n_lines++;
    }

    line->text[x] = ch;
}

void   pp_print_buffer(struct pp_line * buf);
void   pp_print_expression(Node * ast, int * x, int * y, struct pp_line * buf)
{
    switch(ast->type) {
        case BIN_OP_TIMES:
            pp_print_product(ast, x, y, buf);
            break;
        case BIN_OP_POWER:
            pp_print_power(ast, x, y, buf);
            break;
        case FRAC:
            pp_print_fraction(ast, x, y, buf);
            break;
        case INT:
            pp_print_integer(ast, x, y, buf);
            break;
        case VAR:
            pp_print_var(ast, x, y, buf);
            break;     
    }
}

void   pp_print_product(Node * ast, int * x, int * y, struct pp_line * buf)
{
        int n = ast->n_args;
        int i;
        for (i = 0; i < n; i++) {
            pp_print_expression(ast->args[i], x, y, buf);
            *x++;
            pp_print_char(' ', x, y, buf);
        }

}
void   pp_print_power(Node * ast, int x, int y, struct pp_line * buf)
{
        pp_print_expression(ast->args[0], x, y, buf);
        *x++;
        *y--;
        pp_print_expression(ast->args[1], x, y, buf);
        *y++;
}

void   pp_print_fraction(Node * ast, int x, int y, struct pp_line * buf)
{

    int ntop = num_digits(numerator_fun(ast));
    int nbot = num_digits(denominator_fun(ast));

    int linelen = (ntop < nbot) ? nbot : ntop;
    int i;
   
    int x2 = *x;
    int y2 = *y;

    for (i = 0; i < linelen; i++) {
        pp_print_char(buf, '-', x2+i, y2);
    }
    y2--;     
    pp_print_expression(ast->args[0], &x2, &y2, buf);
    y2 += 2;
    pp_print_expresion(ast->args[1], &x2, &y2, buf);
}

    
