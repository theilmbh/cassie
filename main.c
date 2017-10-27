/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  CASSIE main REPL
 *
 *        Version:  1.0
 *        Created:  10/26/2017 02:09:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmplr.h"
#include "symbol.h"
#include "algebra.h"
#include "simplify_rne.h"
#include "simplify.h"   

#define PRINT_AST

void print_fraction( Node * frac)
{
    int ntop = num_digits(numerator_fun(frac));
    int nbot = num_digits(denominator_fun(frac));

    int linelen = (ntop < nbot) ? nbot : ntop;
    int i;
    printf("%d\n", numerator_fun(frac));
    for (i = 0; i < linelen; i++) {
        printf("-");
    }
    printf("\n");
    printf("%d", denominator_fun(frac));
}
    
void print_expression( Node * ast)
{

    /* If it's an operator, print the left side,
       then the operator, then the right side */
    if (ast->type != INT && ast->type != VAR) {
	switch (ast->type) {
	case BIN_OP_TIMES:
            print_expression(ast->args[0]);
	    printf(" ");
            print_expression(ast->args[1]);
	    break;
	case BIN_OP_PLUS:
            print_expression(ast->args[0]);
	    printf("  +  ");
            print_expression(ast->args[1]);
	    break;
	case BIN_OP_MINUS:
            print_expression(ast->args[0]);
	    printf("  -  ");
            print_expression(ast->args[1]);
	    break;
        case BIN_OP_POWER:
            print_expression(ast->args[0]);
            printf("^");
            print_expression(ast->args[1]);
            break;
        case FRAC:
            print_fraction(ast);
            break; 
	}
    }

    /* Otherwise just print the value */
    if (ast->type == INT) {
	printf("%d", ast->value);
    }
    if (ast->type == VAR) {
	printf("%s", ast->name);
    }

}

void print_node(FILE * out, Node * n, int indent)
{
    // for(int i=0; i<indent; i++) {
    //   printf(" ");
    // }
    switch (n->type) {
    case BIN_OP_TIMES:
	fprintf(out, "P Multiply\n");
	break;
    case BIN_OP_PLUS:
	fprintf(out, "P Add\n");
	break;
    case BIN_OP_MINUS:
	fprintf(out, "P Minus\n");
	break;
    case BIN_OP_DIVIDE:
	fprintf(out, "P Divide\n");
	break;
    case FRAC:
        fprintf(out, "P Frac\n");
        break;
    case INT:
	fprintf(out, "P Integer: %d\n", n->value);
	break;
    case VAR:
	fprintf(out, "P Varible: %s\n", n->name);
	break;
    case PAREN:
        print_node(out, n->right, indent);
        break;
    case BIN_OP_POWER:
        fprintf(out, "P Power\n");
        break;
    }
}

void print_ast(FILE * out, Node * n, int indent, int levelmin, int levelmax)
{
    int i,j;
    int iskip = 4;
    print_node(out, n, indent);
    if (n->args[0] != NULL) {
        for (i = 0; i < levelmax; i++) {
            if (i>levelmin) {
                fprintf(out, "|");
                iskip -=1;
            }
            for (j = 0; j < iskip; j++) {
                fprintf(out, " ");
            }
        }
	fprintf(out, "|-> ");
	print_ast(out, n->args[0], indent + 4, levelmin, levelmax+1);
    }
    iskip=4;
    if (n->args[1] != NULL) {
        for (i = 0; i < levelmax; i++) {
            if (i>levelmin) {
                fprintf(out, "|");
                iskip -=1;
            }
            for (j = 0; j < iskip; j++) {
                fprintf(out, " ");
            }
        }
	fprintf(out, "|-> ");
	print_ast(out, n->args[1], indent + 4, levelmin+1, levelmax+1);
    }
}


int main(int argc, char **argv)
{
    //source = fopen(argv[1], "r");
    Node *ast, *ast1;
    int eval_num = 1;
    printf("Welcome to CASSIE \n");
    printf("Computer Algebra System v0.01\n");
    printf("Copyright (c) 2017 Brad Theilman\n\n");
    while (1) {
        printf("In  [%d] :=> ", eval_num);
        ast = parse(stdin);
        print_ast(stdout, ast, 0, -1, 0);
        ast = automatic_simplify(ast);
        //ast1 = attach_variables(ast);
        //FILE *out = fopen("./ast.tree", "w");
#ifdef PRINT_AST
        print_ast(stdout, ast, 0, -1, 0);
#endif
        printf("\n");
        print_expression(ast);
        printf("\n\n");
        //int v = evaluate(rewrite_minus(ast));
        //printf("\nOut [%d] :=> %d\n\n", eval_num, v);
        eval_num++;
    }


    //
    // printf("Rewrite Minus: \n");
    // ast = rewrite_minus(ast);
    // print_ast(ast, 0);
    //
    // printf("Expanded: \n");
    // ast = expand(ast);
    // print_ast(ast, 0);
    //
    // printf("Reorder coeff: \n");
    // ast = reorder_coeff(ast);
    // print_ast(ast, 0);
    //
    // print_expression(ast);
    // printf("\n");
    //fclose(out);
    //fclose(source);
    return 0;
}
