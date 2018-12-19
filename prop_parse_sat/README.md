# Propositional Parser

A simple parser/satisfiability decider for propositional logic written in C99, 
implemented as required for the COMP0009 module of the Mathematical Computation 
MEng course at University College London.

The assignment is to take a string (of at most 50 characters) and output whether 
the formula is:

1. Legal under the (simplified) syntax of propositional logic, and
2. Satisfiable (i.e. whether there exists a valuation of propositions *p*, *q*, 
   and *r* such that the formula is true).  

The syntax of propositional logic expected by the assignment is expressed in
Backus-Naur Form (BNF) below:  

prop ::= p | q | r  
BC   ::= v | ^ | >  
fmla ::= prop | - fmla | ( fmla BC fmla )  

Where *prop* represents a proposition, *BC* represents a binary connective,
*fmla* represents a formula, and whitespace is ignored.  

The solution is implemented in *prop.c* and exposed through *prop.h*. A test 
driver program is supplied in *test.c*.
