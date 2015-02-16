## CPPGM Programming Assignment 7 (nsdecl)

### Overview

Write a C++ application called `nsdecl` that takes as input a set of C++ Source Files, executes translation phases 1 through 7, and describes the semantically analyzed translation units in the specified format.

The behaviour of `nsdecl` is undefined if ANY of the following are true:

- A translation unit does not match pa7.gram

- The program is ill-formed, whether the standard requires a diagnostic or not.

- If a function has more than one entry in its overload set.

- If a name lookup occurs in a declaration after a qualified declarator-id (3.4.3p3 not required).

### Prerequisites

You should complete Reading Assignment A before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa7.git

It contains:

- a stub implementation of `nsdecl`
- a compiled reference implementation `nsdecl-ref`
- a test suite.
- a stdin/stdout wrapper for `nsdecl-ref` called `nsdecl-ref-stdin`.
- the grammar for this assignment called `pa7.gram`
- a html grammar explorer of `pa7.gram` in the sub-directory `grammar/`

You will also want to reuse some of your PA6 solution.

### Input / Command-Line Arguments

The same as PA6 `recog`.  Behaviour is undefined unless the command-line arguments match:

    $ nsdecl -o <outfile> <srcfile1> <srcfile2> ... <srcfileN>

with the same relaxations as PA6

### Output Format

`nsdecl` shall write the following to `<outfile>`:

The first line shall be:

    <n> translation units
    
where `<n>` is the number of translation units.

Following that, each translation unit shall be described in the order specified on the command-line.

#### Translation Unit Description

Each translation unit shall start with:

    start translation unit <srcfile>
    
Where `<srcfile>` is the same as the command-line argument.

The global namespace shall then be described.

Each translation unit shall end with:

    end translation unit
    
#### Namespace Description

Each named namespace description shall start with:

    start namespace <name>

Each unnamed namespace, and the global namespace, shall start with:

    start unnamed namespace

If a namespace is inline the following line shall be:

    inline namespace
    
Following shall be descriptions of member variables, functions and namespaces - in that order.

Each of the three lists of entities shall be ordered in order of first declaration within the translation unit.

Finally, each namespace description shall be terminated with:

    end namespace

#### Variable Description

Each declared variable (whether or not defined) shall be described on one line as follows:

    variable <name> <type>

Where `<name>` is the unqualified variable name, and `<type>` is the description of the variables type.

#### Function Description

Each declared function (whether or not defined) shall be described on one line as follows:

    function <name> <type>
    
Where `<name>` is the unqualified function name, and `<type>` is the description of the variables type.

#### Type Description

Fundamental Types shall be described in the canonical form given in PA2.  Specifically one of:

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
	float
	double
	long double
	void
	nullptr_t

Compound types shall be described recursively as follows:

    const T
    volatile T
    const volatile T
    pointer to T
    lvalue-reference to T
    rvalue-reference to T
    array of unknown bound of T
    array of N T
    function of (P1, P2, P3) returning R
	function of (P1, P2, P3, ...) returning R

Where T, P1, P2, ..., Pn and R are types and N is a positive integer

### Error Handling

Behaviour of `nsdecl` is undefined if an error occurs.

### Standard Output / Error

Standard output and standard error are ignored for `nsdecl`.

### Features

You will need to implement the following features:

- typedef, variable and function declarations

- namespaces, both named and unnamed, and both noninline and inline.

- namespace aliases (`namespace foo = bar`), using declarations (`using a::b::c`), using directives (`using namespace foo`) and alias declarations (`using foo = bar`).

- unqualified and qualified name lookup in order to support typedef use, and qualified declaration matching.

In this assignment `constant-expression` is used by array bound, but a pa7.gram `constant-expression` can only be a `TT_LITERAL` (a non-user-defined literal).  Which literals are semantically well-formed are described in 8.3.4:

> [the literal bound shall be a] converted constant expression of type std::size_t and its value shall be greater than zero.

As per the PA7 requirements only literals that satisfy this condition have defined behaviour for PA7.

### Standard Revision

You will want to carefully review:

- _3.4.1 Unqualified Name Lookup_
- _3.4.3 Qualified Name Lookup_
- _7.1.6.1 The Cv-Qualfiers_
- _7.1.6.2 Simple Type Specifiers_
- _7.3 Namespaces_
- _8.3 Meaning Of Declarators_

However other parts of the standard will come into play as well.  Reading these sections alone is not a substitute for completing RAA.

### Design Notes (Optional)

There are several building blocks you will need to implement.

Firstly you will need to model and pass around type information within the compiler.  One way is to create a base class `Type` to represent the different possible types, then derive each type category from that.

Thereafter you may want to similarly model entities (objects, references, functions, namespaces, etc), with a base class `Entity`.

For this assignment there is only one type of scope, that being namespace scope.  Recall that the global namespace is itself a namespace.  Other types of scopes (including function prototype scope), can be disregarded for PA7.

A scope contains a map from names to entities.  For this assignment you can just store this map in your Namespace object.

Thereafter you will want to go through PA7.gram and either using your PA6 parser as a prototype or an abstract base, create semantic actions in the parser that will construct and modify appropriate model objects.  For example, if you encounter a simple declaration of a new variable, you may construct a Variable object and add it to the current Namespace object.

When you encounter a name in a declaration you will need to implement name lookup.  The Namespaces that need to be searched for a name, and in what order, are described in 3.4.  Inline namespaces and using directives have different rules depending on whether you are doing an unqualified lookup (a name `A`, or the `B` in `B::C::D`), or doing qualified lookup (the `C` in `B::C::D`).  Also different name lookup contexts may only find certain types of names (for example only namespaces, or only types), so you may want to parameterize your name lookup code with a filter parameter.

Linking is not required for this assignment.  Each translation unit can be semantically analyzed separately with no interaction between them.  In a later assignment you will need to analyze linkage and determine when two names from two different translation units should name the same entity.

The recommended overall structure of your `nsdecl` application is to parse each translation unit into an object model, and then walk the object model in the given order to create the output format.
