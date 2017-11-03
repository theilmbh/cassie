/*
 * =====================================================================================
 *
 *       Filename:  derivative.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/03/2017 02:00:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Brad Theilman (BHT), bradtheilman@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "cmplr.h"
#include "simplify.h"

Node * derivative_evaluate(Node * d);

Node * derivative_do_evaluate(Node * u, Node * x);

Node * derivative_node(Node * u, Node * x);

#endif
