# Grammar:

## Operator Handling:

### Arithmetic Operator:

\+ (Addition)

\- (Subtraction)

\* (Multiplication)

/ (Division)

% (Modulo)

### Comparison Operator:

== (Equal)

!= (Not Equal)

\> (Greater than)

< (Less than)

\>= (Greater than or equal to)

<= (Less than or equal to)

### Logical Operators:

&& (Logical AND)

|| (Logical OR)

! (Not)

### Assignment Operators:

= (Assignment)

<< (Left Shift)

\>> (Right Shift)


## Literal Handling:

Numerical literals given in template.

" begins string literal token,

" ends string literal token as long as it is not escaped (\\" does not end token)

## Keyword/Identifier Handling

### Keywords:

repeat (initiates a loop that runs a specific command repeatedly)

until  (the post-condition that stops a loop)

if     (initiates a one step path that is traversed through if the right conditions are met)

### Identifiers:

An Identifier is a name for a variable, function, etc.

They are similar to literals with two key differences:

1. They MUST start with a letter, otherwise they're invalid.

2. They become identifiers when following a keyword that requires an identifier.


## Delimiters:

List of delimiters that the lexer accepts: 

- '('
- ')'
- '{'
- '}'
- '['
- ']'
- ','