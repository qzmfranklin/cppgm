## CPPGM Programming Assignment 4 (macro)

### Overview

Write a C++ application called `macro` that accepts a _C++ Source File_ on standard input and executes phases 1 through 6 and the tokenization part of phase 7.

The input file may contain:

- `#define` preprocessing directives
- `#undef` preprocessing directives

The input file does NOT include any...

 - preprocessing directives other than `#define` or `#undef`
 - pre-defined macro names
 - the pragma operator

Output the resulting tokens as per PA2 `posttoken`.

### Prerequisites

You should complete Programming Assignment 2 before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa4.git

It contains a stub implementation of `macro`, a compiled reference implementation and a test suite.

You will also want to reuse most of your code from PA2.

### Input Format

The C++ Source File shall be read from standard input `std::cin` in UTF-8 format as per PA1 `pptoken`.

### Output Format

The tokens shall be output as per PA2 `posttoken`.

### Error Reporting

If an error occurs `macro` should `return EXIT_FAILURE` as per the behaviour of `pptoken` in PA1.

### Example

For an input of:

    #define a 3
    #define f(x) x x
    f(a)
    
Produces an output of:

    literal 3 int 03000000
    literal 3 int 03000000
    eof
  
### Restrictions

As per PA1

### Features

Once you have executed phase 1-3 as per PA1 `pptoken` to produce a sequence of `preprocessing-tokens`, you will divide them into preprocessing directives and `text-sequences`.

Preprocessing directives can be identified by their prefix, one of:

    start-of-file #
    start-of-file whitespace-sequence #
    new-line #
    new-line whitespace-sequence #

Where `start-of-file` denotes the beggining of the token sequence.

Any of these patterns must start a preprocessing directive.  The preprocessing directive is terminated by the next `new-line`.

The only preprocessing directives for PA4 will be macro defines and undefs:

    // object-like macros
	# define identifier new-line
	# define identifier must-whitespace replacement-list new-line
	
	// function-like macros
	# define identifier no-whitespace ( ) replacement-list new-line
	# define identifier no-whitespace ( identifier-list ) replacement-list new-line
	# define identifier no-whitespace ( ... ) replacement-list new-line
	# define identifier no-whitespace ( identifier-list , ... ) replacement-list new-line

    // undefine macro
	# undef identifier new-line
	
`no-whitespace` means that there is no `whitespace-sequence` between the two adjacent tokens.  `must-whitespace` means there _must_ be a `whitespace-sequence` between the two adjacent tokens. Otherwise `whitespace-sequence` may optionally appear between any tokens.

A `replacement-list` is a possibly empty sequence of any `preprocessing-tokens` apart from `new-line`.

An `identifier-list` is a comma-separated sequence of `identifiers`.

`#define` directives define macros, `#undef` directives undefine them.

A `text-sequence` is a maximal sequence of tokens that is not part of a preprocessing directive.

Each `text-sequence` must be macro replaced using the currently defined set of macros at that point in the file.  Once this has been done the macro replaced `preprocessing-tokens` should be processed by phase 5-7 as per PA2 `posttoken` and output.

The rules for macro replacement are described in section _16.3 Macro Replacement_ of the standard.

> If the name of the macro being replaced is found during this scan of the replacement list (not including the
rest of the source file’s preprocessing tokens), it is not replaced. Furthermore, if any nested replacements
encounter the name of the macro being replaced, it is not replaced. These nonreplaced macro name preprocessing
tokens are no longer available for further replacement even if they are later (re)examined in contexts
in which that macro name preprocessing token would otherwise have been replaced.

We shall course-define two invocations X and Y to be nested with respect to each other if the head macro name `identifier` token of Y was part of the replaced tokens of the X invocation.  This nestedness relationship shall not survive parameter substitution:

    #define f(x) 1 g(x)
    #define g(x) 2 f(x)
    
    f(f(x))
    
The trace of the above is:

    f(f(x))
    f(1 g(x))
    f(1 2 f(x))
    1 g(1 2 f(x))
    1 2 f(1 2 f(x))

However a macro name token that is not replaced due to nesting is never replaced again, even after substitution as:

    #define z z[0]
    #define f(x) 1 x
    
    f(z)
    
The trace is:

    f(z)
    f(z[0])
    1 z[0]

z remains unavailable for invocation even after it is substituted in a parameter.

Finally we show:

    #define f(x) 1 x
    #define g(x) 2 x

    g(f)(g)(3)

The trace is:

    g(f)(g)(3)
    2 f(g)(3)  <-- f is nested with respect to g as the token is part of the previous g invocation
    2 1 g(3)   <-- g is nested with respect to f, therefore it is not replaced (g -> f -> g)

Note that this course-defined interpretation of the nesting semantics is not necessarily the same one used by all other preprocessors (in particular gcc).

### Design Notes (optional)

As usual these design notes only describe one possible way to do this assignment, and are entirely optional.

First of all, it is recommended that you read 16.3 a couple of times before you continue reading these notes, and certainly before you start writing code.  Some find the logic of macro replacement quite challenging to implement, so be prepared for this.

You can start by making a `PreprocessingToken` class that packs up the output of `IPPTokenStream`.

Perform a simple top-down predictive parse to get a stream of preprocessing directives and `text-sequences`.

Keep a table of currently defined macros.

When you encounter a `#define` add the macro to the table.  If it currently exists check to see that the `identifier-list` and `replacement-list` is the same as the currently defined one.  It is an error if they do not match.

When you encounter an `#undef` remove the macro from the table.

When you encounter a `text-sequence` you need to process any macro invocations it contains.

As a first step note that `new-lines` are not significant once a `text-sequence` has been identified, so you can collapse `new-lines` into `whitespace-sequence` at this point.  After that you will also want to collapse each resulting contiguous sequences of `whitespace-sequences` into a singular `whitespace-sequence`.

As you find macro invocations you will invoke them and replace the invocation with a new sequence of tokens.  These new tokens have to be _rescanned_.

Consider the following:

	#define f(x) 1 x (
	#define g(x) 2 x
	f(g)b)

The `text-sequence` is as follows:

    f(g)b)
    
After the first invocation `f(g)` is replaced by `1 g (` resulting in:

    1 g (b)
    
This must be rescanned, and a new invocation is found.  That of `g (b)`, which is replaced by `2 b`:

    1 2 b
    
Given the nature of that operation, a good data structure to use is a stack, where the top of the stack is the start of the text sequence (ie token order is in reverse).  That way when an invocation is replaced, its tokens can be popped off the top of the stack and the replacement list can be pushed onto it, and processing can continue from the top of the stack again.

After you identify a function-like macro invocation and its arguments, there are three possible transformations you may need to perform on each argument before they are used:

- the argument is used as is
- the argument is macro replaced before being used
- the argument is stringized

Which of the three options is applied depends on the usage of the parameter within the `replacement-list` of the macro.  If it follows a `#`, the argument is stringized.  If it is next to a `##` the argument is used as is.  If neither, than it is macro replaced before being used.

The rules for nesting and recursion are difficult to understand, and even more difficult to implement.

We suggest keeping with each identifier token, a blacklist of nested macro names and a noninvokable flag.  When an identifier token is produced as part of a macro invocation its blacklist is assigned to be that of the head token of the macro that was just invoked, and also the name of the macro that was used is added.

Once you have identified a new potential macro invocation, check to see if the heads name is contained in its own blacklist, if it is flag it as noninvokable.  If the head token is flagged as noninvokable, abort the invocation.
