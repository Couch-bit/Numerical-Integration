Basic Info:
It's written in Visual C, though the program can easily be made compatible 
with C99 by changing scanf_s (sscanf_s) to scanf (sscanf).
The Program uses Boole's formula to approximate the value of a definite
integral (by default for 1000 intervals, 
though this can be changed by modifying the constant inside the code of the project)
It reads the variable of integration, a function, 
integration bounds and prints the value of the definite integral.
The user is also asked if they want to continue calculating integrals 
at the end of the program.


Variable Prerequisites:
- must be a letter that's not 'e';

Function Prerequisites:
- can consist of elementary functions:
	- ln; 
	- abs;
	- sin;
 	- cos;
	- tan;
	- cot;
	- sinh;
	- cosh;
	- tanh;
	- coth;
	- log;
	- arcsin;
	- arccos;
	- arctan;
	- arccot;
	- arcsinh;
	- arccosh;
	- arctanh;
	- arccoth;
	- sqrt;
	- logNUMBER (NUMBER - any numerical value, will fail to evaluate if it's non-positive or equal to 1.
- can consist of numbers;
- can consits of constants:
	- e;
	- pi;
- can consist of variables;
- can consist of operators:
	- -
	- +
	- *
	- /
	- ^
- must be continuous on the interval of integration;
- must have correct syntax:
	- every operator follows and is followed by a number, constant, variable 
	(except for negation at the beginning of an expression, which only has to meat the latter);
	- a number cannot follow a number;
	- all open parantheses have to be closed and the expressions inside must have correct syntax;
	- every elementary function listed prior must be 
	followed by an expression inside of parantheses;
	- spaces between operators, parantheses and numbers, constants, variables are ignored.

Bound Prerequisites:
- Same as function but can't have variables;
- Upper bound can't be less or equal to the lower bound.

Consequences For inputing incorrect information:
- if the variable is incorrect asks for it to be inputed again;
- if the syntax is incorrect asks for the expression to be inputed again;
- if bounds are incorrect asks for them to be inputed again;
- if the upper bound is less or equal to the lower bound asks for it to be inputed again;
- if the function couldn't be evaluated at a point calculations end and a prompt is displayed.
