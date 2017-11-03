/*
 * =====================================================================================
 *
 *       Filename:  symbol_table.h
 *
 *    Description:  Symbol table definitions and prototypes
 *
 *        Version:  1.0
 *        Created:  11/03/2017 01:29:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

enum symtab_type = {SYM_FUNC, SYM_VAR};

struct symtab_entry {
    enum symtab_type kind;
    char name[MAXIDENT];
    Node * expr;
};

struct symtab {
    struct symtab_entry * entry;
    struct symtab * next;
};

struct symtab * create_empty_symtab();

void delete_symtab(struct symtab * tab);

void check_if_defined(struct symtab * tab, char * symbol);

void define_symbol(struct symtab * tab, char * symbol, enum symtab_type kind, 
                   Node * expr);

void redefine_symbol(struct symtab * tab, char * symbol, enum symtab_type kind, 
                     Node * expr);

void clear_symbol(struct symtab * tab, char * symbol);

#endif

