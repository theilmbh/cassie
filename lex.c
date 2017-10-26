#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmplr.h"

void print_token(Token * tok)
{
    switch (tok->type) {

    case OP:
	printf("Operator\n");
	break;
    case INTEGER:
	printf("Integer: %d\n", tok->val.int_val);
	break;
    case ENDA:
	printf("END\n");
	break;
    case KW_INT:
	printf("Keyword int\n");
	break;
    case SEMICOLON:
	printf("Semicolon\n");
	break;
    case ASSIGN_OP:
	printf("Assign Op\n");
	break;
    }
}

void read_one_token(Token * tok, FILE * f)
{
    int ch;
    ch = getc(f);
    if (ch == '\n') {
        tok->type = ENDA;
        return;
    }
    while (isspace(ch)) {
	ch = getc(f);
    }

    /* Handle integer literal */
    if (isdigit(ch)) {
	tok->type = INTEGER;
	tok->val.int_val = ch - '0';
	while (isdigit(ch = getc(f))) {
	    tok->val.int_val = tok->val.int_val * 10 + ch - '0';
	}
	ungetc(ch, f);
	return;
    }

    if (ch == '+') {
	tok->type = OP;
	tok->val.op = PLUS;
	return;
    }

    if (ch == '*') {
	tok->type = OP;
	tok->val.op = MULTIPLY;
	return;
    }

    if (ch == '-') {
	tok->type = OP;
	tok->val.op = MINUS;
	return;
    }

    if (ch == '/') {
	tok->type = OP;
	tok->val.op = DIVIDE;
	return;
    }

    if (ch == '(') {
	tok->type = L_PAREN;
	return;
    }

    if (ch == ')') {
	tok->type = R_PAREN;
	return;
    }

    if (ch == ';') {
	tok->type = SEMICOLON;
	return;
    }

    if (ch == '^') {
	tok->type = OP;
        tok->val.op = POWER;    
	return;
    }

    if (ch == ':') {
	if ((ch = getc(f)) == '=') {
	    tok->type = ASSIGN_OP;
	    return;
	} else {
	    ungetc(ch, f);
	}
    }

    if (isalpha(ch)) {
	int ind = 0;
	tok->type = IDENT;
	tok->val.ident[ind] = ch;
	while (isalpha(ch = getc(f))) {
	    ind += 1;
	    tok->val.ident[ind] = ch;
	}
	ungetc(ch, f);
	tok->val.ident[ind + 1] = '\0';
	// printf("Lex Ident: %s\n", tok->val.ident);
	if (!strcmp(tok->val.ident, "int")) {
	    tok->type = KW_INT;
	}
	return;
    }

    if (ch == EOF) {
	printf("End of file?\n");
	tok->type = ENDA;
	return;
    }
    if (ch == '\n' || ch == '\r') {
        tok->type = ENDA;
        return;
    }

    printf("Lexing Error!\n");
}
