/*
 * =====================================================================================
 *
 *       Filename:  parse.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/26/2017 10:25:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef PARSE_H
#define PARSE_H

#include "symbol.h"
#include "algebra.h"
#include "cmplr.h"

void match(token_t type);
Node *negate(Node * n);
Node *statement();
Node *parse();

#endif
