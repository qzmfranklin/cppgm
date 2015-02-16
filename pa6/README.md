## CPPGM Programming Assignment 6 (recog)

### Overview

Write a C++ application called `recog` that takes as input a set of C++ Source Files, preprocesses, tokenizes and then determines if each token sequence is syntactically valid against the `translation-unit` grammar given in `pa6.gram`, with the additional requirements given below.

### Prerequisites

You should complete Programming Assignment 5 before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa6.git

It contains:

- a stub implementation of `recog` with some starter code
- a compiled reference implementation `recog-ref`
- a test suite.
- a stdin/stdout wrapper for `recog-ref` called `recog-ref-stdin` that also has the `--trace` switch enabled
- the grammar for this assignment called `pa6.gram`
- a html grammar explorer of `pa6.gram` in the sub-directory `grammar/`

You will also want to reuse your Preprocessor/Lexer from PA1-PA5.

### Input / Command-Line Arguments

The same as PA5 `preproc`.  Behaviour is undefined unless the command-line arguments match:

    $ recog -o <outfile> <srcfile1> <srcfile2> ... <srcfileN>

with the same relaxations as PA5

### Output Format

`recog` shall write the following to `<outfile>`:

The first line is:

    recog <N>
    
where `<N>` is the number of srcfiles given on the command line

The following N lines shall be:

    <srcfile> <status>
    
Where:

- `<srcfile>` is the filename given on the command line.
- `<status>` is a string, either `OK` or `BAD`

### Error Handling

If an error occurs at any stage from opening the file, during translation, or the token sequence does not match `translation-unit`, than output `BAD` for that file.  (Do not `EXIT_FAILURE`)

If no errors occur and the token sequence matches `translation-unit`, output `OK` for that file.

### Standard Output / Error

Standard output and standard error are ignored for `recog`, however it is suggested that you should output a parse tree (in a text format of your choosing) to standard output on success.

You _should not_ try to mimick the parse tree output format of the reference implementation.  It is presented only as a rough outline of its internal parse tree for debugging purposes, and not designed to be diff-equivilant to your implementation.

### Special Tokens

We have introduced several special tokens in the grammar.  They are:

	TT_IDENTIFIER
	TT_LITERAL
	ST_EMPTYSTR	
	ST_EOF
	ST_FINAL
	ST_NONPAREN
	ST_OVERRIDE
	ST_RSHIFT_1
	ST_RSHIFT_2
	ST_ZERO

#### `TT_IDENTIFIER`

These are the normal `identifier` tokens from previous assignments

#### `TT_LITERAL`

These include all literal token types from previous assignments, _including user-defined literals_

#### `ST_EMPTYSTR`

This represents a literal token with a spelling of `""`, ie an empty ordinary string literal.

Note that a `ST_EMPTYSTR` token is also a `TT_LITERAL` token.

#### `ST_ZERO`

This represents a literal token with a spelling of `0`, ie the zero octal literal.

Note that a `ST_ZERO` token is also a `TT_LITERAL` token.

#### `ST_OVERRIDE` and `ST_FINAL`

These tokens are both identifiers with the spelling `override` and `final` respectively.  Note that they are not keywords and so will match `identifier` in other contexts.  They are _context-sensetive keywords_.  That is they are only considered keywords when used in the correct context.  See 2.11.2 for clarification.

Note that `ST_OVERRIDE` and `ST_FINAL` are both also `TT_IDENTIFIER` tokens.

#### `ST_NONPAREN`

This is shorthand for any token NOT including:

    OP_LPAREN
    OP_RPAREN
    OP_LSQUARE
    OP_RSQUARE
    OP_LBRACE
    OP_RBRACE
    ST_EOF

#### `ST_EOF`

This represents the end of the translation unit (`emit_eof()`)

#### `ST_RSHIFT_1` and `ST_RSHIFT_2`

The PA5 token `OP_RSHIFT` shall be logically replaced with the two consecutive tokens `ST_RSHIFT_1` and `ST_RSHIFT_2` before parsing starts.

We then define a new grammar `close-angle-bracket`:

	close-angle-bracket:
		OP_GT
		ST_RSHIFT_1
		ST_RSHIFT_2

and replace occurences of `OP_GT` that have a "close an angle bracket pair" semantic use with it.  This allows for constructs like `T<U<3>>` to parse correctly.  See 14.2.3.

To support this we redefine `shift-operator` (and other uses of `OP_RSHIFT`) to:

	shift-operator:
		OP_LSHIFT
		ST_RSHIFT_1 ST_RSHIFT_2

So `OP_RSHIFT` no longer occurs in the grammar.

### Mock Name Lookup

We will explain briefly what "real name lookup" is, and then explain the "mock name lookup" you will use instead in PA6.

In order to parse C++ successfully and efficiently you must categorize identifiers, such as `foo`, and `simple-template-ids` such as `foo<bar>`, as to what kind of thing they identify.  These are called names.  The process of identifying name categories is called _name lookup_, and discussed in Clause 3.
 
As per 3.0.7:

> Some names denote types or templates. In general, whenever a name is encountered it is necessary to
determine whether that name denotes one of these entities before continuing to parse the program that
contains it.

So for example if you encounter the following in block scope:

    x * y;

If `x` is a type, than this declares a `pointer to x` with the name `y`:

    typedef int x;
    x * y;

If `x` is a variable, than `x * y` is an `expression-statement`, and so it is a function call to the binary `operator*` with the two operands `x` and `y`:

    struct X {...};
    X x, y;
    x * y;

Later in the course (not for PA6), in your final compiler, as you are parsing, you will lookup each name in the symbol table to determine what category of name it is.  This may involve instantiating templates.  Instantiating templates may further involve evaluating constant expressions to determine the correct specialization to use.  Evaluating constant expressions may involve calling `constexpr` functions.  This is all densely interconnected with semantic analysis.

For PA6 instead of that, you will use a mock implementation of name lookup that does not require a symbol table.

Based on the lexical form of an identifier you can determine its name category.

Specifically:

- If an identifier contains the letter `C`, it is a `class-name`
- If an identifier contains the letter `T`, it is a `template-name`
- If an identifier contains the letter `Y`, it is a `typedef-name`
- If an identifier contains the letter `E`, it is an `enum-name`
- If an identifier contains the letter `N`, it is a `namespace-name`

Note that an identifier can belong to two or more categories.

For example `TC1` is both a `template-name` and a `class-name`, so therefore `TC1<123>` is a `simple-template-id`, and that `simple-template-id` will also match as a whole `class-name`.

Functions to determine this mock name lookup are included in the starter code.

The situations where an identifier is required to match one of these names, is specified implictly in the grammar by the use of the `*-name` grammars.

For example suppose we have a nonterminal `foo` defined as follows:

    foo:
        class-name enum-name

    class-name:
        TT_IDENTIFIER
        simple-template-id

    enum-name:
        TT_IDENTIFIER

    simple-template-id:
        template-name OP_LT template-argument-list? close-angle-bracket

Here, to match the nonterminal `foo`, a `class-name` is required, followed by an `enum-name`.

- So first we check if the next token is an identifier.
- Then we check if it contains the letter `C`.
- We check if it is also `template-name` (contains `T`), and only if it does, we reduce a `simple-template-id`.
- We have now succesfully parsed a `class-name` (for example `C1` or `TC2<bar>`), and move on to the `enum-name`.
- We check that it is an identifier and that it contains a letter `E`, for example `Ebaz`.
- If it is we return the `enum-name`, and have successfully parsed `foo`.

### PA6 Grammar

The PA6 grammar is located in the file `pa6.gram` in the starter kit.

It uses an extended BNF format with the following operators:

    foo?       means 0 or 1 foo in sequence
    foo*       means 0 or more foo
    foo+       means 1 or more foo
    (foo)      parenthesis group symbols together for
    				application of the above operators

Line splices are ignored.  Alternative bodies are given indented, one per logical line.

For each non-terminal `foo`, an analysis of the non-terminal `foo` is given in `grammar/foo.html` in the starter kit.

Each analysis includes:

    USED: The non-terminals that refer to `foo` (back references)
    FIRST: The set of tokens that can possibly start `foo`
    FOLLOW: The set of tokens that can possibly occur after `foo`
    
Following these items is the grammar for `foo`, and then a breadth first list of the grammars it refers to.  That is, first the grammars that `foo` uses are listed (first degree), followed by the grammars that are used by those grammars (second degree), and so on until all nested referenced grammars are shown.

Each non-terminal is html-linked to its own analysis page.

Finally there is a `grammar/terminals.html` page that shows the `USED` field of each terminal (token) type.

### Special non-terminals:

#### `decl-specifier-seq`

In the PA6 grammar when `decl-specifier-seq` is used, it must always be present.  This precludes matching constructors, destructors and conversion operators:

    class C
    {
        C();               // wont match PA6
        ~C();              // wont match PA6
        operator bool();   // wont match PA6
    }

You will implement parsing constructor, destructor and conversion operators in a later assignment, when more of your symbol table machinery is implemented.

Further there is an important semantic rule regarding the form of `decl-specifier-seq`, it is:

> If a `type-name` is encountered while parsing a `decl-specifier-seq`, it is interpreted as part of the `decl-specifier-seq` if and only if there is no previous `type-specifier` other than a `cv-qualifier` in the `decl-specifier-seq`.

You will need to implement this rule while parsing a `decl-specifier-seq`.

#### `closing-angle-bracket`

When angle brackets are used to delimit a sequence of tokens, for example `static_cast<C1>(bar)`, `Tfoo<1,2,3>`, or `T1<T2<C>>`, there are problems caused by the fact that `>` and `>>` can also be used as operators.

How to deal with this is described in 14.2.3.

The first non-nested `closing-angle-bracket` encountered is taken to close the bracket pair.

C++ tokens nest correctly in terms of the four kinds of brackets `()`, `[]`, `{}` and `<>`.  For the three non-angle brackets types (`()`, `[]`, `{}`) this is true without any caveats, as those tokens are only ever used as brackets.  Closing angle brackets however look exactley the same as the operators `>` and `>>`, and so these tokens are not used in a balanced fashion.

What the rule in 14.2.3 is saying is that when you are inside an angle bracket pair `<>`, the next `closing-angle-bracket` parsed _at the same nesting level as the opening angle bracket_ closes it, and is never parsed as part of an `>` operator or `>>` operator.

So as an example:

	TC1< 1>2 > x1;       // syntax error
	TC1<(1>2)> x2;       // OK - not at same nested level
	TC1<TC2<1>> x3;      // OK
	TC1<TC2<6>>1>> x4;   // syntax error
    TC1<TC2<(6>>1)>> x5; // OK

We will suggest an implementation in the design notes.

### Ambiguities

You should pay attention to the two special types of ambiguities mentioned in 6.8 and 8.2, these are present in the test suite.  You should check that your parser not only recoginizes these cases but produces the correct parse tree for them.

### Ill-formed but Syntactically Valid

In some cases there are constructs that could be reduced to the PA6 grammar, but could never be part of a valid C++ program.  In some of those cases the reference implementation will reject them, in others it will accept them and leave them to fail later in semantic analysis.  In general it is a grey area and a design decision.  We expect that your implementation matches the reference implementation at least in its result for the cases in the provided test suite.  If there is a case that you think falls into this category, and are not sure about, please bring it up on forum.cppgm.org for discussion.  In general the best policy is to accept these constructs, and defer as much logic as possible until semantic analysis, when a disambiguated parse tree is available.

### Design Notes (Optional)

As usual these design notes are optional, there is more than one way to do it, and we suggest one possible implementation.

To start with you should review the grammar in the standard Annex A.3 through A.13, side-by-side with the `pa6.gram` file.  There is an __Index of grammar productions__ at page 1273 of N3485.  If there is a nonterminal name that you don't recognize or understand, use that Index to look up where that grammar is discussed in the main text.  You can also use the grammar explorer in the `grammar` directory of the starter kit to get more information about a nonterminal.

Once you are comfortable with the grammar, you will need to prepare your parsing infrastructure.  You should prepare a `Token` class if you have not already done so, and place the tokens produced from your PA5 code into it as a `vector<Token>`, replacing occurences of `OP_RSHIFT` with the two tokens `ST_RSHIFT_1` and `ST_RSHIFT_2`, and terminating the sequence with a `ST_EOF` token.

To write the parser, we recommend that for each nonterminal `foo` you should write a function `parse_foo`.  These functions can be member functions of your C++ parser class.

Each parse function in the reference implementation returns an instance of a dynamic type called `AST`.  We will describe the properties of this type here:

An `AST` is one of the following:

- an Error AST
- an Empty AST (distinct from Error AST)
- a Token AST
- a Compound AST

An Error AST represents a failed parse.

As Empty AST represents a valid but empty parse tree.

A Token AST wraps a single `Token` from the token stream.

A Compound AST contains a `map<string, AST>`, where the value field is a non-Error AST.  Many times the key names used in this map in the reference implementation will correspond to a non-terminal name, but sometimes we use shorthand context-sensetive names.

So this AST type forms a kind of "associative tree" data structure.  Additionally each AST type has a `std::set` of string "properties" it can be marked with and queried for.

Within the parser class there is a lookahead position, initially `0`, pointing at the first token.

The interface discipline of a parse function is as follows:

If the parse function succeeds it shall return a non-Error AST representing the parse sub-tree it has found, and shall leave the lookahead one past the last token used.

If the parse function fails it shall return an Error AST, and shall reset the lookahead position to where it was on entry.

It is easy to see from here how to get from this architecture to a recursive descent parser.  Furthermore you can use `FIRST` and `FOLLOW` (when they are small) to make predictive decisions to cut down parse time.

You can reread the first half of chapter 4 of the dragon book for a refresher on top down parsing.

In cases where you need to parse a sequence of the same non-terminal (`foo*` or `foo+`), in almost all cases the C++ grammar will allow you to do this greedily.  That is you can match as many as you can, and you will usually not need to backtrack, you can usually fail the parent parse function without trying shorter sequences.

Keep in mind that the AST type described is just a placeholder type that will be replaced with a set of static types later on for the final compiler.  If you use the described architecture than do not spend too much time worrying about exactley what key names to use and how to structure the tree.  It only needs to provide you enough information to verify that the tokens were recognized correctly.

With regard to implementing the `close-angle-bracket` problem, we recommend keeping track of your current nesting level and bracket type.

That means on entry to `parse_translation_unit` you are at nesting level 0 and not inside any bracket type.  If you parse an opening bracket, increase your nesting level and push the current bracket type onto a stack.  When you parse a closing bracket, reduce the nesting level and pop the bracket type.

When you are parsing an expression involving the non-bracket form of `OP_GT` or `ST_RSHIFT_1/ST_RSHIFT_2` (ie right shift and greater than) and the current bracket type is angle brackets, refuse to match those operators, because they are reserved for use by `close-angle-bracket`

Note that it is possible to parse in a much more efficient way (with much better prediction) than the way that `recog-ref` does.  This can be achieved by combining certain similar production prefixes together, effectively doing some localized bottom up parsing.  Better performance can also be realized through earlier detection using the lookahead, combining expression parsing with a precedence table as mentioned in PA3, and many other things.  However for this assignment we are mainly concerned with recognizing the correct unambiguous parse of the translation unit.  Once correct, and regression testing is in place, you can always add these optimizations incrementally later.

### `--trace` flag (optional)

The reference implementation has an extra command-line switch `--trace` that traces the progress of the recursive descent by logging:

- when a parse function is entered
- when a parse function exits
- whether the parse function succeeded or failed
- what the current lookahead is

It is enabled by default in the `recog-ref-stdin` wrapper.

You may wish to implement a similar switch as an aide to debugging your parser.
