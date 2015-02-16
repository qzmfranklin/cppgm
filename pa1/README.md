## CPPGM Programming Assignment 1 (pptoken)

### Overview

Write a C++ application called `pptoken` that accepts a _C++ Source File_ on standard input, executes phases 1, 2 and 3 of the _Phases of Translation_ (defined below), and describes the resulting sequence of `preprocessing-tokens` to standard output in the specified format.

### Definition: Phases of Translation

The overall process of taking source files and producing compiled programs is broken down into 9 phases of translation.

Translation phase 4 is called _preprocessing_.  The preprocessing phase applies preprocessing directives like `#include`, `#define`, `#ifdef`, and so on.

The _preprocessing_ phase takes as input a stream of `preprocessing-tokens`.  Phases 1 through 3 are about taking a physical source file and decomposing it into those tokens.  They are essentially the "preprocessors tokenizer".

It is your task in this assignment to implement these first three phases.  The `pptoken` application is a wrapper for them to fit the provided test suite.

Please read _Clause 2: Lexical Conventions_ (pages 16 through 32) of [the C++ standard (Nov 12 Working Draft - N3485)](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3485.pdf).

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa1.git

It contains a stub implementation of pptoken, a compiled reference implementation and a test suite.

### Input Format

The C++ Source File shall be read from standard input `std::cin` in UTF-8 format.

The _source character set_ and the _execution character set_ shall be _course defined_ (defined below) as UTF-8.  Line endings are UNIX - a single LF (0x0A).

### Definition: UTF-8

If you already know how a Unicode code point is encoded and decoded into UTF-8, UTF-16, and UTF-32, then you can skip this definition.

The Unicode character set includes almost every character from every human language.

Each character in the Unicode character set has been assigned an integer, called its _code point_.

Code points can range from a minimum of 0 to a maximum of 0x10FFFF

For example:

 - the english letter A has a code point of 0x41
 - the greek letter π has a code point of 0x3C0
 - the musical symbol 𝄞 has a code point of 0x1D11E

In order to store _one code point_ we encode it into a sequence of _one or more fixed-width code units_.

 - UTF-8 is an encoding scheme that uses 8 bit wide code units
 - UTF-16 is an encoding scheme that uses 16 bit wide code units
 - UTF-32 is an encoding scheme that uses 32 bit wide code units

The simplest way is to store each code point is in its own single code unit.  Of the three schemes only UTF-32 has a large enough code unit (32 bits) to hold any code point in one code unit.  UTF-32 effectively means storing each code point in a 32-bit integer.

The remaining two ways (UTF-8 and UTF-16) use a variable length encoding.  Each code point is transformed into one or more code units.

Two simple bit manipulation algorithms are specified for UTF-8 and UTF-16 respectively to calculate the code point from a sequence of code units (and visa-versa).

The two algorithms are described here:

1. [UTF-8: http://tools.ietf.org/html/rfc3629#page-4](http://tools.ietf.org/html/rfc3629#page-4)
2. [UTF-16: http://tools.ietf.org/html/rfc2781#section-2](http://tools.ietf.org/html/rfc2781#section-2)

### Error Reporting

If an error occurs in `pptoken` you should print a helpful error message to `std::cerr` and `main` should `return EXIT_FAILURE`.  The test harness will interpret `EXIT_FAILURE` as indicating that the C++ Source File is _ill-formed_.

Internally you may throw an exception and put a catch handler in `main` that then returns failure.  The skeleton code is already setup like this.

### Development Environment

The grading servers run on _Ubuntu 12.10 64-bit_ with the default Ubuntu server packages and only additionally the `build-essential` package installed as a bootstrap toolchain:

    $ sudo apt-get install build-essential

It is recommended that you setup your developer machine with an identical environment.

### Restrictions

You may only depend upon the C++ standard library and what is already included in this skeleton code.  No additional third party libraries/tools dependencies may used.

You may not submit intermediate computer-generated code.  (You may submit original source code for a tool that is built and generates code during the build process - although that is _not recommended_ for this assignment.)

We require that you agree to not publish your solution to this programming assignment as a condition of taking this assignment.  You are free to discuss the design in general terms, but please do not share code.

### Output Format

`pptoken` shall write to standard output the following in UTF-8 format:

For each token in the sequence, one line shall be printed consisting of 3 space-character separated elements.

The first element is the _Token Type_ (defined below).  The next is an integer N which is the byte length of the _Token Data_ (defined below).  The last is the N bytes of the Token Data.

At the end of the output `pptoken` should print `eof` on a line by itself.

There is an interface and implementation in the skeleton code you can use, called `IPPTokenStream` and `DebugPPTokenStream` respectively, that will produce the correct output format for you.

### Definition: Course Defined

In the standard there are a number of cases that are called _implementation defined_, meaning it is up to the toolchain author what to choose in that case.

In some of those cases we will define them as the same for everybody taking this course.  We will say such a case is _course defined_.

In the remaining cases, it is up to each participant to choose for themselves.

### Features

Several transformations will need to be applied in a pipeline as described in section 2.2.{1,2,3} of the standard.  The following summary is given as a rough reference:

Decode each UTF-8 code unit sequence from the input file to its unicode code point.

Convert any `universal-character-names` (escape sequences `\uXXXX` and `\UXXXXXXXX`) into their unicode code point.

The nine three-character trigraphs sequences should be replaced using their single-character equivalents:

    ??=  -->  #
    ??/  -->  \
    ??'  -->  ^
    ??(  -->  [
    ??)  -->  ]
    ??!  -->  |
    ??<  -->  {
    ??>  -->  }
    ??-  -->  ~

Line Splicing: If you encounter a backslash followed by a linefeed, ignore it.

If the file does not end in a linefeed add one.

Comments (`/* ... */` and `// ...`) are replaced by a space character (ie they are part of a non-empty `whitespace-sequence`)

Finally decompose the source file into a sequence of tokens of the following types:

    whitespace-sequence
    new-line
    header-name
    identifier
    pp-number
    character-literal
    user-defined-character-literal
    string-literal
    user-defined-string-literal
    preprocessing-op-or-punc
    non-whitespace-character

We have synthesized `whitespace-sequence` and `new-line`, they are not defined explicitly in the standard.  They will be significant in preprocessing so it is useful to deal with them as part of the token stream.

A `whitespace-sequence` is a maximal sequence of whitespace characters _not including line feeds_, (which effectively also includes comments)

A `new-line` is an LF (0x0A) character.

The remaining 9 token types correspond to the 9 bodies of the `preprocessing-token` productions.

### Definition: Token Type:

The token type is a string, one of:

    whitespace-sequence
    new-line
    header-name
    identifier
    pp-number
    character-literal
    user-defined-character-literal
    string-literal
    user-defined-string-literal
    preprocessing-op-or-punc
    non-whitespace-character

### Definition: Token Data:

The following token types have empty token data:

    whitespace-sequence
    new-line

The remaining token types show the UTF-8 encoded sequence of the content they correspond to.

### Example Output

For example the source file:

    foo 1.0e2 *=@ /* bar */ "baz"

Produces an output of:

    identifier 3 foo
    whitespace-sequence 0 
    pp-number 5 1.0e2
    whitespace-sequence 0 
    preprocessing-op-or-punc 2 *=
    non-whitespace-character 1 @
    whitespace-sequence 0
    string-literal 5 "baz"
    new-line 0 
    eof

## Testing

To execute the local test suite simply issue the command:

    $ make test

This will build `pptoken`, run each test against it, then compare the results against the reference implementation.

Each test input has a filename like `tests/NNN-foo.t`

`NNN` is a number representing the test complexity. `foo` is a descriptive name of the test.

The reference output is given in:

`tests/NNN-foo.ref`

and the reference exit status is given in:

`tests/NNN-foo.ref.exit_status`

The output of your version is given in:

`tests/NNN-foo.my`

and the exit status of your version is given in:

`tests/NNN-foo.my.exit_status`

So you can diff the files to determine why a test is failing.

When you execute the test suite it will execute them in order and stop with an error on the first failed test.

## Reference Implementation

There is a compiled reference implementation called `pptoken-ref` of `pptoken` in the starter kit than you can use to generate arbitrary reference output for comparison to your implementation.

If you find a bug in the reference implementation (an inconsistency of its output to what is described in this document or the standard) than please report it to the forum.  Make sure to include the output of `./pptoken-ref -v`.

## Submitting Your Implementation For Grading

We will provide an interface that will allow you to submit your solution for grading closer to the due date.  Instructions will be made available on the main course site.

## Preprocessing Token Grammar Summary

The grammar for `preprocessing-token` is as follows with some notes added.  It is functionally equivalent to the way it is presented in the standard, and mostly uses the same names:

    preprocessing-token:
        identifier
        pp-number
        character-literal
        user-defined-character-literal
        string-literal
        user-defined-string-literal
        preprocessing-op-or-punc
        header-name
        non-whitespace-character

    non-whitespace-character:
        any single non-whitespace code point that does
                not fit into another preprocessing token
                except `'` and `"`

This is used so that garbage will still tokenize in some cases because the section it contains may be removed later anyway during preprocessing (by exclusion from an `#if 0` or similar).  The test suite (as is the standard) is quite forgiving about whether a malformed token causes an error or gets parsed using `non-whitespace-character`.

It is _course defined_ that `'` and `"` do not match `non-whitespace-character`. This is a standard-compliant feature (see 2.5.2).

    identifier:
        identifier-nondigit
        identifier identifier-nondigit
        identifier digit

    identifier-nondigit:
        nondigit
        code points in Annex E1

An identifier may not start with code points from Annex E2.

    nondigit: one of
        `a` `b` `c` `d` `e` `f` `g` `h` `i` `j` `k` `l` `m`
        `n` `o` `p` `q` `r` `s` `t` `u` `v` `w` `x` `y` `z`
        `A` `B` `C` `D` `E` `F` `G` `H` `I` `J` `K` `L` `M`
        `N` `O` `P` `Q` `R` `S` `T` `U` `V` `W` `X` `Y` `Z` `_`

    digit: one of
        `0` `1` `2` `3` `4` `5` `6` `7` `8` `9`

    pp-number:
        digit
        `.` digit
        pp-number digit
        pp-number identifier-nondigit
        pp-number `e` sign
        pp-number `E` sign
        pp-number `.`

    sign:
        `+`
        `-`

    character-literal:
        `'` c-char-sequence `'`
        `u'` c-char-sequence `'`
        `U'` c-char-sequence `'`
        `L'` c-char-sequence `'`

    c-char-sequence:
        c-char
        c-char-sequence c-char

    c-char:
        any code point except `'`, `\`, or new-line
        escape-sequence

    escape-sequence:
        simple-escape-sequence
        octal-escape-sequence
        hexadecimal-escape-sequence

    simple-escape-sequence: one of
        `\'` `\"` `\?` `\\` `\a` `\b` `\f` `\n` `\r` `\t` `\v`

    octal-escape-sequence:
        `\` octal-digit
        `\` octal-digit octal-digit
        `\` octal-digit octal-digit octal-digit

    octal-digit: one of
        `0` `1` `2` `3` `4` `5` `6` `7`

    hexadecimal-escape-sequence:
        `\x` hexadecimal-digit
        hexadecimal-escape-sequence hexadecimal-digit

    hexadecimal-digit: one of
        `0` `1` `2` `3` `4` `5` `6` `7` `8` `9`
        `a` `b` `c` `d` `e` `f`
        `A` `B` `C` `D` `E` `F`
        
    user-defined-character-literal:
        character-literal ud-suffix

    ud-suffix:
        identifier

    string-literal:
        `""`
        `"` s-char-sequence `"`
        `R` raw-string
        encoding-prefix `""`
        encoding-prefix `"` s-char-sequence `"`
        encoding-prefix `R` raw-string

    s-char-sequence:
        s-char
        s-char-sequence s-char

    s-char:
        any code point except `"`, `\`, new-line
        escape-sequence

    encoding-prefix:
        `u8`
        `u`
        `U`
        `L`

    raw-string:
        `"` d-char-sequence `(` r-char-sequence `)` d-char-sequence `"`

Note that in a raw string most of the early translations are switched off (see 2.5.3.1 and 2.14.5 in the standard).

    d-char-sequence:
        d-char
        d-char-sequence d-char

    d-char:
        any code point except space, `(`, `)`, `\`, horizontal tab,
            vertical tab, form feed, and newline.

    r-char-sequence:
        r-char
        r-char-sequence r-char

    r-char:
        any code point, except a sequence that is
            a right parenthesis `)` followed by the initial
            d-char-sequence (which may be empty) followed
            by a double quote `"`.

    user-defined-string-literal:
        string-literal ud-suffix

    preprocessing-op-or-punc: one of
        `{` `}` `[` `]` `#` `##` `(` `)` `<:` `:>` `<%` `%>` `%:` `%:%:` `;` `:` `...`
        `new` `delete` `?` `::` `.` `.*` `+` `-` `*` `/` `%` `ˆ` `&` `|` `~` `!` `=` `<` `>`
        `+=` `-=` `*=` `/=` `%=` `ˆ=` `&=` `|=` `<<` `>>` `>>=` `<<=` `<=` `>=` `&&` `==` `!=`
        `||` `++` `--` `,` `->*` `->` `and` `and_eq` `bitand` `bitor` `compl`
        `not` `not_eq` `or` `or_eq` `xor` `xor_eq`

    header-name:
        `<` h-char-sequence `>`
        `"` q-char-sequence `"`

`header-names` are context sensetive.  They should only be tokenized after a sequence of (start of file or `new-line`) (`#` or `%:`) `include` ...disregarding `whitespace-sequences`.  Read section 16.2 in the standard to understand what is going on here.

    h-char-sequence:
        h-char
        h-char-sequence h-char

    h-char:
        any code point except new-line and `>`

    q-char-sequence:
        q-char
        q-char-sequence q-char

    q-char:
        any member of the source character set except new-line and `"`

    universal-character-name:
        `\u` hex-quad
        `\U` hex-quad hex-quad

    hex-quad:
        hexadecimal-digit hexadecimal-digit hexadecimal-digit hexadecimal-digit

### Design Notes (Optional)

There are many ways to design `pptoken` and we will describe in general terms one way to write `pptoken`.  We make no claims that this is the best way by any metric. You are free to use this design or you are free to ignore this and do it your own way.

Read chapter 3 of the dragon book if you have it.

`pptoken` can be broken into two parts.  First there is the translation tasks, then there is the tokenization task.

The translation tasks are:

- UTF8 decoding
- universal character name decoding
- trigraph decoding
- line splicing
- file terminating line-ending

Implement each of these in a pipeline.  These should result in a stream of code points you can store in `int`.  It makes it easy to add a code point `-1` to represent the end of the file.  Each part of the pipeline can be implemented as a state machine that takes one code point at a time and returns zero or more code points.

In order to support raw mode in raw string literals it is necessary to be able to turn this on and off, bypassing the pipeline when entering into tokenization of a raw string literal and restoring it on exit.

Now we describe the tokenization part.

Start by drawing a DFA (deterministic finite automaton) for each token type.  Recall that a DFA is a directed graph where the vertexes are states and the edges are code points (or sets of code points).

We will briefly mention each type:

    new-line

This is trivial.  There are two states, the start state and end state.  The edge is a line feed.

    identifier

An identifier also has two states.  From the start state to end state is an edge with all leading characters (`nondigit` and everything from Annex E1 minus E2).  Connecting the end state back to itself is another edge with all allowable identifier body characters (`nondigit` and `digit` and E1).

    pp-number

`pp-number` has a similar structure to identifier with a few more states.

    character-literal

Once again just follow the grammar.

    user-defined-character-literal

The is a character literal followed by an idenitifier, so you can just chain those two together.

    string-literal

Similar to character-literal, except for raw string.  Raw string literals require special treatment in two ways.  First they need to go into a _raw mode_ and turn off the early translations.  Secondly they terminate in an unusual way.  The standard provides us some leeway by saying than anything up to an opening `"` that could start a raw string literal (eg `R"`) will be a raw string literal.

    user-defined-string-literal

Effectively a `string-literal` and an `identifier` chained together.

    preprocessing-op-or-punc

For the identifier-like operators just ignore them for now.  You can emit them from the identifier DFA by checking them against a `set<string>`.  For example: Let `not_eq` be initially tokenized as an identifier, but before you emit it, check if they are in the set of indentifier-like operators.  If it is, emit it as a `preprocessing-op-or-punc` instead of `identifier`.

For the rest form a DFA.  It will have many states.  Maybe separate it out into different diagrams for each leading character.

For example a leading character of `<` could result in `<<`, `<%`, `<=`, `<<=`, `<:`.  If we see another `<` (so `<<` so far) we still need a third state to decide between `<<=` and `<<`.

Also note that there is a special exception involving the sequence `<::`:

> 2.5.3: ...if the next three characters are `<::` and the subsequent character is neither `:` nor `>`, the `<`
is treated as a preprocessor token by itself and not as the first character of the alternative token `<:`

One way to deal with this by adding three surrogate operators `<::`, `<::>` and `<:::` to your DFA, and when you find one of these, rather than emiting them as one token, emit the two appropriate for each case.  You may need to reread 2.5.3 a couple of times before this is clear.

    whitespace-sequence

We recommend subsuming comments into this production rather than replacing them with a single space.  The start and end states should be connected by non-new-line whitespace.  If you encounter `/` followed by `*`, enter _inline comment state_; if you encounter `/` followed by `/`, enter _single line comment state_.  Exit them as described in the standard.

    header-name

This is straightforward, just follow the grammar.  However, it should only be considered in certain contexts as described earlier.

    each non-white-space character that cannot be one of the above

This is simple.  If you cannot tokenize a sequence, just emit each code point in this category.  We will also permit (as does the standard) to simply error in many of these cases.

Now that you have all the DFAs you will combine them into a single state machine with look-ahead.  The state machine starts in the `start` state.  It will receive the stream of code points from the translation tasks.  Based on the look-ahead it will enter one of the DFAs above.  In some cases there will still be ambiguity so you will need to make a decision based on look-ahead.  For example `u8` can be an identifier, it can also be the `encoding-prefix` of a `string-literal`.  So, at the end of identifier `u8`, if the look-ahead is `"` then enter the `string-literal` DFA, otherwise emit `u8` as an `identifier` and return to the `start` state.  Similarly at the end of `character-literal`, if the look-ahead is a valid `identifier` start, than enter `user-defined-character-literal`, otherwise emit the `character-literal`.

Also, there is some cross-over between `whitespace-sequence` comment starts `//`, and `/*` and the divide and divide-assign operators `/` and `/=`.  You can deal with that by combining the DFAs.  For example you might create a state `forward-slash` that indicates the last seen code point is `/`, then based on lookahead either enter the operator DFA or the whitespace/comment DFA.

Usually you will need to match the longest token (some exceptions are comments, raw strings and the `<` `::` case).  So greedily "keep going" in the state machine until you cannot go and further, then emit the discovered token and return to the start state.

To emit a token there is an interface provided in the skeleton code called `IPPTokenStream`.  It has one function for each token type.  In most cases you will need to store and then UTF-8 encode the code point sequence corresponding to the token to pass to the data argument.  The token data is expected to have the translation tasks applied (for one example _line splicing_), but not the literal tasks (for example decoding `simple-escape-sequences` is not required at this stage) (which occur later in translation phase 7 and is not part of this assignment).

### Self-Tokenizing (Optional/Ungraded)

To get into the self-hosting spirit of things, once you have completed this assignment run your `pptoken` application using your `pptoken.cpp` source file as input:

    $ .pptoken < pptoken.cpp > pptoken.my

And compare by hand some of the `pptoken.cpp` file to the output in `pptoken.my` to check it tokenized correctly.

