#include "expression.h"


// Defines the value for pi.
#define PI 3.141592653


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
// that was being evaluated as part of the evaluate_expression function.
static double elementary_function(char*, double, char);
// Calculates the reverse polish notation of a composite function.
static char* function_rpn(char*, uint_fast16_t*, uint_fast16_t*);
// Checks whether the string is numeric
// (by checking if the first character is an operator or parantheses)
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

    // Returns false if the function string is empty.
    if (*function == '\0') {
        return false;
    }
    // Defines a variable that stores information on how much
    // memory is currently being used by the current string.
    uint_fast16_t current_memory = 2;
    // Defines a char pointer that will point to 
    // newly allocated 2 bytes memory.
    char* current = malloc(current_memory);
    if (!current) {
        printf("Memory Error, aborting...\n");
        exit(-1);
    }
    // Defines the integer that stores information on
    // whether the string currently being read is a function.
    uint_fast16_t is_function = 0;
    // Defines an integer that stores information on
    // what the last element in the expression was.
    // (0 - none, 1 - number, 2 - operator)
    uint_fast16_t previous = 0;
    // Declares the variable that stores the index in the string currently
    // being read from the function.
    uint_fast16_t current_index;
    // Declares an index to be used 
    // for recursively checking expressions inside parantheses.
    uint_fast16_t temp_index;

    // Skips leading spaces in the function.
    while (*function == ' ') {
        ++function;
    }

    // Reads the entire function string.
    while (*function != '\0') {
        // Assigns the next character in the function string to the first character of the current string.
        current[0] = *function;
        // Resets current_index to 1.
        current_index = 1;
        // Moves on to the nest character in the function string.
        ++function;
        // Continues assigning function string characters to current string
        // if the first character isn't an operator or parantheses
        // (presumably).
        if (is_numeric(current[0])) {
            // Continues assigning function string characters to current string
            // until the next character is a space, null terminator or isn't numeric, 
            // except for the situation where a letter is followed by an opening parantheses
            // meaning that the current string is a function
            // (the space and numeric rules don't apply if the string has already been marked as a function)
            while ((*function != ' ' || is_function) && *function != '\0' &&
                (((is_numeric(*function)) || is_function) ||
                    (is_numeric(*(function - 1)) && *function == '('))) {
                // Checks if there is enough memory in the current string to store the next character.
                if (current_index + 2 > current_memory) {
                    // Reallocate extra memory for the current string.
                    current = realloc(current, (size_t)current_index + 2);
                    if (!current) {
                        printf("Memory Error, aborting...\n");
                        exit(-1);
                    }
                    // Updates the variable storing the amount of memory reserved for the current string.
                    current_memory = current_index + 2;
                }
                // Increments the is_function int if there is an opening parantheses after the first character,
                // as well as appends the current character in the function string to the end of the current string.
                if ((current[current_index] = *function) == '(') ++is_function;
                // If a closing parantheses is reached of a function is reached decrement the integer storing information
                // on whether the current string is a function
                // (if there happens to be a closing parantheses where there hasn't been
                // an opening parantheses returns false)
                else if (current[current_index] == ')') {
                    if (is_function > 0) --is_function;
                    else return false;
                }
                // Moves on to the next character of the function string.
                ++function;
                // Updates the index in the current string.
                ++current_index;
            }
            // Returns false if not all parantheses have been closed in the function.
            if (is_function) {
                return false;
            }
        }
        // Appends null terminator to the end of the current string.
        current[current_index] = '\0';

        // If a '-' char appears at the beginning of the function
        // change it to '~' in the current string.
        if (!previous && current[0] == '-') {
            current[0] = '~';
        }

        // If the first_character of the current string
        // is not an operator or an a parantheses treats as if it was a number
        if (is_numeric(current[0])) {
            // Returns false if the previous element in the function was a number.
            if (previous == 2) return false;

            // Sets the previous element to a number.
            previous = 2;

            // If the string is one character long and that one character
            // is the variable, passes.
            if (current[0] == variable && strlen(current) == 1) {}
            // If the string starts with 'e' and the string is one
            // character long passes.
            // If however the string starts with 'e' but is longer than
            // one character returns false.
            else if (current[0] == 'e') {
                if (strlen(current) == 1) continue;
                return false;
            }
            // If the string start with "pi" and is two chars long passes.
            // If the string starts with "pi" but is longer than two chars
            // returns false.
            else if (current[0] == 'p' && current[1] == 'i') {
                if (strlen(current) == 2) continue;
                return false;
            }
            // If the current string is a number pass.
            else if (is_number(&current[0])) {}
            // If none of the above conditions are met returns false if
            // the current string isn't a correct function
            // (or a function at all).
            else {
                if (!is_function_correct(current, variable)) return false;
            }
        }
        // If the string is an operator, check rules that apply to them.
        else if (current[0] != '(' && current[0] != ')') {
            // If the previous string was an operator or this is the first element of a function
            // returns false
            if (previous == 1 || (previous == 0 && current[0] != '~')) return false;
            // Sets previous to 1 (operator)
            previous = 1;
        }
        // If the current string is an opening parantheses
        // treats the expression inside the parantheses
        // as a sub_function 
        // (assuming the parantheses is closed properly).
        else if (current[0] == '(') {
            // Returns false if the last element in the function was a number.
            if (previous == 2) {
                return false;
            }
            // Sets the previous element to 2 (number).
            previous = 2;
            // Sets the temporaty index to 0.
            temp_index = 0;
            // Sets the is_function to 1 
            // (1 more opening parantheses than closing parantheses)
            is_function = 1;
            // While not all parantheses are closed and there are still characters
            // in the string continues reading characters until this requirement is met
            // or reaching the end of the string.
            while (is_function && function[temp_index] != '\0') {
                if (function[temp_index] == '(') ++is_function;
                else if (function[temp_index] == ')') --is_function;
                ++temp_index;
            }
            // If the requirement hasn't been met returns false.
            if (is_function) return false;
            // If the requirement has been met replaces the final closing parantheses
            // with a null terminator.
            function[temp_index - 1] = '\0';
            // Returns false if the expression inside the parantheses is incorrect.
            if (!is_correct(function, variable)) return false;
            // Adds back the closing parantheses.
            function[temp_index - 1] = ')';
            // Updates the function pointer to be placed after the parantheses.
            function += temp_index;
        }
        // Returns false if the current string is a closing parantheses.
        else return false;

        // Skips trailing spaces.
        while (*function == ' ') ++function;
    }
    // Frees memory allocated for the current string.
    free(current);
    // If the last element is a number returns true.
    if (previous == 2) return true;
    // Otherwise returns false.
    return false;
}


double evaluate_expression(char* function, double value, char variable) {

    // Defines a double stack that will be used for 
    // computing the value of the expression.
    stack_d* stack_for_computing = NULL;
    // Defines a variable that stores information on how much
    // memory is currently being used by the current string
    uint_fast16_t current_memory = 2;
    // Defines a char pointer that will point to 
    // newly allocated 2 bytes memory.
    char* current = malloc(current_memory);
    if (!current) {
        printf("Memory Error, aborting...\n");
        exit(-1);
    }
    // Defines the value that will store the number last extracted 
    // from the stack with the value NAN.
    double temp = NAN;
    // Defines an integer that will store information 
    // on whether or not the string currently being read is a function.
    uint_fast16_t is_function = 0;
    // Declares a variable that will store 
    // the index in the current string.
    uint_fast16_t index;
    // Defines a double variable that will store 
    // the number that was just read. 
    double number;
    // Removes leading spaces from the reverse polish notation.
    while (*function == ' ') {
        ++function;
    }
    // Reads the string until reaching the null terminator.
    while (*function != '\0') {
        // Resets the index to zero.
        index = 0;
        // Read the number or operator until reaching a space 
        // (unless it's inside of a function)
        while ((*function != ' ' || is_function) && *function != '\0') {
            // Allocates more memory for the function if necssary,
            // as well as updates the variable storing 
            // the number of bytes currently allocated.
            if (index > current_memory - 2) {
                current = realloc(current, (size_t)index + 2);
                if (!current) {
                    printf("Memory Error, aborting...\n");
                    exit(-1);
                }
                current_memory = index + 2;
            }
            // Adds 1 to the is_function integer if 
            // the current character is an opening parantheses.
            if ((current[index] = *function) == '(') ++is_function;
            // Subtracts 1 from the is_function integer 
            // if the current character is an opening parantheses.
            else if (*function == ')') --is_function;
            // Moves on to the next character in the function string.
            ++function;
            // Increments the index in the current string.
            ++index;
        }

        // Puts the null terminator at the end of the current string.
        current[index] = '\0';
        // Moves on to the next character in the function string
        // until reaching a non space character.
        while (*function == ' ') {
            ++function;
        }

        // If the current string isn't an operator calculates
        // the value of the expression.
        if (is_numeric(current[0])) {
            // If the string is the variable assigns the value 
            // of the the value of the variable to the number double.
            if (current[0] == variable) {
                number = value;
            }
            // If the string is "e" assigns the the value of e.
            else if (current[0] == 'e') {
                number = exp(1);
            }
            // If the string is "pi" assigns the value of pi.
            else if (index > 1 && current[0] == 'p' && current[1] == 'i') {
                number = PI;
            }
            // If the string is a number use sscanf to 
            // assign the value of the number.
            else if (isdigit(current[0])) {
                sscanf_s(current, "%lf", &number);
            }
            // If it's a function calculates it's value using
            // elementary_function function.
            else {
                number = elementary_function(current, value, variable);
            }
            // Push the resulting number onto the stack.
            stack_for_computing = push_d(stack_for_computing, number);
        }
        // If the string is an operator applies it to 
        // the numbers on the stack. 
        else {
            if (stack_for_computing == NULL) {
                return NAN;
            }
            // If the operator is negation negates the number 
            // atop the stack.
            if (current[0] == '~') {
                stack_for_computing->data = -stack_for_computing->data;
            }
            // If it's any other operator removes one number from the stack
            // and applies the operation to the number currently atop the stack
            // as well as the one last extracted
            // (in that order).
            else {
                stack_for_computing = pop_d(stack_for_computing, &temp);
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
    // Frees memory reserved for current.
    free(current);
    // Returns said number.
    return temp;
}


char* reverse_polish_notation(char* function) {

    // Defines a NULL pointer that will later point to 
    // the top of the stack used for transforming the function.
    stack_c* stack_for_transformation = NULL;
    // Defines a variable that starts of as the length of the function string 
    // and will be expanded if a need arises from the result string.
    uint_fast16_t length_of_function = (uint_fast16_t)strlen(function);
    // Defines the string that will contain the reverse polish notation of the given function,
    // starting with the same of length as the function.
    char* result = malloc(length_of_function);
    if (!result) {
        printf("Memory Error, aborting...");
        exit(-1);
    }
    // Defines a variable that contains the current index in the resulting string.
    uint_fast16_t index = 0;
    // Defines the integer that stores information on
    // whether the string currently being read is a function.
    uint_fast16_t is_function = 0;
    // Defines a variable that stores information on how many bytes of memory
    // are currently reserved for the current string.
    uint_fast16_t current_memory = 2;
    // Allocates memory for the string that will be read, as well as defines a pointer to it.
    char* current = malloc(current_memory);
    if (!current) {
        printf("Memory Error, aborting...");
        exit(-1);
    }
    // Declares the variable that stores the index in the string currently
    // being read from the function.
    uint_fast16_t current_index;
    // Declares the char that will hold the priority of the current operator.
    uint_fast16_t current_operator;
    // Declares the char that will hold the operator extracted from the stack.
    char temp;
    // While loop skips leading spaces in the function string.
    while (*function == ' ') {
        ++function;
    }
    // While loop goes through the entire function string.
    while (*function != '\0') {
        // Assigns the next character in the function string to the first character of the current string.
        current[0] = *function;
        // Resets current_index to 1.
        current_index = 1;
        // Moves on to the nest character in the function string.
        ++function;
        // Continues assigning function string characters to current string
        // if the first character isn't an operator or parantheses.
        if (is_numeric(current[0])) {
            // Continues assigning function string characters to current string
            // until the next character is a space, null terminator or isn't numeric, 
            // except for the situation where a letter is followed by an opening parantheses
            // meaning that the current string is a function
            // (the space and numeric rules don't apply if the string has already been marked as a function)
            while ((*function != ' ' || is_function) && *function != '\0' &&
                (((is_numeric(*function)) || is_function) ||
                    (is_numeric(*(function - 1)) && *function == '('))) {
                // Checks if there is enough memory in the current string to store the next character.
                if (current_index + 2 > current_memory) {
                    // Reallocate extra memory for the current string.
                    current = realloc(current, (size_t)current_index + 2);
                    if (!current) {
                        printf("Memory Error, aborting...\n");
                        exit(-1);
                    }
                    // Updates the variable storing the amount of memory reserved for the current string.
                    current_memory = current_index + 2;
                }
                // Sets the is_function bool to true if there is an opening parantheses after the first character,
                // as well as appends the current character in the function string to the end of the current string.
                if ((current[current_index] = *function) == '(') ++is_function;
                // If the end of a function is reached make a reverse polish notation
                // of the expression inside it and reset the integer storing information
                // on whether the current string is a function. 
                else if (current[current_index] == ')') {
                    --is_function;
                    if (!is_function) {
                        current[current_index] = '\0';
                        current = function_rpn(current, &current_memory, &current_index);
                    }
                }
                // Moves on to the next character of the function string.
                ++function;
                // Updates the index in the current string.
                ++current_index;
            }
        }
        // Changes the subtraction sign to tilde if it's meant as a negative of a number 
        if (((strlen(function) == (size_t)length_of_function - 1) || (*(function - 2) == '(')) && current[0] == '-') {
            current[0] = '~';
        }
        // Puts the null terminator at the end of the function string.
        current[current_index] = '\0';
        // Continues moving on to the next character until the current character isn't a space.
        while (*function == ' ') {
            ++function;
        }
        // If the current string is a number add it to the resulting string.
        if (is_numeric(*current)) {
            // For loop goes through the entire current string and adds it to the result string. 
            for (register uint_fast16_t i = 0; i < strlen(current); ++i) {
                // Allocates extra memory if necessary, 
                // updates the variable storing the current amount of bytes allocated.
                if (index > length_of_function - 1) {
                    result = realloc(result, (size_t)index + 1);
                    if (!result) {
                        printf("Memory Error, aborting...\n");
                        exit(-1);
                    }
                    length_of_function = index + 1;
                }
                // Adds the current character to the end of the result string.
                result[index] = current[i];
                // Updates the index in the result string.
                ++index;
            }
            // Allocate extra memory for the space if necessary,
            // updates the variable storing the amount of bytes allocated.
            if (index > length_of_function - 1) {
                result = realloc(result, (size_t)index + 1);
                if (!result) {
                    printf("Memory Error, aborting...\n");
                    exit(-1);
                }
                length_of_function = index + 1;
            }
            // Appends a space to the end of the result string.
            result[index] = ' ';
            // Updates the index in the result string.
            ++index;
        }
        // If the current string is an operator moves into the else if statement.
        else if (current[0] != '(' && current[0] != ')') {
            // Assigns the priority of the current operator to a dummy variable.
            current_operator = operation_priority(current[0]);
            // While loop continues until the bottom of the stack
            // (opening parantheses is the bottom of the stack).
            while (stack_for_transformation != NULL && stack_for_transformation->data != '(') {
                // Puts the operator on the stack if the operator
                // currently atop the stack has lower priority.
                if (operation_priority(stack_for_transformation->data) < current_operator) {
                    stack_for_transformation = push_c(stack_for_transformation, current[0]);
                    break;
                }
                // Extracts the next operator from the stack.
                stack_for_transformation = pop_c(stack_for_transformation, &temp);
                // Allocates extra memory if necessary and assign the new amount in bytes
                // to the corresponding variable.
                if (index > length_of_function - 2) {
                    result = realloc(result, (size_t)index + 2);
                    if (!result) {
                        printf("Memory Error, aborting...\n");
                        exit(-1);
                    }
                    length_of_function = index + 2;
                }
                // Appends the operator to the resulting string.
                result[index] = temp;
                ++index;
                // Appends a trailing space to the result string.
                result[index] = ' ';
                ++index;
            }
            // If the bottom of the stack is reached put the operator atop the stack.
            if (stack_for_transformation == NULL || stack_for_transformation->data == '(') {
                stack_for_transformation = push_c(stack_for_transformation, current[0]);
            }
        }
        // If the current string is an opening parantheses add it to the stack.
        else if (current[0] == '(') {
            stack_for_transformation = push_c(stack_for_transformation, current[0]);
        }
        // If the current string is a closing parantheses extract operators
        // from the stack until reaching the opening parantheses.
        else {
            while (stack_for_transformation != NULL && stack_for_transformation->data != '(') {
                stack_for_transformation = pop_c(stack_for_transformation, &temp);
                // Assigns extra memory if necessary.
                if (index > length_of_function - 2) {
                    result = realloc(result, (size_t)index + 2);
                    if (!result) {
                        printf("Memory Error, aborting...\n");
                        exit(-1);
                    }
                    length_of_function = index + 2;
                }
                // Adds the operator to the result string.
                result[index] = temp;
                ++index;
                // Appends a trailing space to the result string.
                result[index] = ' ';
                ++index;
            }
            // Removes the opening parantheses from the stack.
            stack_for_transformation = pop_c(stack_for_transformation, &temp);
        }
    }
    // Removes all remaining operators from the stack and add it to the current string.
    while (stack_for_transformation != NULL) {
        stack_for_transformation = pop_c(stack_for_transformation, &temp);
        if (index > length_of_function - 2) {
            result = realloc(result, (size_t)index + 2);
            if (!result) {
                printf("Memory Error, aborting...\n");
                exit(-1);
            }
            length_of_function = index + 2;
        }
        result[index] = temp;
        ++index;
        result[index] = ' ';
        ++index;
    }
    // Replaces the last space in the result string with a null terminator.
    result[index - 1] = '\0';
    // Frees memory reserved for the current string.
    free(current);
    // Returns the result string.
    return result;
}


static bool is_number(char* string) {

    // Defines a bool that stores information on whether or not
    // there has already been a coma in a real number.
    bool can_be_coma = true;
    // If the first character isn't a digit returns false.
    if (!isdigit(*string)) return false;
    // Moves on to the next character.
    ++string;
    // Checks if characters are part of a number.
    while (*string != '\0') {
        // Passes if the current character is a digit.
        if (isdigit(*string)) {}
        // Passes if the current character is the firs coma ('.')
        // in the string.
        else if (*string == '.' && can_be_coma) can_be_coma = false;
        // Returns false if neither of the above are met.
        else return false;
        // Moves on to the next character.
        ++string;
    }
    // If the function reaches the end returns true.
    return true;
}


static bool is_function_correct(char* expression, char variable_char) {

    // Defines a variable that will store information on the location
    // of the first character after the first opening parantheses.
    uint_fast16_t index = 0;

    // Finds the first opening parantheses in the expression.
    while (expression[index] != '(' && expression[index] != '\0') {
        ++index;
    }
    // Returns false if the there was no opening parantheses in the string.
    if (expression[index] == '\0') {
        return false;
    }
    // Returns false if the last character of the expression isn't an opening parantheses.
    if (expression[strlen(expression) - 1] != ')') {
        return false;
    }

    // Sets the final parantheses in the string to the null terminator.
    expression[strlen(expression) - 1] = '\0';
    // Sets the opening parantheses to the null terminator.
    expression[index] = '\0';
    // Sets the index to be the index of the first character after the first opening parantheses.
    ++index;

    // Switch statement that picks what to do based on the length of the function name.
    switch (index - 1) {
    case 2:
        // If the name length is 2, then if it's not "ln" returns false.
        if (strcmp(expression, "ln")) return false;
        // If it is "ln" checks if the expression inside the function is correct.
        return is_correct(&expression[index], variable_char);
    case 3:
        // Checks if the name matches any of the defined functions. If it does, checks the inside expression.
        // If it doesn't returns false.
        if (!strcmp(expression, "abs") || 
            !strcmp(expression, "log") || 
            !strcmp(expression, "sin") || 
            !strcmp(expression, "cos") || 
            !strcmp(expression, "tan") || 
            !strcmp(expression, "cot")) return is_correct(&expression[index], variable_char);
        return false;
    case 7:
        // Checks if the name matches any of the defined functions, if it does, checks the inside expression.
        if (!strcmp(expression, "arcsinh") ||
            !strcmp(expression, "arccosh") ||
            !strcmp(expression, "arctanh") ||
            !strcmp(expression, "arccoth")) return is_correct(&expression[index], variable_char);
        // FALLTHROUGH
    case 6:
        // Checks if the name matches any of the defined functions, if it does, checks the inside expression.
        if (index == 7 && (!strcmp(expression, "arcsin") ||
            !strcmp(expression, "arccos") ||
            !strcmp(expression, "arctan") ||
            !strcmp(expression, "arccot"))) return is_correct(&expression[index], variable_char);
        // FALLTHROUGH
    case 4:
        // Checks if the name matches any of the defined functions, if it does, checks the inside expression.
        if (index == 5 && (!strcmp(expression, "sqrt") ||
            !strcmp(expression, "sinh") ||
            !strcmp(expression, "cosh") ||
            !strcmp(expression, "tanh") ||
            !strcmp(expression, "coth"))) return is_correct(&expression[index], variable_char);
        // FALLTHROUGH
    default:
        // Checks if the first 3 letters match "log". If they do checks if it's followed by a number,
        // which is then followed by an expression inside the parantheses 
        // (which is correct).
        if (expression[0] == 'l' && expression[1] == 'o' && expression[2] == 'g') {
            if (is_number(&expression[3])) return is_correct(&expression[index], variable_char);
        }
        // If it's none of the above functions returns false.
        return false;
    }
}


static double elementary_function(char* expression, double argument, char argument_char) {

    // Defines a variable that will store the index of the beginning of the 
    // expression in the function.
    uint_fast16_t index = 0;
    // Defines a variable that will store the base of a logarithm if necessary.
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

    // Changes which functions to check based on the length of the name.
    switch (index - 1) {
    case 2:
        // If the length is 2 it has to be the natural logarithm.
        return log(evaluate_expression(&expression[index], argument, argument_char));
    case 3:
        // Checks the one letter that's enough to identify a function and calculates it's value.
        if (*expression == 'a') return fabs(evaluate_expression(&expression[index], argument, argument_char));
        if (*expression == 's') return sin(evaluate_expression(&expression[index], argument, argument_char));
        if (expression[2] == 's') return cos(evaluate_expression(&expression[index], argument, argument_char));
        if (*expression == 't') return tan(evaluate_expression(&expression[index], argument, argument_char));
        if (*expression == 'l') return log10(evaluate_expression(&expression[index], argument, argument_char));
        // If it's none of the above it's cot.
        else return 1 / tan(evaluate_expression(&expression[index], argument, argument_char));
    case 7:
        // Check if it's a hyperbolic cyclometric function by checking the first character.
        if (*expression == 'a') {
            // Checks the one letter that's enough to identify a function and calculates it's value.
            if (expression[3] == 's') return asinh(evaluate_expression(&expression[index], argument, argument_char));
            if (expression[5] == 's') return acosh(evaluate_expression(&expression[index], argument, argument_char));
            if (expression[3] == 't') return atanh(evaluate_expression(&expression[index], argument, argument_char));
            // If it's none of the aboce it's arccoth.
            else return (double)1 / 2 * log(1 + 2/(evaluate_expression(&expression[index], argument, argument_char) - 1));
        }
        // FALLTHROUGH
    case 6:
        // Check if it's a cyclometric function by checking the first character.
        if (index == 7 && *expression == 'a') {
            // Checks the one letter that's enough to identify a function and calculates it's value.
            if (expression[3] == 's') return asin(evaluate_expression(&expression[index], argument, argument_char));
            if (expression[5] == 's') return acos(evaluate_expression(&expression[index], argument, argument_char));
            if (expression[3] == 't') return atan(evaluate_expression(&expression[index], argument, argument_char));
            // If it's none of the above it's arccot.
            else return PI/2 - atan(evaluate_expression(&expression[index], argument, argument_char));
        }
        // FALLTHROUGH
    case 4:
        // If the name starts with 's' calculate the square root. 
        if (expression[1] == 'q') return sqrt(evaluate_expression(&expression[index], argument, argument_char));
        if (*expression == 's') return sinh(evaluate_expression(&expression[index], argument, argument_char));
        if (expression[2] == 's') return cosh(evaluate_expression(&expression[index], argument, argument_char));
        if (*expression == 't') return tanh(evaluate_expression(&expression[index], argument, argument_char));
        if (*expression == 'c') return (double)1 / tanh(evaluate_expression(&expression[index], argument, argument_char));
        // FALLTHROUGH
    default:
        // If it's none of the above it has to be a logarithm with a given base.
        // Gets the base of the logarithm.
        sscanf_s(&expression[3], "%lf", &temp);
        // Calculates the value of the function.
        return log(evaluate_expression(&expression[index], argument, argument_char)) / log(temp);
    }
}


static uint_fast16_t operation_priority(char operation) {
    uint_fast16_t priority;
    if (operation == '^') {
        priority = 3;
    }
    else if (operation == '~') {
        priority = 2;
    }
    else if (operation == '/' || operation == '*') {
        priority = 1;
    }
    else {
        priority = 0;
    }
    return priority;
}


static char* function_rpn(char* string, uint_fast16_t* max_length, uint_fast16_t* index) {

    // Declares a pointer that will point at the reverse polish notation of the inner function.
    char* temp_notation;
    // Defines an integer that will store in the index in the reverse polish notation of the inner function.
    uint_fast16_t temp_index = 0;

    // Resets the index in the function string.
    *index = 0;
    // Makes the index the first character after an opening parantheses
    // (the first character of the inside function).
    while (string[*index] != '(') {
        ++(*index);
    }
    ++(*index);

    // Makes a reverse polish notation of the inside function.
    temp_notation = reverse_polish_notation(&string[*index]);
    // Adds the reverse polish notation of the inside to the string.
    while (temp_notation[temp_index] != '\0') {
        if (*index > *max_length - 3) {
            string = realloc(string, (size_t)*index + 3);
            if (!string) {
                printf("Memory Error, aborting...\n");
                exit(-1);
            }
            *max_length = *index + 2;
        }
        string[*index] = temp_notation[temp_index];
        ++(*index);
        ++temp_index;
    }

    // Adds back the closing parantheses at the end of the string.
    string[*index] = ')';
    // Frees memory allocated for the reverse polish notation of the
    // inside function.
    free(temp_notation);
    // Returns the function with the inside expression in reverse polish
    // notation.
    return string;
}


static bool is_numeric(char current) {
    if (current == '+' || current == '-' ||
        current == '*' || current == '/' ||
        current == '~' || current == '^' ||
        current == '(' || current == ')') {
        return false;
    }
    return true;
}


static stack_d* push_d(stack_d* top, double element) {
    stack_d* result = malloc(sizeof(stack_d));
    if (!result) {
        printf("Memory Error, aborting...\n");
        exit(-1);
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
    stack_c* result = malloc(sizeof(stack_c));
    if (!result) {
        printf("Memory Error, aborting...\n");
        exit(-1);
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