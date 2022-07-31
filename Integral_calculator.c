#include "expression.h"


// Defines the max expression size.
#define MAX_EXPRESSION_SIZE 1000
// Defines the number of intervals used for numerical integration.
#define NUMBER_OF_INTERVALS 1000


// Clears stdin.
static void clear_stdin();
// Removes the newline from a string if there is one, otherwise 
// clears stdin.
static void remove_newline(char*);


int main(void) {

    // Defines a string that will store the function given by the user.
    char user_function[MAX_EXPRESSION_SIZE];
    // Defines a string that will store the expression that's the lower integration bound.
    char a_string[MAX_EXPRESSION_SIZE];
    // Defines a string that will store the expression that's the upper integration bound.
    char b_string[MAX_EXPRESSION_SIZE];
    // Declares a pointer that will store the adress to the
    // reverse polish notation of the function.
    char* function_rpn;
    // Declares a char that will store the variable of integration.
    char variable_char;
    // Defines a char that will store the response given by the user
    // when asked if they want to continue.
    char user_response = 'n';
    // Declares values that will respectively store
    // lower integratin bound, upper integration bound, distance between arguments,
    // the five function values inside an interval and the current argument.
    double a, b, h, y0, y1, y2, y3, y4, current_argument;
    // Declares a double that will store the result of the definite integral.
    double result;

    do {
        // Asks the user to input the variable of integration.
        printf("Input the variable (single letter, cannot be e): ");
        // Reads a character from user, only accepting letters that aren't 'e',
        // repeats unitl getting the correct character.
        while (!(scanf_s("%c", &variable_char, 1) == 1 &&
            variable_char != 'e' && isalpha(variable_char))) {
            printf("Illegal variable, try again: ");
            if (variable_char != '\n') clear_stdin();
        }
        // Clears stdin.
        clear_stdin();

        // Asks the user to input a function.
        printf("Input the function:\n");
        // Reads the function from user, repeats the process until 
        // the function is correct.
        while (!((fgets(user_function, MAX_EXPRESSION_SIZE, stdin) != NULL ||
            (clear_stdin(), false)) &&
            (remove_newline(user_function), is_correct(user_function, variable_char)))) {
            printf("Invalid function, try again:\n");
        }

        // Makes a reverse polish notation of the function given.
        function_rpn = reverse_polish_notation(user_function);

        // Asks the user for the lower integration bound.
        printf("Input the lower integration bound:\n");
        // Reads the lower integration bound from user and calculates it's numerical value, 
        // continues until the expression and the value are correct.
        while (!((fgets(a_string, MAX_EXPRESSION_SIZE, stdin) != NULL ||
            (clear_stdin(), false)) &&
            (remove_newline(a_string), is_correct(a_string, '\0')) &&
            !(isnan(a = evaluate_expression(reverse_polish_notation(a_string), 0, '\0')) || isinf(a)))) {
            printf("Invalid bound, try again:\n");
        }

        // Asks the user to input the upper integration bound
        printf("Input the upper integration bound:\n");
        // Reads the upper integration bound from user, repeats until 
        // reading a correct expression that has a correct numerical value that's
        // greater than the value of the lower integraion bound.
        while (!((fgets(b_string, MAX_EXPRESSION_SIZE, stdin) != NULL ||
            (clear_stdin(), false)) &&
            (remove_newline(b_string), is_correct(b_string, '\0')) &&
            (b = evaluate_expression(reverse_polish_notation(b_string), 0, '\0'),
                !(isnan(b) || isinf(b))) && b > a)) {
            printf("Invalid bound, try again:\n");
        }

        // Defines the distance between arguments that need to be calculated. 
        h = (b - a) / (NUMBER_OF_INTERVALS * 4);
        // Sets the current argument to the beginning of the integration interval.
        current_argument = a;
        // Defines the result to be 0 at the beginning.
        result = 0;

        // Loops through all the intervals.
        for (register unsigned short i = 0; i < NUMBER_OF_INTERVALS; ++i) {
            // Calculates the value of the function at the 5 points.
            y0 = evaluate_expression(function_rpn, current_argument, variable_char);
            current_argument += h;
            y1 = evaluate_expression(function_rpn, current_argument, variable_char);
            current_argument += h;
            y2 = evaluate_expression(function_rpn, current_argument, variable_char);
            current_argument += h;
            y3 = evaluate_expression(function_rpn, current_argument, variable_char);
            current_argument += h;
            // Adds a failsafe that will reset the value of the current argument back to b
            // if it exceeds it.
            if (current_argument > b) {
                current_argument = b;
            }
            y4 = evaluate_expression(function_rpn, current_argument, variable_char);
            // Calculates the result for the interval with Boole's formula.
            result += 2 * h / 45 * (7 * y0 + 32 * y1 + 12 * y2 + 32 * y3 + 7 * y4);
            // if the result isn't a correct numerical value inform the user and break out of the loop.
            if (isnan(result) || isinf(result)) {
                printf("Failed to evaluate integral, make sure the function is defined for the entire interval as well as its edges\n");
                break;
            }
        }

        // Displays the value of the integral.
        printf("The value of the integral of the function: \n%s\non the interval [%lf; %lf] is: \n%lf\n",
            user_function, a, b, result);
        // Frees memory allocated for the reverse polish notation of the function.
        free(function_rpn);

        // Asks the user whether or not to keep going.
        printf("Continue calculating integrals? (y/n): ");
        // Reads the response from the user.
        while (!(scanf_s("%c", &user_response, 1) == 1 && (user_response == 'y' || user_response == 'n'))) {
            printf("Illegal response, try again: ");
            if (variable_char != '\n') clear_stdin();
        }
        // clears stdin.
        clear_stdin();
        // Continue reading integrals if the user answered 'y'
        // otherwise end the program.
    } while (user_response == 'y');
    return 0;
}


static void clear_stdin() {
    static char temp;
    while ((temp = getchar()) != '\n' && temp != EOF) {}
}


static void remove_newline(char* string) {
    if (string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
    else {
        clear_stdin();
    }
}