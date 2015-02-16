## CPPGM Programming Assignment 5 (preproc)

### Overview

Write a C++ application called `preproc` that takes as input a set of C++ Source Files, executes translation phases 1 through 6 and the tokenization part of phase 7, and describes the resulting sequence of tokens to an output file.  (Notice that this means `preproc` will be a complete standard-compliant C++ Preprocessor and Lexer.)

### Command-Line Arguments

`preproc` will be invoked as follows:

    $ preproc -o <outfile> <srcfile1> <srcfile2> ... <srcfileN>

where `N >= 1`

`preproc` shall read each `<srcfilei>` file, execute phases 1 through 6 and the tokenization part of phase 7, then describe the resulting sequences of fully preprocessed and lexically analyzed tokens to `<outfile>` in the specified format.

- If the first argument of `preproc` is not `-o`, behaviour is undefined.
- If the `<outfile>` argument or any `<srcfilei>` argument starts with `-`, behaviour is undefined

These two relaxations give you the freedom to conditionally-implement implementation-defined non-default behaviour with command-line switches such as `-a`, `--foo`, and so on.

### Prerequisites

You should complete Programming Assignment 3 and 4 before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa5.git

It contains a stub implementation of `preproc` with some starter code, a compiled reference implementation and a test suite.

You will also want to reuse most of your code from PA1-PA4.

### Input Format

Each `<srcfilei>` should be opened and read relative to the current working directory of `preproc` using `std::ifstream`. Each input file will be in UTF-8 format.

### Output Format

`preproc` shall create (or overwrite if it exists) a file with the name `<outfile>`.  You can use `std::ofstream` to do so.

The first line of `<outfile>` shall be:

    preproc <N>

where `<N>` is the number of `<srcfiles>`.

Following that, for each `<srcfilei>`, a per-srcfile section is output, in the same order as given on the command-line.

The first line of the per-srcfile section shall be:

    sof <srcfilei>

where `<srcfilei>` is the name of the infile given on the command-line.

The remainder of each per-srcfile output is the same as PA2 `posttoken`.  This implies each per-srcfile section is terminated with an `eof`, so there will be N `eofs` in total.  (`sof` and `eof` lines are not generated for `#include` files)

### Example

For example for a call of

    $ preproc -o foo bar baz

Where `bar` is a file that contains `1 + 2` and `baz` is a file that contains `qux`

A file called `foo` should be created with the following contents:

    preproc 2
    sof bar
    literal 1 int 01000000
    simple + OP_PLUS
    literal 2 int 02000000
    eof
    sof baz
    identifier qux
    eof

### Error Reporting

For PA5 it is considered an error if any token stream at phase 7 contains a PA2 `invalid` token.

It is also an error if the `#error` directive is encountered in a non-excluded section.

It is also an error if an `#if` controlling expression contains a PA3 `error` result.

If an error occurs, `preproc` should `return EXIT_FAILURE` as per the behaviour of `pptoken` in PA1.

The state of the `outfile` in case of `EXIT_FAILURE` is undefined.

`preproc` standard output and standard error are undefined and ignored, whether or not an error occurs.

### Restrictions

As per PA1

### Testing

As usual to run the tests against the reference implementation use:

    $ make test

Note that because the filesystem and current working directory are significant for PA5, `preproc` must be run from the root of the PA5 starter kit (one directory up from the `tests` directory).  `make test` takes care of this for you.

Also note that stdout and stderr are combined into a `.stdout` file for PA5.  This output is not significant.  The`.ref` and `.my` files without a further suffix are the actual `outfiles`.

For adhoc testing two wrapper scripts are included called `preproc-stdin` and `preproc-ref-stdin`.  They create temporary files to hold the `srcfile` and `outfile` and then pass stdin to the temporary file, execute the `preproc`, and write the outfile to stdout.  `preproc-stdin` uses your `preproc` implementation.  `preproc-ref-stdin` uses the reference implementation `preproc-ref`.

### Features

Logically each source file is processed in turn with no shared state between them.

The following preprocessing directives must be implemented:

    - conditional inclusion (`#if`...`#endif`)  (using PA3)
    - source file inclusion (`#include`)
    - macro replacement (using PA4)
    - line control (`#line`)
    - a course-defined list of pre-defined macros
    - a course-defined list of pragmas (`#pragma`)
    - error directive (`#error`)
    - null directive and non-directive

Also you must implement the `_Pragma()` operator

#### Preprocessing Directive List

    #if
    #ifdef
    #ifndef
    #elif
    #else
    #endif
    #include
    #define
    #undef
    #line
    #error
    #pragma

If no tokens follow `#` on a logical line, it is a null directive and is always ignored.

If the first token after `#` is not an identifier, or not one of the identifiers in the above list, the logical line is a `non-directive`. It is an error if in an active `#if` section, or ignored if it is in an inactive one.

#### Pre-defined Macros

The following pre-defined macros shall be implemented:

    #define __CPPGM__ 201303L               // CPPGM course run version
    #define __cplusplus 201103L             // C++ version
    #define __STDC_HOSTED__ 1              // hosted implementation

The above are fixed values.

    #define __CPPGM_AUTHOR__ "John Smith"  // Your full real name as enrolled in the course

Replace "John Smith" with a string literal that is your full real name.

    #define __FILE__ "foo"                 // current presumed source file name
    #define __LINE__ 123                   // current presumed source line number

Behaviour specified below.

    #define __DATE__ "Mmm dd yyyy"         // build date from asctime
    #define __TIME__ "hh:mm:ss"            // build time from asctime

Use the `std::asctime` function to implement `__DATE__` and `__TIME__`.  Call it once at the entry of main, and use the same build date and time for all srcfiles.

You may conditionally-implement any other pre-defined macros provided they start with an underscore and then a capital (`_Foo`) or another underscore (`__foo`).

#### Pragmas

The following pragmas shall be supported:

    #pragma once

The following pragma shall NOT be supported (and so should be ignored):

    #pragma cppgm_mock_unknown pptokens

You may conditionally-implement any additional pragmas.

The course-defined treatment of the _Pragma operator is as follows:

> A `_Pragma(string-literal)` shall be recognized only in a PA4 `text-sequence`, and only _after_ all macro replacement.  Any occurence of the identifier `_Pragma` must be proceeded by `( string-literal )` or it is an error.  The pragma operator invocation tokens shall be removed from the `text-sequence` after it is executed.

#### Source File Inclusion and `__FILE__`

The course-defined handling of source file inclusion in the default case shall be as follows:

The current file shall be tracked by a string variable `__FILE__`.  Initially `__FILE__` shall be the same as the command-line argument.

When a `#include` directive is encountered it will match this form:

    #include pptokens

where `pptokens` is any sequences of `preprocessing-tokens`.

`pptokens` shall be macro replaced as per PA4.  If the resulting sequence of tokens is not a `header-name` or an ordinary `string-literal`, behaviour is undefined.  (Notice that a `header-name` can only be the result if it was already there, before macro replacement, as per PA1)

Both `header-name` types (`<foo>` and `"foo"`) are treated the same.  The delimiters are stripped and the resulting code points are converted into a UTF-8 string, we shall call `nextf`.

In the case of an ordinary `string-literal` it shall be post-tokenized into a UTF-8 string, and likewise we shall call the string `nextf`.

If `__FILE__` contains a `/` character, a new string `pathrel` is formed by concatenating (A) the sub-string of `__FILE__` up to and including the last `/`; and (B) the string `nextf`

In pseudo-code:

    __FILE__ = "foo/bar/baz"

    #include "qux"

    nextf = "qux"

    pathrel = "foo/bar/" + "qux" = "foo/bar/qux"

Once the two strings `nextf` and `pathrel` are identified they are searched as follows:

1. If `pathrel` is defined and a file exists of that path relative to the current working directory (or absolute if it starts with `/`), it shall be the include file.
2. Otherwise, if a file exists of the path `nextf` relative to the current working directory (or absolute if it starts with a `/`) shall be the include file.
3. Otherwise, it is an error and `EXIT_FAILURE` should be returned.

The new value of `__FILE__` is whichever one of 1 or 2 succeeded.

Recall that you can optionally implement command-line switches which alter or extend this behaviour.  You may wish to implement a `-I <path>` switch to add additional paths, and/or a `--stdinc` switch which also searches `/usr/include`, and so on.  However, exactley the two search paths specified (`__FILE__` relative, and current working directory relative) must be the `preproc` default behaviour.

#### Line Control and `__LINE__`

    #line pptokens

where `pptokens` is any sequences of `preprocessing-tokens`.  `preprocessing-tokens` are macro-replaced.

After macro replacement `#line` will match one of the following two forms:

    #line ppnumber
    #line ppnumber string-literal

The `ppnumber` should post-tokenize to a positive integer.  The `string-literal` if present shall be an ordinary `string-literal`.  If this is not the case behaviour is undefined.

The integer shall set the current value of `__LINE__`, the string shall set the current value of `__FILE__` (notice that this will impact future `#include` behaviour).

#### Pragma Once

The course-defined `#pragma once` handling is specified as follows:

There is a function in the starter code called `PA5GetFileId`:

    bool PA5GetFileId(const string& path, PA5FileId& out_fileid)

It takes a file `path` as input and an out parameter of type `PA5FileId`.  It returns `true` on success (or `false` on failure, such as because the file does not exist).

So it should be used as follows:

    string filepath = "foo/bar/baz";
    PA5FileId fileid;
    bool ok = PA5GetFileId(filepath, fileid);
    if (ok)
        // use fileid
    else
        // file not found or unaccessible

For each srcfile maintain an (initially empty) set of fileids of headers that have been pragma onced (eg with a `std::set<PA5FileId>`)

When you process a `#pragma once`, add the file id of `__FILE__` to that set.

Each time you encounter an `#include` directive, lookup the file id and check if it is in that set.  If it is ignore the `#include` directive.

> Side Note: PA5GetFileId is implemented by using the `stat(2)` system call.  It looks up the device id and ino number of the file, and uses those two numbers to differentiate files.  We will implement system calls in a later assignment, so you don't need to understand this now.

### Reading

You should read the parts of clause _16 Preprocessing directives_ that you have not already read.

### Design Notes (Optional)

You will need to start with PA1 `pptoken` code as usual, however you will need to add source file name and physical line tracking to support `__FILE__` and `__LINE__`.

One way to do this is before the entry to `PPTokenizer` count physical new line characters _before_ they enter the tokenizer, and then as `preprocessing-tokens` are emitted, store the current source file name and line in each token.  So you are not storing a string in each token (although they are usually copy-on-write anyway), you may want to store filenames in a table and use an `int` index.  After macro invocation assign the source file and line of each produced token to be that of the head.  When you invoke a `__FILE__` or `__LINE__` predefined macro, simply replace it with its own file or line.

Now we have a stream of `pptokens` with file and line numbers marked.

The next step is to accumulate preprocessing directives and text sequences.  You will need to do this in a stream, as some preprocessing directives will change the system state that will effect how later directives are handled.  You can delimit preprocessing tokens as previously.  A `new-line #` signifies the start of a preprocessing directive and a `new-line` ends one.  This can be done with a little DFA or state machine.

Each time a preprocessing directive is encountered you will need to take a certain action.

Conditional inclusion is a bit more complicated.  A `#if`, `#ifdef` or `#ifndef` will start an if group, a matching `#endif` will close it.  In between there can be one of more `#elifs` and an optional `#else`.  Depending on the corresponding truth values of the controlling expressions, each section is either active or inactive.

Whether in an inactive or active group, a nested `#if` group must be ordered correctly (with respect to `#elifs`, `#else` and `#endif`), however inside an inactive group all preprocessing directives (aside from there name) are ignored.  For example, the following is valid:

    #if 0
    #if foo bar baz
    #elif foo bar baz ... @
    #else @@@@@
    #endif @@@@@
    #else
    ok
    #endif

This is because the nested `#if` group is in an inactive section.  Any sequence of tokens can come after the directive names in this case.  However the following is invalid:

    #if 0
    #if foo bar baz
    #else @@@@@
    #elif foo bar baz ... @
    #endif @@@@@
    #else
    ok
    #endif

The `#else` and `#elif` directives are in the wrong order.  This order must be correct even in an inactive section.

This can be implemented with a stack of states.  When you enter an `#if` group or `#include`, push a new state onto the stack.  As you proceed through an `#if` group update the state.  When you exit an `#if` group or `#include` pop the state off the stack.  The state can keep track of whether or not you are in an `#if` group, where in the `#if` group you are, and whether or not you are currently active.  As the different directives are encountered they alter the state on the top of the stack or push or pop states from it.

### Standard Platform Includes (Optional/Ungraded)

This is an optional additional feature.  You are not required to implement it, and it will not be graded.

Implement a `--stdinc` command-line switch that adds the following directories to your include file search path:

    /usr/include/c++/4.7
    /usr/include/c++/4.7/x86_64-linux-gnu
    /usr/include/c++/4.7/backward
    /usr/lib/gcc/x86_64-linux-gnu/4.7/include
    /usr/local/include
    /usr/lib/gcc/x86_64-linux-gnu/4.7/include-fixed
    /usr/include/x86_64-linux-gnu
    /usr/include

These are the stanard system includes from the bootstrap enviornment.

You can extract this list from the bootstrap gcc compiler with the following command:

    $ echo | g++ -E -v -x c++ -

Also of interest, to extract the list of predefined macros from gcc you can use this command:

    $ gcc -dM -E - <<<''

If you have time feel free to experiment with preprocessing the headers in `/usr/include`, but note that many headers use non-standard features that will not be compatible with your toolchain.  This is expected, and not something to worry about.  Much later in the course you will be making a self-hosting build of your compiler, and discard the bootstrap environment.  The produced self-hosted compiler will compile programs against _your_ standard library headers, and not the standard library headers from the bootstrap environment.  The bootstrap standard library is only used for building your "stage 1" compiler, which is dynamically linked against the bootstrap standard library (`libstdc++`).  Your stage 1 compiler will statically link compiled programs against your standard library.  Your stage 1 compiler will then be used to compile your stage 2 compiler, by compiling your compiler sources.  This stage 2 compiler will be statically linked against your standard library (as well as compiling programs against your standard library like the stage 1 compiler).  Notice that this stage 2 compiler will have no dependencies on the bootstrap environment.

The important thing to understand for this assignment is that nowhere in this whole bootstrapping process does your `preproc` code need to preprocess the headers in `/usr/include`.

