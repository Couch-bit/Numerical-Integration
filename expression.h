#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>


// Checks whether an expression is correct.
bool is_correct(char*, char);
// Returns the reverse polish notation of a function.
char* reverse_polish_notation(char*);
// Evaluates an expression using the reverse polish notation.
double evaluate_expression(char*, double, char);