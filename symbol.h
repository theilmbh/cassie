/*
 * =====================================================================================
 *
 *       Filename:  symbol.h
 *
 *    Description:  Symbol Table Management declarations
 *
 *        Version:  1.0
 *        Created:  09/20/2017 03:33:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include "cmplr.h"

#define MAXSYM 256

typedef struct Symbol {
    char name[MAXIDENT];
    Node * (*eval)();
    void (*print)();
    Node *expr;
} Symbol;

Symbol *symtab[MAXSYM];

int check_symbol(char *);
Symbol *find_symbol(Node *);
void add_symbol(Node *, Node *);

#endif
