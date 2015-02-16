## CPPGM Programming Assignment 8 (nsinit)

### Overview

Write a C++ application called `nsinit` that takes as input a set of C++ Source Files, executes all phases of translation, and outputs a mock program image in the format described below.

The behaviour of `nsinit` is undefined if a translation unit does not match pa8.gram.

Note that unlike PA7, behaviour is now defined for diagnostic-required ill-formed programs that match the assignment grammar.

Also note, there is no machine code generation required for the mock program image, variables are output in their zero-initialized or constant-initialized state, and functions are output as fixed stubs.

### Prerequisites

You should complete Programming Assignment 7 before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa8.git

It contains:

- a stub implementation of `nsinit`
- a compiled reference implementation `nsinit-ref`
- a wrapper for a production C++ compiler called `nsinit-other` that serves as a secondary partial reference implementation.  (to use it you will need to modify the command)
- a test suite.
- a stdin/stdout wrapper for `nsinit-ref` called `nsinit-ref-stdin`, that passes the output program image through `xxd`.
- a stdin/stdout wrapper for `nsinit-other` called `nsinit-other-stdin`, that passed the output program image through `objdump`.
- the grammar for this assignment called `pa8.gram`
- a html grammar explorer of `pa8.gram` in the sub-directory `grammar/`

You will want to extend your PA7 solution to complete this assignment.

#### Test Format Changes

In this assignment there are test cases with multiple translation units.  To accomodate this, each test case now consists of one _or more_ input source files:

    NNN-testname.t.1
    NNN-testname.t.2
    ...
    NNN-testname.t.<n>

The output files:

    NNN-testname.ref
    NNN-testname.ref.stdout
    NNN-testname.ref.exit_status
    
remain the same, however note the outfile for PA8 is of binary format, so you will want to look at it in hex (with, for example, `xxd` or `ghex`)

#### `nsinit-other`

Additional to the normal `.ref.*` output files, there are output files:

    NNN-testname.other.stdout
    NNN-testname.other.exit_status

which are generated with `nsinit-other`.  _These are not used in automated testing of `nsinit`._  For ill-formed test cases the error message in `.other.stdout` may be superior to the reference implementations.  The `.other.exit_status` is mainly for internal use to help test the reference implementation during development against a production compiler.

Note that the output of `nsinit-other` is a real ELF program image, and so not the same as the PA8 Mock Program Image.  To save space it is deleted during generation of the `.other.*` output.

The `.ref.*` and `.other.*` can be regenerated with the `make ref-test` target.  To use it you will need to modify `nsinit-other` to wrap the production toolchain you wish to use as `other`.

### Input / Command-Line Arguments

The same as PA7 `nsdecl`.  Behaviour is undefined unless the command-line arguments match:

    $ nsinit -o <outfile> <srcfile1> <srcfile2> ... <srcfileN>

with the same relaxations as PA7

### Output Format

`nsinit` shall write a binary image to <outfile> in the PA8 Mock Program Image format described below.

### PA8 Mock Program Image

The magic first four bytes shall be the `array of 4 char` string literal "PA8".

The following entities shall be appended:

- variables of static or thread storage duration
- temporary objects bound to references
- mock function stubs
- string literals

In the following order:

_BLOCK 1_: Variables and functions shall then be appended in order of first declaration within the program, where the translation units are processed in the same order as given on the command-line.

_BLOCK 2_: Lifetime-extended temporaries bound to references shall then be appended in order of their first use within the program, with command-line translation unit order.

_BLOCK 3_: String Literals shall then be appened in order of their tokens within the program, with command-line translation unit order.

#### Variables/Functions

Each defined named variable and declared function shall be appended to the output image, regardless of whether it have static or thread local storage duration (automatic and dynamic storage duration are not available in PA8).  Recall from the ABI that fundamental types all have an alignment equal to their size.  Zero padding should be used to place the object at the next properly aligned offset within the file.

The size and alignment of the complete fundamental types are:

	signed char             1
	short int               2
	int                     4
	long int                8
	long long int           8
	unsigned char           1
	unsigned short int      2
	unsigned int            4
	unsigned long int       8
	unsigned long long int  8
	wchar_t                 4
	char                    1
	char16_t                2
	char32_t                4
	bool                    1
	float                   4
	double                  8
	long double            16
	nullptr_t               8

`void` is an incomplete type, and so does not have a size or alignment.

`nullptr_t` is an unnamed fundamental type.  There is no `nullptr_t` keyword.  It is the type of the expression `nullptr`.  There is no way to declare it in PA8.  In a later assignment you will be able to access the type by using `decltype(nullptr)`, when you implement `decltype`.  When you implement your standard library you will create a type called `std::nullptr_t` as follows:

    namespace std
    {
        using nullptr_t = decltype(nullptr);
    }

This is how the `std::nullptr_t` name is created.

The types `const T`, `volatile T` and `const volatile T` all have the same size and alignment as `T`.

The type `pointer to T` has a size and alignment of 8 bytes.  Pointers shall be represented using an unsigned 64-bit little-endian value that is an offset within the image file.  That is, for the mock program image, it shall be as if the entire image file (including magic four bytes) is loaded into memory starting at memory address 0.

`std::size_t` has the same size, alignment and representation as `unsigned long int`, 8 and 8.

The type `reference to T` (both lvalue and rvalue versions) shall be implemented using a `pointer to T` representation.  The pointer shall point to the referenced entity.

An `array of unknown bound` is an incomplete type, and so does not have a size or alignment.

An `array of N T` has a size of `N*sizeof(T)`, and an alignment of `alignof(T)`.

A `function of XXX` has a mock size of 4 and a mock alignment of 4, and a mock representation same the `array of 4 char` "fun".  (Later on in the real program image functions will consist of x86-64 machine code of variable length.)

### Error Handling

If the program is ill-formed, but syntactically correct according to pa8.gram, `nsinit` shall `EXIT_FAILURE`.

If the program is not syntactically correct according to pa8.gram, behaviour is undefined.

### Standard Output / Error

Standard output and standard error are ignored for `nsinit`.  You are free to use them as you want.  It is suggested that you may output error messages to stderr, and output helpful debug information to stdout.  The reference implementation outputs some undocumented linking information that may give some helpful clues as to what it is doing internally, it is however not recommended to try and mimic it.

### Features

#### Expressions

Expressions have been extended to include non-user-defined literals (including true, false, nullptr), id-expressions, and parenthesized expressions thereof:

	expression:
		KW_TRUE
		KW_FALSE
		KW_NULLPTR
		TT_LITERAL
		OP_LPAREN expression OP_RPAREN
		id-expression

    constant-expression:
    	expression

#### Initialization

Expressions may now be used in the following contexts:

- simple declaration initializers
- static_assert declarations    	
- array bounds

The semantics of initialization are described in 8.5.  In the full grammar initialization takes place in more than just these three places.  To implement initialization you will also have to implement many of the Standard Conversions, clause 4.  This will also entail annotating value categories of expressions (3.10).

You will also need to determine during translation if an expression is a constant expression, and if it is, evaluate it.  This is described in 5.19.

#### Specifiers

The specifiers `static`, `thread_local`, `extern`, `typedef`, `constexpr`, `inline` as well as the cv-qualifiers and simple type specifiers are all in effect and should be semantically checked, analyzed and result in correct behaviour.

#### Functions

The grammar now includes a function definition that may only have an empty body.  The presence of the body distinguishes a function declaration from a function definition.  The semantics of function definitions and their relationship to ODR and linking should be applied appropriately.  This will include distinguishing two functions of the same name based on their signature as described in 3.5.

#### Declarations

As per PA7 you will continue to support namespace definitions, simple declarations, namespace alias definitions, using declarations, using directives and alias declarations.

Added in PA8 are initializers for simple declarations, function definitions with empty bodies and `static_assert` declarations.

You are also expected to handle the correct scope change for a name lookup that occurs in a declaration after a qualified declarator-id (3.4.3p3 is now required).

### Standard Revision

You will want to carefully review:

    3.2 One definition rule
    3.5 Program and linkage
    3.6.2 Initialization of non-local variables
    3.10 Lvalues and rvalues
    4.x Standard Conversions
    7.1.1 Storage Class Specifiers
    7.1.5 Constexpr Specifier
    8.5.0 Initializers
    8.5.2 Initializers / Character arrays
    8.5.3 Initializers / References

However other parts of the standard will come into play as well.  Reading these sections alone is not a substitute for completing RAA.

### Design Notes (Optional)

Start by reviewing chapter 5 and 6 of the dragon book to get some background theory.

In general when annotating a full expression you will start from the top, work your way to the leaves, and then back up to the top.  For this assignment an expression consists of only one of two possible things, literals or id-expressions - neither of which have sub-expressions - so this is relatively straightforward for now.  Start by analyzing the expression and determining its natural type and value category.  In this assignment this information can always be obtained for an expression, irrespective of its context.  In the full grammar there are a few exceptions to this which need top-down information, but not for this assignment.  Once the expressions natural type and value category have been determined, based on the context the expression is in, further implicit conversions or other transformations will take place.

The natural value category of a literal is prvalue, except for string literals which are lvalues.  The type and value of most literals can be determined from your PA2 code.  The three simple keyword literals not dealt with in PA2 are mentioned here: `true` is a 1 byte bool prvalue 0x01, `false` is a 1 byte bool prvalue 0x00, and `nullptr` is an 8 byte prvalue of a unique type (later to be named `std::nullptr_t` in your standard library) and has a value of 8 zero bytes.

The value category of an id-expression is an lvalue.  References are implicitly dereferenced so their type is the underlying referenced type, for all other variables and functions their type is the same as their declared type.

Later on there is an additional value category xvalue, which is not reachable in PA8 - however you should account for all three value categories in your system in preparation.

For this assignment the only contexts an expression can be in are initialization contexts where the expression is a full expression.  The rules for initialization are explained in 8.5, and they appeal to the standard conversions.  You can think of the major case of initialization as a function of a destination type, a source type, a source value category and sometimes some other properties of the source (eg null pointer constants, string literals).  Based on these "parameters" the initialization is either well-formed or ill-formed, and if well-formed it dictates a series of implicit conversions that need to be applied.

The other part of annotation is constant expression evaluation.  There are several ways to implement this, but you can essentially consider each expression to have a value that is a property of that expression.  In some cases that value is completely determinable during translation, for example if it is a literal.  In other cases it is unknown until runtime, so you can think of such as a value as in an "undefined" state during translation.  For other values partial information is known.  For example a pointer to a static variables value is not known until link-time, but you can track it in a position-independant manner as a symbol plus a byte offset called an addend.  The symbol is effectively a free variable that you will complete after linking.  The rules in 5.19 about constant expressions imply the information you need to track in this regard, and when you are allowed/required to calculate this value during translation.

If an expression is a constant expression under 5.19 then you will have enough information about its value to evaluate it during parsing.  This is what you must do for array bounds and static_assert, or else the program is ill-formed.

Once the translation units are annotated you will then proceed to link them.  This involves applying 3.5 to associate declarations from different translation units to the same entity.  Once you have determined the full set of linked entities you will then plan out their position in the program image in the order described in the assignment.  Once the final file offsets are known you will then go back and complete the position-independant values with absolute values.  This is known as relocation.  Depending on how you have structured the values you may need to make another pass to propogate these absolute addresses through-out the annotated initializers.

At this stage you are now ready to write the program image.  Iterate through all the entities and write the information described.  For this assignment, for functions you only need to write "fun".  For variables if you know their initial value (they are constant), write the value (constant initialization) - otherwise write zero (zero initialization).  Likewise for temporaries.

You are then ready to output the program image to a file.
