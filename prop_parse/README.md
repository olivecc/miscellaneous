# Propositional Parser

A simple parser for propositional logic, implemented as required for the 
COMP0009 module of the Mathematical Computation MEng course at University 
College London.

The assignment is to parse a string according to the (simplified) syntax of 
propositional logic, and output:  

1. Whether the formula is valid, and  
2. The type of formula the string represents.  

The syntax of propositional logic expected by the assignment is expressed in
Backus-Naur Form (BNF) below:  

prop ::= p | q | r  
BC ::= v | ^ | >  
fmla ::= prop | - fmla | ( fmla BC fmla )  

Where *prop* represents a proposition, *BC* represents a binary connective,
*fmla* represents a formula, and whitespace is ignored.  

The solution is required to declare a function with signature 
'int parse(char\* g)', which parses a string *g* and depending on the formula 
type that *g* represents, returns a value of 0 for an invalid formula, 1 for a 
proposition, 2 for a negation, and 3 for a binary formula.  

Two different solutions are presented in the files 'prop\_parse.c' and 
'prop\_parse\_unrolled.c', representing a recursive solution and the 'unrolled' 
iterative version of it respectively, and a test driver program is supplied in 
'test.c'. 
