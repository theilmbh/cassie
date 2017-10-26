/*
 * =====================================================================================
 *
 *       Filename:  symbol.c
 *
 *    Description:  Symbol Table Management
 *
 *        Version:  1.0
 *        Created:  09/20/2017 03:42:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmplr.h"
#include "symbol.h"

static int symcount = 0;

int check_symbol(char *nm)
{
    /* Check if a symbol is already
     * defined in the symbol table */
    int i;
    for (i = 0; i < symcount; i++) {
	if (!strcmp(symtab[i]->name, nm)) {
	    return 1;
	}
    }
    return 0;
}

Symbol *find_symbol(Node * n)
{
    /* Return a reference to a previously defined symbol
     * Print an error and exit if the symbol is not defined */
    int i;
    for (i = 0; i < symcount; i++) {
	if (!strcmp(symtab[i]->name, n->name)) {
	    return symtab[i];
	}
    }
    printf("Error: Variable %s undeclared\n", n->name);
    exit(-1);
}

/* Add a symbol to the symbol table. 
 * Print Error and exit if symbol is already defined */
void add_symbol(Node * var, Node * expr)
{

    if (check_symbol(var->name)) {
	printf("Symbol %s already declared. Terminating.\n", var->name);
	exit(-1);
    }
    Symbol *sym = malloc(sizeof(Symbol));
    strcpy(sym->name, var->name);
    sym->expr = expr;
    symtab[symcount] = sym;
    symcount++;
}

