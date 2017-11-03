#include <stdlib.h>
#include <stdio.h>

#include "cmplr.h"
#include "pretty_print.h"
#include "simplify_rne.h"
#include "simplify.h"
#include <math.h>


struct pp_buf * create_pp_buffer()
{
    struct pp_buf * buf = calloc(1, sizeof(struct pp_buf));
    return buf;
}

void pp_print_char(struct pp_buf * buf, char ch, int x, double y)
{

    int line_ind;
    struct pp_line * line = NULL;
    
    for (line_ind = 0; line_ind < buf->n_lines; line_ind++) {
        if (buf->lines[line_ind].y_pos == y) {
            line = &buf->lines[line_ind];
        }
    }

    if (line == NULL) {
        if (buf->n_lines == MAXLINE) return;
        line = &buf->lines[buf->n_lines];
        line->y_pos = y;
        buf->n_lines++;
    }

    line->text[x] = ch;
}

int line_pos_cmpr(const void * a, const void *b)
{
    const struct pp_line * A = (struct pp_line *) a;
    const struct pp_line * B = (struct pp_line *) b;
    return A->y_pos < B->y_pos;
}

void   pp_print_buffer(struct pp_buf * buf)
{
    /*  sort buffer lines */
    qsort(buf->lines, buf->n_lines, sizeof(struct pp_line), line_pos_cmpr);

    int i;
    for (i = 0; i < buf->n_lines; i++) {
        printf("%s\n", buf->lines[i].text);
    }
}

void   pp_print_expression(Node * ast, int * x, double * y, double * ULim, double * LLim,
                           struct pp_buf * buf)
{
    switch(ast->type) {
        case BIN_OP_TIMES:
            pp_print_product(ast, x, y, ULim, LLim, buf);
            break;
        case BIN_OP_POWER:
            pp_print_power(ast, x, y, ULim, LLim, buf);
            break;
        case FRAC:
            pp_print_fraction(ast, x, y, ULim, LLim, buf);
            break;
        case INT:
            pp_print_integer(ast, x, y, ULim, LLim, buf);
            break;
        case VAR:
            pp_print_var(ast, x, y, ULim, LLim, buf);
            break;     
    }
}

void   pp_print_product(Node * ast, int * x, double * y, double * ULim, double * LLim,
                           struct pp_buf * buf)
{
        int n = ast->n_args;
        int i;
        for (i = 0; i < n; i++) {
            pp_print_expression(ast->args[i], x, y, ULim, LLim, buf);
            *x++;
            pp_print_char(buf, ' ', *x, *y);
        }

}
void   pp_print_power(Node * ast, int *x, double *y, double * ULim, double * LLim, struct pp_buf * buf)
{
        pp_print_expression(ast->args[0], x, y, ULim, LLim, buf);
        /* advance to next character */
        *x++;
        double y_exponent = (*ULim + *y)/2.0; 
        pp_print_expression(ast->args[1], x, &y_exponent, ULim, LLim, buf);
}

void   pp_print_fraction(Node * ast, int * x, double *y, double * ULim, double * LLim, struct pp_buf * buf)
{

    int ntop = num_digits(numerator_fun(ast));
    int nbot = num_digits(denominator_fun(ast));

    int linelen = (ntop < nbot) ? nbot : ntop;
    int i;
   
    int x2 = *x;
    int xnum = *x;
    int xdenom = *x;
    double y2 = *y;
    double ynum = y2 - 1;
    double ydenom = y2 +1;
    double LLim_num = *y;
    double ULim_denom = *y;

    for (i = 0; i < linelen; i++) {
        pp_print_char(buf, '-', x2+i, y2);
    }
    pp_print_expression(ast->args[0], &xnum, &ynum, ULim, &LLim_num, buf);
    pp_print_expression(ast->args[1], &xdenom, &ydenom, &ULim_denom, LLim, buf);
}

void pp_print_integer(Node * ast, int * x, double *y, double * ULim, double * LLim, struct pp_buf * buf)
{

    char str[256];
    sprintf(str, "%d", ast->value);
    int slen = (int)((ceil(log10(ast->value))+1)*sizeof(char));

    int i;
    for (i = 0; i < slen; i++)
    {
        pp_print_char(buf, str[i], *x, *y);
        *x++;
    }

}

void pp_print_var(Node * ast, int * x, double * y, double * ULim, double * LLim, struct pp_buf * buf)
{
    int ind = 0;
    while (ast->name[ind] != '\0')
    {
        pp_print_char(buf, ast->name[ind], *x, *y);
        *x += 1;
    }
}
