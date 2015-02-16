## CPPGM Programming Assignment 3 (ctrlexpr)

### Overview

Write a C++ application called `ctrlexpr` that accepts a _C++ Source File_ on standard input that contains Controlling Expressions (defined below) for Conditional Inclusion, one per logical line.

The input file does NOT include any...

 - preprocessing directives
 - pre-defined macro names
 - the pragma operator

Evaluate each controlling expression and return the results in the specified format.

`ctrlexpr` should execute translation phases 1, 2, 3 to delimit logical source lines.

### Prerequisites

You should complete Programming Assignment 2 before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa3.git

It contains a stub implementation of `ctrlexpr` with some _optional_ starter code, a compiled reference implementation and a test suite.

You will also want to reuse most of your code from PA2.

### Mock `defined` Implementation

As we do not have any macros defined in PA3, in order to test the `defined` operator, we will use a mock implementation with the following functionality:

If the first UTF-8 code unit of the identifier is odd than return true, else return false.

There is an implementation of this mock function called `PA3Mock_IsDefinedIdentifier` in the starter code.

### Input Format

The C++ Source File shall be read from standard input `std::cin` in UTF-8 format as per PA1.

Each logical line will contain a (possibly invalid, possibly empty) controlling expression after phase 3.

### Output Format

For each logical line of the input file `ctrlexpr` should output zero or one line of output.

In the event the logical source line is empty (contains only whitespace or nothing) after phase 3, `ctrlexpr` should not output a line.

If the logical source line contains an invalid controlling expression (one that contains invalid tokens or does not match the grammar or semantics of a controlling expression), `ctrlexpr` should output one line containing `error`.

In the event the logical source line contains a valid controlling expression, `ctrlexpr` should output a decimal literal as the result of the controlling expression.  In the case that the output is signed it should have no suffix.  In the event that the output is unsigned it should have the suffix `u` (lowercase).

At the end of output, `ctrlexpr` should output `eof` on a line by itself.

### Error Reporting

If an error occurs in phases 1, 2 or 3 `main` should `return EXIT_FAILURE` as per the behaviour of `pptoken` in PA1.

For all other errors the line should be output as an `error` as specified in Output Format (above).

In particular if any of the following conditions are true it is course defined for this assignment to output `error`:

- The right operand of division or modulous is zero
- The right operand of a shift operator is negative or greater than or equal to 64

### Example

For an input of:

    2 + 3
    2u + 3
    2 + 3 == 5
    2 + 3 == 4
    /* blank line */
    2 + == 4

Produces an output of:

    5
    5u
    1
    0
    error
    eof

### Restrictions

As per PA1

### Definition: Controlling Expression

A controlling expression is the sequence of tokens that appears in the Conditional Inclusion Preprocessing Directive:

    #if <controlling-expression> new-line
    #elif <controlling-expression> new-line

If using the token names from PA2 it should match the Controlling Expression Grammar (given below).

The two missing productions from the grammar are `integral-literal` and `identifier_or_keyword`.

We shall define them here:

An `integral-literal` is a `literal` (not user defined) of a non-array integral type.  Integral types are the subset of the Fundamental Types given here:

    signed char
	short int
	int
	long int
    long long int
    unsigned char
    unsigned short int
    unsigned int
    unsigned long int
    unsigned long long int
    wchar_t
    char
    char16_t
    char32_t
    bool

Some types that are NOT integral types are:

    float
    double
    long double
    void
    nullptr_t
    array of <anything>
    
An `identifier_or_keyword` is simply an identifier in `preprocessing-token` context (meaning the `identifier` from PA1 and not from PA2).  In particular it treats keywords the same as identifiers.  There are two special cases.  They are:

 - `true`, which is effectively evaluated as `1`
 - `defined` which has special meaning in a controlling expression

> Note: For PA3, in the context of a preprocessor controlling expression only, we shall course-define the treatment of the identifier `true` to be evaluated the same as the integer literal `1`, and the identifier `false` to be evaluated the same as the integer literal `0`.  Note that this may conflict slightly with the standard requirements.
 
At this point try to read 16.1 Conditional Inclusion, there will be many "forward" references to terminology that has not been required reading in the course yet, so don't worry if you don't understand everything.  The parts which you do understand should be sufficient to complete this assignment.

### Controlling Expression Grammar

	primary-expression:
	    integral-literal      // see above
	    OP_LPAREN controlling-expression OP_RPAREN
	    defined identifier_or_keyword     			  // see above:  defined id
	    defined OP_LPAREN identifier_or_keyword OP_RPAREN       // alternative form: defined(id)
	    identifier_or_keyword		// evaluate as `0`

	unary-expression:
	    primary-expression
	    OP_PLUS unary-expression
	    OP_MINUS unary-expression
	    OP_LNOT unary-expression
	    OP_COMPL unary-expression
	
	multiplicative-expression:
	    unary-expression
	    multiplicative-expression OP_STAR unary-expression
	    multiplicative-expression OP_DIV unary-expression
	    multiplicative-expression OP_MOD unary-expression
	
	additive-expression:
	    multiplicative-expression
	    additive-expression OP_PLUS multiplicative-expression
	    additive-expression OP_MINUS multiplicative-expression
	
	shift-expression:
	    additive-expression
	    shift-expression OP_LSHIFT additive-expression
	    shift-expression OP_RSHIFT additive-expression
	
	relational-expression:
	    shift-expression
	    relational-expression OP_LT shift-expression
	    relational-expression OP_GT shift-expression
	    relational-expression OP_LE shift-expression
	    relational-expression OP_GE shift-expression
	
	equality-expression:
	    relational-expression
	    equality-expression OP_EQ relational-expression
	    equality-expression OP_NE relational-expression
	
	and-expression:
	    equality-expression
	    and-expression OP_AMP equality-expression
	
	exclusive-or-expression:
	    and-expression
	    exclusive-or-expression OP_XOR and-expression
	
	inclusive-or-expression:
	    exclusive-or-expression
	    inclusive-or-expression OP_BOR exclusive-or-expression
	
	logical-and-expression:
	    inclusive-or-expression
	    logical-and-expression OP_LAND inclusive-or-expression
	
	logical-or-expression:
	    logical-and-expression
	    logical-or-expression OP_LOR logical-and-expression
	
	controlling-expression:
	    logical-or-expression
	    logical-or-expression OP_QMARK controlling-expression OP_COLON controlling-expression
	    
Notice that this grammar is unambiguous and has the precedence and associativity of operators built into it.

### Features

The following steps do not necessarily need to occur in the order specified, however `ctrlexpr` should behave _as if_ they had.

First apply phase 1-3 to get a stream of `preprocessing-tokens`.

Then split them by the `new-line` token.  Discard/ignore `whitespace-sequences`.

For each non-empty sequence of `preprocessing-tokens` `ctrlexpr` will output one line, either (a) a decimal number; (b) a decimal number followed by `u`; or (c) `error`.

Apply some of the "post-tokenization" code from PA2 to get a sequence of `tokens` (paying attention to the `identifier_or_keyword` case).

Check that there are no `invalid` tokens and that all `literal` tokens are of `integral-literal` type (defined above).  If not output `error`.

As per 16.2.4:

> For the purposes of this token conversion and evaluation all signed and unsigned integer types act as if they have the same representation as, respectively, `intmax_t`
or `uintmax_t`

`intmax_t` and `uintmax_t` are in `#include <cstdint>`.  On the standard platform they are typedefs `long int` and `unsigned long int`, which are both 64-bit.

> We will course-define the interpretation of this to mean that each `integral-literal` is interpreted as per its phase 7 PA2 post-tokenization type first.  Then after that, if it is signed it is promoted to `intmax_t`, and if it is unsigned it is promoted to `uintmax_t`.  For this course footnote 148 shall be considered a standard defect, and should be ignored.

Promote each `integral-literal` to one of these two types depending on if it is signed or unsigned.

It is course-defined (and by the ABI and bootstrap) that following integral types are signed:

    bool
    wchar_t
    char
    signed char
	short int
	int
	long int
    long long int

and the following are unsigned:

    unsigned char
    unsigned short int
    unsigned int
    unsigned long int
    unsigned long long int
    char16_t
    char32_t

This can also be looked up with `std::numeric_limits<T>::is_signed`

Parse the token sequence by the `controlling-expression` grammar into a parse tree.  If it does not match the grammar output "error".

Walk the tree in post-order evaluating each sub-expression up to the root.  The behaviour of each of the used operators are documented in clause 5 of the standard.  You do not need to read the entire clause, just the parts that deal with those operators used in the controlling expression grammar.

You should read and understand the concepts of _Integral Promotion_ (4.7), _Integer Conversion Rank_ (4.13) and the _Usual Arithmetic Conversions_ (5.0.10).

After each sub-expression is evaluated promote its type to `intmax_t` or `uintmax_t` (depending on whether it signed) before further evaluation.

It is course defined that:

- signed integral types (2s-compliment) are sign extended when promoted to a larger type.
- right shift operations on a signed left operand sign preserve (shift in bits the same as sign bits)

### Design Notes (Optional)

One way to solve PA3 is to create another implementation of `IPPTokenStream` for PA3 that accumulates `preprocessing-tokens` until it gets `new-line` and then sends them off for evaluation and "post-tokenization".  You can create some other class like `CtrlExprEvaluator` that parses and evaluates these sequences of tokens.

To match the sequence of tokens to the grammar you will need to write a parser.  It is recommended to read chapter 4 of the dragon book if you have it.

Although the PA3 grammar is LR(1), and you could in theory solve the assignment by writing an LR parser generation tool that takes the grammar and automatically generates a parser for it, that is _not recommended_ for this assignment.  There are two reasons.  The first is that it will take longer to implement than the recommended way.  The second is that although an LR parser is easy for this particular grammar, for C++ as a whole it is not the proven approach.

Most agree that a top-down approach makes it easier to deal with the special cases needed to parse C++.  For example, both GCC and Clang use a hand-written top-down approach.  It is true that some have succeeeded in writing LR parsers for C++ using disambiguation techniques like GLR, however we will be recommending and supporting "the typical top-down approach" in this course.  You should understand that there is contention on this point.

> Side Note: If you are determined to ignore our advice and use LR parsing than you may hand-write an LR parser generation tool, and build and execute this tool from your Makefile.  As per the usual course restrictions, you may not use a third-party parser generation tool - you have to write it yourself.  The algorithms to do this are described in the second half of chapter 4 of the dragon book.  GLR is not covered in the dragon.  GLR just means that you solve shift/reduce and reduce/reduce conflicts by searching both options in parallel - but no such conflicts exist in this particular grammar.  To LR parse the larger C++ grammar you will need GLR.

The recommended way to parse is to write a hand-written predictive top-down parser.  To deal with the left recursion in the PA3 grammar, you simply use iteration and manually preserve the associativity.

For example:

    T -> X
    T -> T + X

Such a production will match `X`, `X + X`, `X + X + X` and so on.  So we first parse `X`, then we lookahead to see if there is a `+`.  If there is, we parse `+ X` and repeat.  If not we return what we have.  As the operator is left associative we build our parse tree (or evaluation order) leftward.

If you implement the parser in this way than in order to parse a simple expression like `42`, it will result in a deep call stack from `controlling-expression` all the way up to `primary`.  That is ok as a solution to this assignment, we are not too worried about performance.  You should be aware that there is a technique in which you can collapse the calls to all the different binary operator expressions into one call.  This is achieved by keeping a precedence table and making decisions about how to build the parse tree based upon it.  If you have extra time after finishing this assignment, we encourage you to look into and implement this optimization.

It is also recommended that you keep your controlling expression parsing code separate from your future C++ expression parsing code.  Although you could use some common base for both, there are significant differences that are most likely not worth creating an abstract base for.  It will be better to keep the PA3 code encapsulated within your preprocessor.
