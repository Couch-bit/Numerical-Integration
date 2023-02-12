#include "expression.h"


#define MAX_EXPRESSION_SIZE 1000
#define NUMBER_OF_INTERVALS 1000


// Clears stdin.
static void clear_stdin();
// Removes the newline from a string if there is one, otherwise 
// clears stdin.
static void remove_newline(char*);


int main(void) {

    char user_function[MAX_EXPRESSION_SIZE];
    char a_string[MAX_EXPRESSION_SIZE];
    char b_string[MAX_EXPRESSION_SIZE];
    char* function_rpn = NULL;
    char variable_char;
    char user_response = 'n';
    double a, b, h, y0, y1, y2, y3, y4, current_argument;
    double result;

    do {

        printf("Input the variable (single letter, cannot be e): ");
        // Reads a character from user, only
        // accepting letters that aren't 'e',
        // repeats until getting the correct character.
        while (!(scanf_s("%c", &variable_char, 1) == 1 &&
            variable_char != 'e' && isalpha(variable_char))) {
            printf("Illegal variable, try again: ");
            if (variable_char != '\n') clear_stdin();
        }
        clear_stdin();

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
        while (!(fgets(a_string, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(a_string),
                is_correct(a_string, '\0')) &&
            !(isnan(a = evaluate_expression
            (reverse_polish_notation(a_string), 0, '\0')) ||
                isinf(a)))) {
            puts("Invalid bound, try again:");
        }

        puts("Input the upper integration bound:");
        // Reads the upper integration bound from user, repeats until
        // reading a correct expression that has a
        // correct numerical value that's
        // greater than the value of the lower integration bound.
        while (!(fgets(b_string, MAX_EXPRESSION_SIZE, stdin) &&
            (remove_newline(b_string),
                is_correct(b_string, '\0')) &&
            (b = evaluate_expression
            (reverse_polish_notation(b_string), 0, '\0'),
                !(isnan(b) || isinf(b))) && b > a)) {
            puts("Invalid bound, try again:");
        }

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
                puts("Failed to evaluate integral, make sure the"
                    "function is defined for the entire interval"
                    "as well as its edges");
                break;
            }
        }

        printf("The value of the integral of the function:"
            "\n%s\non the interval [%f; %f] is: \n%f\n",
            user_function, a, b, result);

        free(function_rpn);

        printf("Continue calculating integrals? (y/n): ");

        while (!(scanf_s("%c", &user_response, 1) == 1 &&
            (user_response == 'y' || user_response == 'n'))) {
            printf("Illegal response, try again: ");
            if (user_response != '\n') clear_stdin();
        }

        clear_stdin();
    } while (user_response == 'y');

    return 0;
}


static void clear_stdin() {

    static int temp = '\n';

    while ((temp = getchar()) != '\n' && temp != EOF) {}
}


static void remove_newline(char* string) {

    char* temp = &string[strlen(string) - 1];

    if (*temp == '\n') {
        *temp = '\0';
    }
    else clear_stdin();
}
