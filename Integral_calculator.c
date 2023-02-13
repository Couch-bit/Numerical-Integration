#include "expression.h"


#define MAX_EXPRESSION_SIZE 1000
#define NUMBER_OF_INTERVALS 1000


// Clears stdin.
inline static void clear_stdin();
// Removes the newline from a string if there is one, otherwise 
// clears stdin.
inline static void remove_newline(char*);


int main(void) {

    char user_function[MAX_EXPRESSION_SIZE];
    char buffer[MAX_EXPRESSION_SIZE];
    char* bound_rpn = NULL;
    char* function_rpn = NULL;
    char user_response = 'n';
    char variable_char;
    double a, b, h, y0, y1, y2, y3, y4, current_argument;
    double result;

    do {

        printf("Input the variable (single letter, cannot be e): ");
        // Reads a character from user, only
        // accepting letters that aren't 'e',
        // repeats until getting the correct character.
        while (!(fgets(buffer, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(buffer), 
                buffer[0] != 'e' && isalpha(buffer[0])))) {
            printf("Illegal variable, try again: ");
            
        }
        variable_char = buffer[0];
        

        puts("Input the function:");
        // Reads the function from user, repeats the process until 
        // the function is correct.
        while (!(fgets(user_function, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(user_function),
                is_correct(user_function, variable_char)))) {
            puts("Invalid function, try again:");
        }

        // Makes a reverse polish notation of the function given.
        function_rpn = reverse_polish_notation(user_function);
        if (!function_rpn) {
            puts("Memory error, aborting...");
            return 1;
        }

        puts("Input the lower integration bound:");
        // Reads the lower integration bound from user and
        // calculates it's numerical value, 
        // continues until the expression and the value are correct.
        while (!(fgets(buffer, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(buffer),
                is_correct(buffer, '\0')) &&
            !(isnan(a = evaluate_expression
            (bound_rpn = reverse_polish_notation(buffer),
                0, '\0')) ||
                isinf(a)))) {
            if (bound_rpn) {
                free(bound_rpn);
                bound_rpn = NULL;
            }
            puts("Invalid bound, try again:");
        }
        free(bound_rpn);
        bound_rpn = NULL;

        puts("Input the upper integration bound:");
        // Reads the upper integration bound from user, repeats until
        // reading a correct expression that has a
        // correct numerical value that's
        // greater than the value of the lower integration bound.
        while (!(fgets(buffer, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(buffer),
                is_correct(buffer, '\0')) &&
            (b = evaluate_expression
            (bound_rpn = reverse_polish_notation(buffer),
                0, '\0'),
                !(isnan(b) || isinf(b))) && b > a)) {
            if (bound_rpn) {
                free(bound_rpn);
                bound_rpn = NULL;
            }
            puts("Invalid bound, try again:");
        }
        free(bound_rpn);
        bound_rpn = NULL;

        // Defines the distance between arguments.
        h = (b - a) / (NUMBER_OF_INTERVALS * 4);

        current_argument = a;
        result = 0;

        // Loops through all the intervals.
        for (register uint_fast16_t i = 0;
            i < NUMBER_OF_INTERVALS; ++i) {

            // Calculates the value of the function at the 5 points.
            y0 = evaluate_expression
            (function_rpn, current_argument, variable_char);
            current_argument += h;
            y1 = evaluate_expression
            (function_rpn, current_argument, variable_char);
            current_argument += h;
            y2 = evaluate_expression
            (function_rpn, current_argument, variable_char);
            current_argument += h;
            y3 = evaluate_expression
            (function_rpn, current_argument, variable_char);
            current_argument += h;

            // Adds a failsafe that will reset the value of the
            // current argument back to b if it exceeds it.
            if (current_argument > b) {
                current_argument = b;
            }

            y4 = evaluate_expression
            (function_rpn, current_argument, variable_char);

            // Calculates the result for the
            // interval with Boole's formula.
            result += 2 * h / 45 * 
                (7 * y0 + 32 * y1 + 12 * y2 + 32 * y3 + 7 * y4);

            if (isnan(result) || isinf(result)) {
                puts("Failed to evaluate integral, make sure the "
                    "function is defined for the entire interval "
                    "as well as its edges");
                break;
            }
        }

        printf("The value of the integral of the function:"
            "\n%s\non the interval [%f; %f] is: \n%f\n",
            user_function, a, b, result);

        free(function_rpn);

        printf("Continue calculating integrals? (y/n): ");

        while (!(fgets(buffer, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(buffer),
               buffer[0] == 'y' || buffer[0] == 'n'))) {
            printf("Illegal response, try again: ");
        }
        user_response = buffer[0];

    } while (user_response == 'y');

    return 0;
}


inline static void clear_stdin() {

    static int temp = '\n';

    while ((temp = getchar()) != '\n' && temp != EOF) {}
}


inline static void remove_newline(char* string) {

    char* temp = &string[strlen(string) - 1];

    if (*temp == '\n') {
        *temp = '\0';
    }
    else if (!feof(stdin)) clear_stdin();
}
