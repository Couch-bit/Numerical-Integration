#include "expression.h"


#define PI 3.14159265358979323846


// Defines an enum for previous expression type.
typedef enum {
    none,
    operator,
    number
} enum_previous;

// Defines a stack of machine numbers.
typedef struct stack_double {
    double data;
    struct stack_double* next;
} stack_d;

// Defines a stack of characters.
typedef struct stack_char {
    char data;
    struct stack_char* next;
} stack_c;


// Checks whether the string is a non-negative number.
static bool is_number(char*);
// Checks whether an inner function is correct.
static bool is_function_correct(char*, char);
// Evaluates a function that's a part of the expression 
// that was being evaluated as part of
// the evaluate_expression function.
static double elementary_function(char*, double, char);
// Calculates the reverse polish notation of a composite function.
static char* function_rpn(char*, uint_fast16_t*, uint_fast16_t*);
// Checks whether the string is numeric
// (by checking if the first character is an operator or parantheses).
static bool is_numeric(char);
// Returns the priority of an operator.
static uint_fast16_t operation_priority(char);
// Performs push for double stack.
static stack_d* push_d(stack_d*, double);
// Performs pop for double stack.
static stack_d* pop_d(stack_d*, double*);
// Performs push for char stack.
static stack_c* push_c(stack_c*, char);
// Performs pop for char stack.
static stack_c* pop_c(stack_c*, char*);


bool is_correct(char* function, char variable) {


    if (*function == '\0') {
        return false;
    }

    char* current = NULL;
    char* temp_pointer = NULL;
    uint_fast16_t current_memory = 2;

    current = (char*)malloc(current_memory);
    if (!current) {
        return false;
    }

    uint_fast16_t parantheses_count = 0;
    enum_previous previous = none;
    // Declares the variable that stores the index in the string
    // currently being read from the function.
    uint_fast16_t current_index;
    // Declares an index to be used 
    // for recursively checking expressions inside parantheses.
    uint_fast16_t temp_index;

    // Skips leading spaces in the function.
    while (*function == ' ') {
        ++function;
    }

    while (*function != '\0') {

        current[0] = *function;

        current_index = 1;
        ++function;

        if (is_numeric(current[0])) {

            // Continues assigning function string 
            // characters to current string
            // until the next character is a space,
            // null terminator or isn't numeric, 
            // except for the situation where a letter 
            // is followed by an opening parantheses
            // meaning that the current string is a function
            //   (the space and numeric rules don't apply if 
            //   the string has already been marked as a function).
            while ((*function != ' ' || parantheses_count) &&
                *function != '\0' && 
                (((is_numeric(*function)) || parantheses_count) ||
                    (is_numeric(*(function - 1)) &&
                        *function == '('))) {

                // Checks if there is enough memory in the 
                // current string to store the next character.
                if (current_index + 2 > current_memory) {
                    current_memory = current_index + 2;
                    temp_pointer = (char*)
                        realloc(current, current_memory);
                    if (!temp_pointer) {
                        free(current);
                        return false;
                    }
                    current = temp_pointer;
                }

                if ((current[current_index] = *function) == '(')
                    ++parantheses_count;

                else if (current[current_index] == ')') {
                    if (parantheses_count > 0) --parantheses_count;
                    else return false;
                }

                ++function;
                ++current_index;
            }

            if (parantheses_count) {
                return false;
            }
        }
        current[current_index] = '\0';

        // Changes the subtraction sign to tilde if
        // it's meant as a negative of a number. 
        if (!previous && current[0] == '-') {
            current[0] = '~';
        }

        if (is_numeric(current[0])) {

            if (previous == number) return false;

            previous = number;

            if (strlen(current) == 1 && current[0] == variable) {}
            else if (!strcmp(current, "e")) {}
            else if (!strcmp(current, "pi")) {}
            else if (is_number(current)) {}
            else if (!is_function_correct(current, variable)) {
                return false;
            }
        }
        else if (current[0] != '(' && current[0] != ')') {

            if (previous == operator ||
                (previous == none && current[0] != '~')) return false;
            previous = operator;
        }
        else if (current[0] == '(') {

            if (previous == number) {
                return false;
            }

            previous = number;
            temp_index = 0;
            parantheses_count = 1;

            // While not all parantheses are closed and
            // there are still characters
            // in the string continues reading characters 
            // until this requirement is met
            // or reaching the end of the string.
            while (parantheses_count &&
                function[temp_index] != '\0') {
                if (function[temp_index] == '(')
                    ++parantheses_count;
                else if (function[temp_index] == ')')
                    --parantheses_count;
                ++temp_index;
            }

            if (parantheses_count) return false;

            function[temp_index - 1] = '\0';
            if (!is_correct(function, variable)) {
                function[temp_index - 1] = ')';
                return false;
            }
            function[temp_index - 1] = ')';

            function += temp_index;
        }
        else return false;

        // Skips trailing spaces.
        while (*function == ' ') ++function;
    }

    free(current);

    if (previous == number) return true;
    return false;
}


double evaluate_expression(char* function, double value,
    char variable) {

    if (!function) {
        return NAN;
    }

    stack_d* stack_for_computing = NULL;
    char* current = NULL;
    char* temp_pointer = NULL;
    uint_fast16_t current_memory = 2;

    current = (char*)malloc(current_memory);
    if (!current) {
        return NAN;
    }

    double temp = NAN;
    uint_fast16_t parantheses_count = 0;
    uint_fast16_t index;
    double number;

    while (*function != '\0') {

        index = 0;

        // Read the number or operator until reaching a space 
        // (unless it's inside of a function)
        while ((*function != ' ' || parantheses_count) &&
            *function != '\0') {

            // Allocates more memory for the function if necessary.
            if (index + 2 > current_memory) {
                current_memory = index + 2;
                temp_pointer = (char*)
                    realloc(current, current_memory);
                if (!temp_pointer) {
                    free(current);
                    while (stack_for_computing = 
                        pop_d(stack_for_computing, &temp)) {}
                    return NAN;
                }
                current = temp_pointer;
            }

            if ((current[index] = *function) == '(')
                ++parantheses_count;
            else if (*function == ')') --parantheses_count;

            ++function;
            ++index;
        }

        current[index] = '\0';

        // Skips trailing space.
        if (*function == ' ') ++function;

        if (is_numeric(current[0])) {

            if (strlen(current) == 1 &&
                current[0] == variable) number = value;
            else if (!strcmp(current, "e")) number = exp(1);
            else if (!strcmp(current, "pi")) number = PI;
            else if (isdigit(current[0])) {
                sscanf_s(current, "%lf", &number);
            }
            else number = 
                elementary_function(current, value, variable);

            stack_for_computing = 
                push_d(stack_for_computing, number);
            
            if (!stack_for_computing) {
                free(current);
                return NAN;
            }
        }
        else {

            if (!stack_for_computing) {
                free(current);
                return NAN;
            }

            if (current[0] == '~') {
                stack_for_computing->data =
                    -stack_for_computing->data;
            }
            else {
                stack_for_computing =
                    pop_d(stack_for_computing, &temp);
                switch (current[0]) {
                case '^':
                    stack_for_computing->data =
                        pow(stack_for_computing->data, temp);
                    break;
                case '/':
                    stack_for_computing->data /= temp;
                    break;
                case '*':
                    stack_for_computing->data *= temp;
                    break;
                case '+':
                    stack_for_computing->data += temp;
                    break;
                case '-':
                    stack_for_computing->data -= temp;
                    break;
                }
            }
        }
    }

    // Removes the last number from the stack.
    stack_for_computing = pop_d(stack_for_computing, &temp);

    free(current);

    return temp;
}


char* reverse_polish_notation(char* function) {

    stack_c* stack_for_transformation = NULL;
    char* result = NULL;
    char* current = NULL;
    char* temp_pointer = NULL;
    uint_fast16_t length_of_function =
        (uint_fast16_t)strlen(function) + 1;

    result = (char*)malloc(length_of_function);
    if (!result) {
        return NULL;
    }

    uint_fast16_t current_memory = 2;

    current = (char*)malloc(current_memory);
    if (!current) {
        free(result);
        return NULL;
    }

    uint_fast16_t index = 0;
    uint_fast16_t parantheses_count = 0;
    uint_fast16_t current_index;
    uint_fast16_t current_operator;
    char temp;

    // While loop skips leading spaces in the function string.
    while (*function == ' ') {
        ++function;
    }

    while (*function != '\0') {

        current[0] = *function;
        current_index = 1;
        ++function;

        if (is_numeric(current[0])) {
            // Continues assigning function string
            // characters to current string
            // until the next character is a space, 
            // null terminator or isn't numeric, 
            // except for the situation where a letter
            // is followed by an opening parantheses
            // meaning that the current string is a function
            //  (the space and numeric rules don't apply if
            //   the string has already been marked as a function)
            while ((*function != ' ' || parantheses_count) &&
                *function != '\0' && 
                (((is_numeric(*function)) || parantheses_count) ||
                    (is_numeric(*(function - 1)) &&
                        *function == '('))) {

                // Allocates memory if necessary.
                if (current_index + 2 > current_memory) {
                    current_memory = current_index + 2;
                    temp_pointer = (char*)
                        realloc(current, current_memory);
                    if (!temp_pointer) {
                        free(result);
                        free(current);
                        while (stack_for_transformation =
                            pop_c(stack_for_transformation, &temp)) {}
                        return NULL;
                    }
                    current = temp_pointer;
                }

                if ((current[current_index] = *function) == '(')
                    ++parantheses_count;

                // If the end of a function is reached
                // make a reverse polish notation
                // of the expression inside it.
                else if (current[current_index] == ')') {
                    --parantheses_count;
                    if (!parantheses_count) {
                        current[current_index] = '\0';
                        current = function_rpn(current,
                            &current_memory, &current_index);
                        if (!current) {
                            free(result);
                            while (stack_for_transformation =
                                pop_c(stack_for_transformation,
                                    &temp)) {}
                            return NULL;
                        }
                    }
                }

                ++function;
                ++current_index;
            }
        }

        current[current_index] = '\0';

        // Changes the subtraction sign to tilde 
        // if it's meant as a negative of a number. 
        if (((strlen(function) == (size_t)length_of_function - 2) ||
            (*(function - 2) == '(')) && current[0] == '-') 
            current[0] = '~';

        if (is_numeric(current[0])) {

            // Allocates extra memory if necessary.
            if (index + current_index + 1 > length_of_function) {
                length_of_function = index + current_index + 1;
                temp_pointer = (char*)
                    realloc(result, length_of_function);
                if (!temp_pointer) {
                    free(current);
                    free(result);
                    while (stack_for_transformation =
                        pop_c(stack_for_transformation, &temp)) {}
                    return NULL;
                }
                result = temp_pointer;
            }

            for (register uint_fast16_t i = 0;
                i < current_index; ++i) {
                result[index] = current[i];
                ++index;
            }

            result[index] = ' ';
            ++index;
        }
        else if (current[0] != '(' && current[0] != ')') {

            current_operator = operation_priority(current[0]);

            while (stack_for_transformation &&
                stack_for_transformation->data != '(') {

                if (operation_priority(stack_for_transformation->data)
                    < current_operator) {
                    stack_for_transformation =
                        push_c(stack_for_transformation, current[0]);
                    if (!stack_for_transformation) {
                        free(result);
                        free(current);
                        return NULL;
                    }
                    break;
                }

                stack_for_transformation =
                    pop_c(stack_for_transformation, &temp);

                // Allocates extra memory if necessary.
                if (index + 2 > length_of_function) {
                    length_of_function = index + 2;
                    temp_pointer = (char*)
                        realloc(result, length_of_function);
                    if (!temp_pointer) {
                        free(current);
                        free(result);
                        while (stack_for_transformation =
                            pop_c(stack_for_transformation, &temp)) {}
                        return NULL;
                    }
                    result = temp_pointer;
                }

                result[index] = temp;
                ++index;
                result[index] = ' ';
                ++index;
            }

            if (!stack_for_transformation ||
                stack_for_transformation->data == '(') {
                stack_for_transformation =
                    push_c(stack_for_transformation, current[0]);
                if (!stack_for_transformation) {
                    free(result);
                    free(current);
                    return NULL;
                }
            }
                
        }
        else if (current[0] == '(') {
            stack_for_transformation =
                push_c(stack_for_transformation, current[0]);
            if (!stack_for_transformation) {
                free(result);
                free(current);
                return NULL;
            }
        }
        // If the current string is
        // a closing parantheses extract operators
        // from the stack until reaching the opening parantheses.
        else {
            while (stack_for_transformation &&
                stack_for_transformation->data != '(') {

                stack_for_transformation =
                    pop_c(stack_for_transformation, &temp);

                // Allocates extra memory if necessary.
                if (index + 2 > length_of_function) {
                    length_of_function = index + 2;
                    temp_pointer = (char*)
                        realloc(result, length_of_function);
                    if (!temp_pointer) {
                        free(result);
                        free(current);
                        while (stack_for_transformation =
                            pop_c(stack_for_transformation, &temp)) {
                        }
                        return NULL;
                    }
                    result = temp_pointer;
                }

                result[index] = temp;
                ++index;
                result[index] = ' ';
                ++index;
            }

            // Removes the opening parantheses from the stack.
            stack_for_transformation =
                pop_c(stack_for_transformation, &temp);
        }

        // Skips trailing spaces.
        while (*function == ' ') ++function;
    }

    // Removes all remaining operators from the
    // stack and adds them to the current string.
    while (stack_for_transformation) {
        stack_for_transformation =
            pop_c(stack_for_transformation, &temp);

        // Allocates extra memory if necessary.
        if (index + 2 > length_of_function) {
            
            length_of_function = index + 2;
            temp_pointer = (char*)
                realloc(result, length_of_function);
            if (!temp_pointer) {
                free(result);
                free(current);
                while (stack_for_transformation =
                    pop_c(stack_for_transformation, &temp)) {}
                return NULL;
            }
            result = temp_pointer;
        }

        result[index] = temp;
        ++index;
        result[index] = ' ';
        ++index;
    }

    // Replaces the last space in the result
    // string with a null terminator.
    result[index - 1] = '\0';

    free(current);

    return result;
}


static bool is_number(char* string) {

    bool can_be_coma = true;

    if (!isdigit(*string)) return false;
    ++string;

    while (*string != '\0') {
        if (isdigit(*string)) {}
        else if (*string == '.' && can_be_coma) can_be_coma = false;
        else return false;
        ++string;
    }

    return true;
}


static bool is_function_correct(char* expression, char variable_char) {

    char* temp = NULL;
    uint_fast16_t index = 0;

    // Finds the first opening parantheses in the expression.
    while (expression[index] != '(' && expression[index] != '\0') 
        ++index;

    temp = &expression[strlen(expression) - 1];

    // Returns false if the there was
    // no opening parantheses in the string.
    if (expression[index] == '\0') return false;
    if (*temp != ')') return false;

    // Sets the final parantheses in the string to the null terminator.
    *temp = '\0';
    // Sets the opening parantheses to the null terminator.
    expression[index] = '\0';
    // Sets the index to be the index of the first character
    // after the first opening parantheses.
    ++index;

    // Checks if the name matches any of the defined functions.
    // If it does, checks the inside expression,
    // if it doesn't returns false.
    switch (index - 1) {
    case 1:
        return false;
    case 2:
        if (strcmp(expression, "ln")) return false;
        return is_correct(&expression[index], variable_char);
    case 3:
        if (!strcmp(expression, "abs") || 
            !strcmp(expression, "log") || 
            !strcmp(expression, "sin") || 
            !strcmp(expression, "cos") || 
            !strcmp(expression, "tan") || 
            !strcmp(expression, "cot"))
            return is_correct(&expression[index], variable_char);

        return false;
    case 7:
        if (!strcmp(expression, "arcsinh") ||
            !strcmp(expression, "arccosh") ||
            !strcmp(expression, "arctanh") ||
            !strcmp(expression, "arccoth"))
            return is_correct(&expression[index], variable_char);
        // FALLTHROUGH
    case 6:
        if (index == 7 && (!strcmp(expression, "arcsin") ||
            !strcmp(expression, "arccos") ||
            !strcmp(expression, "arctan") ||
            !strcmp(expression, "arccot")))
            return is_correct(&expression[index], variable_char);
        // FALLTHROUGH
    case 4:
        if (index == 5 && (!strcmp(expression, "sqrt") ||
            !strcmp(expression, "sinh") ||
            !strcmp(expression, "cosh") ||
            !strcmp(expression, "tanh") ||
            !strcmp(expression, "coth"))) 
            return is_correct(&expression[index], variable_char);
        // FALLTHROUGH
    default:
        // Checks if the first 3 letters match "log".
        // If they do checks if it's followed by a number,
        // which is then followed by
        // an expression inside the parantheses 
        // (which is correct).
        if (expression[0] == 'l' && expression[1] == 'o' &&
            expression[2] == 'g' && is_number(&expression[3])) 
            return is_correct(&expression[index], variable_char);

        return false;
    }
}


static double elementary_function(char* expression, double argument,
    char argument_char) {

    // Defines a variable that will store
    // the index of the beginning of the 
    // expression in the function.
    uint_fast16_t index = 0;
    // Defines a variable that will store
    // the base of a logarithm if necessary.
    double temp;

    // Finds the index of the first parantheses.
    while (expression[index] != '(') {
        ++index;
    }

    // Replaces the final closing parantheses with a null terminator.
    expression[strlen(expression) - 1] = '\0';
    // Replaces the first opening parantheses with a null terminator.
    expression[index] = '\0';
    // Makes the index the first character of the inside expression.
    ++index;

    // Changes which functions to
    // check based on the length of the name.
    switch (index - 1) {
    case 2:
        return log
        (evaluate_expression(
            &expression[index],
            argument,
            argument_char));
    case 3:
        // Checks the one letter that's enough to
        // identify a function and calculates it's value.
        if (expression[0] == 'a')
            return fabs
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[0] == 's')
            return sin
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[2] == 's')
            return cos
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[0] == 't')
            return tan
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[0] == 'l') 
            return log10
            (evaluate_expression
            (&expression[index],
                argument,
                argument_char));
        // If it's none of the above it's cot.
        else return 1 / tan
        (evaluate_expression(&expression[index],
            argument,
            argument_char));
    case 7:
        // Check if it's a hyperbolic cyclometric function
        // by checking the first character.
        if (*expression == 'a') {
            // Checks the one letter that's enough to 
            // identify a function and calculates it's value.
            if (expression[3] == 's') 
                return asinh
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
            if (expression[5] == 's')
                return acosh
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
            if (expression[3] == 't')
                return atanh
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
            // If it's none of the aboce it's arccoth.
            else 
                return (double)1 / 2 * log
                (1 + 2/(evaluate_expression(&expression[index],
                    argument,
                    argument_char) - 1));
        }
        // FALLTHROUGH
    case 6:
        // Checks if it's a cyclometric function
        // by checking the first character.
        if (index == 7 && *expression == 'a') {
            // Checks the one letter that's enough to
            // identify a function and calculates it's value.
            if (expression[3] == 's')
                return asin
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
            if (expression[5] == 's')
                return acos
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
            if (expression[3] == 't')
                return atan
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
            // If it's none of the above it's arccot.
            else 
                return PI/2 - atan
                (evaluate_expression(&expression[index],
                    argument,
                    argument_char));
        }
        // FALLTHROUGH
    case 4:
        // Checks the one letter that's enough to
        // identify a function and calculates it's value.
        if (expression[1] == 'q') 
            return sqrt
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[0] == 's') 
            return sinh
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[2] == 's')
            return cosh
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[0] == 't')
            return tanh
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        if (expression[0] == 'c')
            return (double)1 / tanh
            (evaluate_expression(&expression[index],
                argument,
                argument_char));
        // FALLTHROUGH
    default:
        // If it's none of the above it has to
        // be a logarithm with a given base.
        // Gets the base of the logarithm.
        sscanf_s(&expression[3], "%lf", &temp);
        // Calculates the value of the function.
        return log
        (evaluate_expression(&expression[index],
            argument,
            argument_char)) / 
            log(temp);
    }
}


static uint_fast16_t operation_priority(char operation) {

    if (operation == '^') return 3;
    if (operation == '~') return 2;
    if (operation == '/' || operation == '*') return 1;
    return 0;
}


static char* function_rpn(char* string, uint_fast16_t* max_length,
    uint_fast16_t* index) {

    // Declares a pointer that will point at the
    // reverse polish notation of the inner function.
    char* temp_notation = NULL;
    char* temp_pointer = NULL;
    // Defines an integer that will store in the index
    // in the reverse polish notation of the inner function.
    uint_fast16_t temp_index = 0;

    *index = 0;

    // Makes the index the first
    // character after an opening parantheses.
    while (string[*index] != '(') {
        ++(*index);
    }
    ++(*index);

    // Makes a reverse polish notation of the inside function.
    temp_notation = reverse_polish_notation(&string[*index]);
    if (!temp_notation) {
        free(string);
        return NULL;
    }

    // Adds the reverse polish notation of the inside to the string.
    while (temp_notation[temp_index] != '\0') {

        // Allocates extra memory if necessary.
        if (*index + 3 > *max_length) {
            *max_length = *index + 3;
            temp_pointer = (char*)
                realloc(string, (size_t)*max_length);
            if (!temp_pointer) {
                free(string);
                return NULL;
            }
            string = temp_pointer;
        }

        string[*index] = temp_notation[temp_index];
        ++(*index);
        ++temp_index;
    }

    // Adds back the closing parantheses at the end of the string.
    string[*index] = ')';

    free(temp_notation);

    return string;
}


static bool is_numeric(char current) {

    if (current == '+' || current == '-' ||
        current == '*' || current == '/' ||
        current == '~' || current == '^' ||
        current == '(' || current == ')') 
        return false;
    return true;
}


static stack_d* push_d(stack_d* top, double element) {

    stack_d* result = NULL;

    result = (stack_d*)
        malloc(sizeof(stack_d));
    if (!result) {
        while (top = pop_d(top, &element)) {}
        return NULL;
    }

    result->next = top;
    result->data = element;

    return result;
}


static stack_d* pop_d(stack_d* top, double* element) {

    if (top != NULL) {
        stack_d* result = top->next;
        *element = top->data;
        free(top);
        return result;
    }

    return NULL;
}


static stack_c* push_c(stack_c* top, char element) {

    stack_c* result = NULL;

    result = (stack_c*)
        malloc(sizeof(stack_c));
    if (!result) {
        while (top = pop_c(top, &element)) {}
        return NULL;
    }

    result->next = top;
    result->data = element;

    return result;
}


static stack_c* pop_c(stack_c* top, char* element) {

    if (top != NULL) {
        stack_c* result = top->next;
        *element = top->data;
        free(top);
        return result;
    }

    return NULL;
}
