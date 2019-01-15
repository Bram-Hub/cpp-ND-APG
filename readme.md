# ND APG
## Authors
2014:
Richard Lee

## About
Generates a proof if one exists using the Fitch inference rules.
The program does not know disjunction elimination or contradiction introduction.

By default, it reads from the file proof.txt and outputs to the console.

main() is in project.cpp

The program accept the following command line arguments:
-i followed by a text file: specifies an alternate file for input
-o followed by a filename: specifies an output file
-d : turns on debug output

Recognized operators are:
and = &
or = |
negation= !
contradiction= ~
conditional= ->
biconditional= <->


Valid input syntax is:
(%1 OP %2)
a
OP 
OP(%1)

where:
%n is a valid clause
a is a literal in all lower case letters
OP is an operator consisting of upper case letters and symbols

note the space following 0-ary operators
ex:  (~  | a)
