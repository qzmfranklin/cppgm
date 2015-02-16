## CPPGM Programming Assignment 9 (cy86)

Write a C++ application called `cy86` that translates a set of source files in the CY86 Mock Intermediate Language (described below) into a native Linux x86-64 program.

### Prerequisites

You should complete Reading Assignment B before starting this assignment.

### Starter Kit

The starter kit can be obtained from:

    $ git clone git://git.cppgm.org/pa9.git

It contains:

- a stub implementation of `cy86`
- a compiled reference implementation `cy86-ref`
- a test suite of CY86 programs.
- the grammar for this assignment called `pa9.gram`
- a html grammar explorer of `pa9.gram` in the sub-directory `grammar/`
- a machine readable list of the CY86 operand constraints (described below) called `cy86-opcode.desc`

You will want to extend your PA5 `preproc` solution to complete this assignment.

### Input / Command-Line Arguments

The same as PA8 `nsinit`.  Behaviour is undefined unless the command-line arguments match:

    $ cy86 -o <outfile> <srcfile1> <srcfile2> ... <srcfileN>

with the same relaxations as PA8

### Output Format

`cy86` shall write a Linux x86-64 Program to `<outfile>`.  A Linux x86-64 Program is an ELF Executable file (details in Reading Assignment B).

### Testing

Testing has changed for PA9.  It is now based on the output of the generated program of your implementation.

For this assignment your solution will be tested in the following way:

For each test case `x` your implementations output will be generated as follows:

There are n input translation units `x.t.1` through `x.t.n` as per PA8.

Your compiler will be executed on them as follows:

    $ cy86 -o x.my.program x.t.*

The __numeric__ exit code (`$?`) will be stored in `x.my.impl.exit_status`

If the exit code is `0` (meaning success), the resulting program will then be executed:

    $ x.my.program > x.my.program.stdout

The numeric exit code (`$?`) will be stored in `x.my.program.exit_status`

The reference implementation output is generated in the same fashion.

If `x.ref.impl.exit_status` is not the same as `x.my.impl.exit_status` the test case fails.

Otherwise, if `x.ref.impl.exit_status` is non-zero (indicating an ill-formed program) the test case passes.

Otherwise, if `x.ref.program.stdout` is identical to `x.my.program.stdout` and `x.ref.program.exit_status` is identical to `x.my.program.exit_status` the test case passes.

Otherwise, the test case fails.

### CY86 Language Specification

#### Phases of Translation

The CY86 phases of translation are identical to the C++ phases of translation from 1 to 6 and up to and including the tokenization part of phase 7.  (That is, the C++ preprocessor and tokenizer are applied.)  It is at the parsing phase the two languages diverge.

There are no user-defined literals in CY86, it is an error if one is present.  Although the C++ keywords are reserved in CY86, they are not used.  It is therefore also an error if a C++ keyword is present.

After tokenization, the token sequence from each translation unit is concatenated in the order given on the command-line into one long token sequence.  This token sequence is then matched against the `program` grammar:

#### Grammar/Semantics

    program:
	    (statement OP_SEMICOLON)*

A program consists of a sequence of statements.  Each statement is in one of the following forms:

    statement:
	    label OP_COLON statement
	    opcode operand*
	    TT_LITERAL
	    OP_MINUS TT_LITERAL

    label:
   	    TT_IDENTIFIER
	
	opcode:
	    TT_IDENTIFIER
	
A `label` introduces a name for a 64-bit unsigned long int value that represents the virtual memory address of the labelled `statement`.  If the spelling of a label is the same as either an opcode, register or another label - the program is ill-formed.  (Note that labels can be forward referenced, so a label may be used before the label is introduced with this construct).

The two literal statement forms place the specified literal data in PA2 encoding, after appropriate zero-padding to achieve the correct alignment.  This allows static data to be introduced into the program (see also the `data` opcodes).  The `OP_MINUS` prefix may only be applied to arithmetic types, and specify the arithmetic negation of the value.

An `opcode` is an identifier.  It specifies one of the CY86 instructions (described below).  This is followed by zero or more operands:

    operand:
	    register
	    immediate
	    memory

The opcode dictates the number of operands, and for each their width and other constraints.  The operands however can be parsed context-free, and later semantic rules can be applied.  (In fact CY86, like many near-assembly-level languages, is a regular language).

Each `operand` may be either a `register`, an `immediate` or a `memory`.

A `register` is an identifier with one of the following spellings:

    sp
    bp
    x8
    x16
    x32
    x64
    y8
    y16
    y32
    y64
    z8
    z16
    z32
    z64
	t8
	t16
	t32
	t64

The CY86 machine has four general-purpose 64-bit registers `x64`, `y64`, `z64` and `t64` and, like x86-64, these registers have 32-bit, 16-bit and 8-bit aliases to the lower bits:

    x8    x16    x32    x64
    y8    y16    y32    y64
    z8    z16    z32    z64
    t8    t16    t32    t64

> NOTE: When an instruction writes to x32, it shall zero the upper 32 bits of x64.  Likewise for y32-y64, z32-z64 and t32-t64.  This is to match the behaviour of x86 registers.

As well as two 64-bit address registers:

    sp  bp

(Note these are not the same as the x86-64 16-bit registers of the same names, sp and bp.  The suggested design has the CY86 sp and bp registers backed by the x86-64 rsp and rbp registers respectively.)

Internally, it also has an unaddressable 64-bit instruction pointer `pc`.  It has no flag registers.

If a register does not match the required operand width in an instruction, the program is ill-formed.

An `immediate` operand has one of the following forms:

	immediate:
		TT_LITERAL
		label
		OP_LPAREN TT_LITERAL OP_RPAREN
		OP_LPAREN OP_MINUS TT_LITERAL OP_RPAREN
		OP_LPAREN label OP_RPAREN
		OP_LPAREN label OP_PLUS TT_LITERAL OP_RPAREN
		OP_LPAREN label OP_MINUS TT_LITERAL OP_RPAREN

The parenthesis only serve to delimit the immediate tokens and have no semantic meaning.

The `OP_MINUS` `TT_LITERAL` form carries out arithmetic negation as for literal statements described above.

The `label` `OP_PLUS/MINUS` `TT_LITERAL` form requires the literal be of integral type.  The value undergoes negation if `OP_MINUS` is present.  The value is then sign-extended to 64-bits if a signed type, or zero-extended to 64-bit if unsigned.  The value is then added to the constant value of the label to produce the immediate value.  The final type is unsigned long int.

If the width of a literal (given in PA2) does not match the operand width required by the opcode:

- If the literal is too long, it is truncated to the required size, by removing the rightmost bytes.
- If the literal is too short it shall be sign-extended if it is a signed integral type, or zero-extended otherwise.

A `memory` operand specifies a range of bytes in virtual memory:

	memory:
		OP_LSQUARE TT_LITERAL OP_RSQUARE
		OP_LSQUARE register OP_RSQUARE
		OP_LSQUARE register OP_PLUS TT_LITERAL OP_RSQUARE
		OP_LSQUARE register OP_MINUS TT_LITERAL OP_RSQUARE
		OP_LSQUARE label OP_RSQUARE
		OP_LSQUARE label OP_PLUS TT_LITERAL OP_RSQUARE
		OP_LSQUARE label OP_MINUS TT_LITERAL OP_RSQUARE

The size of the range of bytes is determined by the opcode and operand width.

The address of the first byte is given by the expression inside the square brackets.  The width of the address value is 64-bit, so only 64-bit registers may be used, and labels and `TT_LITERAL`s are interpreted as described above in the same manner as immediate operands of 64-bit width.

The _red zone_ (the 128 bytes from `[sp-128]` to `[sp-1]` inclusive ) is reserved and is undefined in CY86 programs.  (This allows your CY86 implementation to use this area for temporary storage).

#### Entry Point

If there exists a label `start`, the entry point of a CY86 program is the label `start`.

Otherwise, the entry point of a CY86 program is the first `statement` in the `program`. 

### CY86 Opcode Descriptions

In the following documentation, each opcode descriptor is followed by one or more operand descriptors.  An operand descriptor is a string that is a concatenation of one or more of the following strings:

    w  -  written to - may not be an immediate
    r  -  read from
    a  -  address
    b  -  boolean, behaviour is undefined if not equal to 0 or 1
    i  -  an integer (either signed or unsigned)
    s  -  a signed integer (hi bit is sign bit)
    u  -  an unsigned integer (no sign bit)
    f  -  a floating point number
    I  -  an immediate only
    8  -  8 bits wide
    16 -  16 bits wide
    32 -  32 bits wide
    64 -  64 bits wide
    80 -  80 bits wide

For example, in the following hypothetical definition:

    foo sr32

The opcode is `foo` and it takes one operand.  The operand (`sr32`) is a signed (`s`) 32-bit (`32`) integer (`i`) that is read from.

#### Literal Data

    data8    rI8
    data16   rI16
    data32   rI32
    data64   rI64

The data instructions are literal encodings of their immediate operands (after appropriate immediate width conversion).  They also must be aligned so are preceeded if necessary by appropriate zero-padding to align them to their size.  They are not intended for execution, and like literal statements, are a way to add static data to a program image.  For example:

    foo: data16 0x1122;
    bar: data16 0x3344;
    
The label `foo` address will be divisable by 2.  `bar` is equal to `foo+2`.  The four bytes in memory starting at address `foo` are in hex:

    22 11 44 33  

#### Data Transfer

    move8    w8   r8
    move16   w16  r16
    move32   w32  r32                 op1 = op2;
    move64   w64  r64
    move80   w80  r80

This command simply copies data from the second operand and overwrites the first.

#### Control Transfer

	jump     ar64                     pc = op1
    jumpif   br8   ar64               if (op1) pc = op1
    call ar64                         sp -= 8; *sp = pc; jump op1;
    ret                               jump *sp; sp += 8;

The `jump` command unconditionally assigns to the `pc` instruction counter.  The instruction at the given address will be the next executed.

The `jumpif` command jumps to the address in its second operand, if and only if the first operand is non-zero.

The `call` command pushes the current `pc` on the stack and then jumps to op1.

The `ret` command pops the stack into the `pc`.

#### Bitwise Logic

	not8    w8   r8
	not16   w16  r16                 op1 = ~op2
	not32   w32  r32 
	not64   w64  r64

Bitwise NOT.

	and8    w8   r8   r8
	and16   w16  r16  r16            op1 = op2 & op3
	and32   w32  r32  r32
	and64   w64  r64  r64

Bitwise AND.

	or8     w8   r8   r8
	or16    w16  r16  r16
	or32    w32  r32  r32            op1 = op2 | op3
	or64    w64  r64  r64

Bitwise OR.

	xor8    w8   r8   r8
	xor16   w16  r16  r16
	xor32   w32  r32  r32            op1 = op2 ^ op3
	xor64   w64  r64  r64

Bitwise XOR.

#### Bit Shift

	lshift8     iw8   ir8   ur8
	lshift16    iw16  ir16  ur8       op1 = op2 << op3
	lshift32    iw32  ir32  ur8        
	lshift64    iw64  ir64  ur8

Left shift op3 bits.  If op3 is greater than or equal to operand width, behaviour is undefined.  Zero bits are shifted in.

	srshift8    sw8   sr8   ur8
	srshift16   sw16  sr16  ur8
	srshift32   sw32  sr32  ur8
	srshift64   sw64  sr64  ur8       op1 = op2 >> op3
	urshift8    uw8   ur8   ur8
	urshift16   uw16  ur16  ur8
	urshift32   uw32  ur32  ur8
	urshift64   uw64  ur64  ur8

Right shift op3 bits.  If op3 is greater than or equal to operand width, behaviour is undefined.  Signed versions preserve sign (shift in copy of previous sign bit).  Unsigned versions shift in zero.

#### Floating Conversions

	s8convf80   fw80 sr8
	s16convf80  fw80 sr16
	s32convf80  fw80 sr32
	s64convf80  fw80 sr64
	u8convf80   fw80 ur8
	u16convf80  fw80 ur16        op1 = (long double) op2
	u32convf80  fw80 ur32
	u64convf80  fw80 ur64
	f32convf80  fw80 fr32
	f64convf80  fw80 fr64

Convert from integer and floating types to 80-bit float.

	f80convs8   sw8  fr80
	f80convs16  sw16 fr80
	f80convs32  sw32 fr80
	f80convs64  sw64 fr80
	f80convu8   uw8  fr80
	f80convu16  uw16  fr80              op1 = (T) op2              
	f80convu32  uw32  fr80
	f80convu64  uw64  fr80
	f80convf32  fw32 fr80
	f80convf64  fw64 fr80

Convert from 80-bit float to integer or floating type.  Behaviour is undefined if the value cannot be exactly represented.

#### Arithmatic

    iadd8    iw8   ir8   ir8
    iadd16   iw16  ir16  ir16
    iadd32   iw32  ir32  ir32
    iadd64   iw64  ir64  ir64         op1 = op2 + op3
    fadd32   fw32  fr32  fr32
    fadd64   fw64  fr64  fr64
    fadd80   fw80  fr80  fr80

Arithmetic addition.  Integer and floating versions. (Note that signed and unsigned integer addition are the same operation).

    isub8   iw8   ir8   ir8
    isub16  iw16  ir16  ir16
    isub32  iw32  ir32  ir32
    isub64  iw64  ir64  ir64          op1 = op2 - op3
    fsub32  fw32  fr32  fr32
    fsub64  fw64  fr64  fr64
    fsub80  fw80  fr80  fr80

Arithmetic subtraction.  Integer and floating versions.  (Note that signed and unsigned integer subtraction are the same operation).

    smul8   sw8   sr8   sr8
    smul16  sw16  sr16  sr16
    smul32  sw32  sr32  sr32
    smul64  sw64  sr64  sr64
    umul8   uw8   ur8   ur8
    umul16  uw16  ur16  ur16          op1 = op2 * op3
    umul32  uw32  ur32  ur32
    umul64  uw64  ur64  ur64
    fmul32  fw32  fr32  fr32
    fmul64  fw64  fr64  fr64
    fmul80  fw80  fr80  fr80

Arithmetic multiplication.  Signed integer, unsigned integer and floating-point versions.

    sdiv8   sw8   sr8   sr8
    sdiv16  sw16  sr16  sr16
    sdiv32  sw32  sr32  sr32
    sdiv64  sw64  sr64  sr64
    udiv8   uw8   ur8   ur8
    udiv16  uw16  ur16  ur16          op1 = op2 / op3
    udiv32  uw32  ur32  ur32
    udiv64  uw64  ur64  ur64
    fdiv32  fw32  fr32  fr32
    fdiv64  fw64  fr64  fr64
    fdiv80  fw80  fr80  fr80

Arithmetic division.  Signed integer, unsigned integer and floating-point versions.

    smod8    sw8   sr8   sr8
    smod16   sw16  sr16  sr16
    smod32   sw32  sr32  sr32
    smod64   sw64  sr64  sr64         op1 = op2 % op3
    umod8    uw8   ur8   ur8
    umod16   uw16  ur16  ur16
    umod32   uw32  ur32  ur32
    umod64   uw64  ur64  ur64

Modulus operation.  Signed integer and unsigned integer versions.

#### Comparisons

In all comparison operations the first 8-bit operand is set to 0x01 to indicate true and 0x00 to indicate false.

	ieq8    wb8  ir8  ir8
	ieq16   wb8  ir16 ir16
	ieq32   wb8  ir32 ir32
	ieq64   wb8  ir64 ir64           op1 = op2 == op3
    feq32   wb8  fr32 fr32
    feq64   wb8  fr64 fr64
    feq80   wb8  fr80 fr80

Equals operation.  Integer (bitwise) and floating versions.

	ine8    wb8  ir8  ir8
	ine16   wb8  ir16 ir16
	ine32   wb8  ir32 ir32 
	ine64   wb8  ir64 ir64           op1 = op2 != op3
	fne32   wb8  fr32 fr32
	fne64   wb8  fr64 fr64
	fne80   wb8  fr80 fr80

Not Equals operation.  Integer (bitwise) and floating versions.

	slt8    wb8  sr8  sr8
	slt16   wb8  sr16 sr16
	slt32   wb8  sr32 sr32
	slt64   wb8  sr64 sr64
	ult8    wb8  ur8  ur8
	ult16   wb8  ur16 ur16            op1 = op2 < op3
	ult32   wb8  ur32 ur32
	ult64   wb8  ur64 ur64
    flt32   wb8  fr32 fr32
    flt64   wb8  fr64 fr64
    flt80   wb8  fr80 fr80

Less Than operation.  Signed integer, unsigned integer and floating versions.

	sgt8    wb8  sr8  sr8
	sgt16   wb8  sr16 sr16
	sgt32   wb8  sr32 sr32
	sgt64   wb8  sr64 sr64
	ugt8    wb8  ur8  ur8
    ugt16   wb8  ur16 ur16            op1 = op2 > op3
    ugt32   wb8  ur32 ur32
    ugt64   wb8  ur64 ur64
    fgt32   wb8  fr32 fr32
    fgt64   wb8  fr64 fr64
    fgt80   wb8  fr80 fr80

Greater Than operation.  Signed integer, unsigned integer and floating versions.
                     
	sle8    wb8  sr8  sr8
	sle16   wb8  sr16 sr16
	sle32   wb8  sr32 sr32
	sle64   wb8  sr64 sr64
	ule8    wb8  ur8  ur8
	ule16   wb8  ur16 ur16            op1 = op2 <= op3
	ule32   wb8  ur32 ur32
	ule64   wb8  ur64 ur64
	fle32   wb8  fr32 fr32
	fle64   wb8  fr64 fr64
	fle80   wb8  fr80 fr80

Less than or Equal operation.  Signed integer, unsigned integer and floating versions.

	sge8    wb8  sr8  sr8
	sge16   wb8  sr16 sr16
	sge32   wb8  sr32 sr32
	sge64   wb8  sr64 sr64
	uge8    wb8  ur8  ur8
	uge16   wb8  ur16 ur16            op1 = op2 >= op3
	uge32   wb8  ur32 ur32
    uge64   wb8  ur64 ur64
    fge32   wb8  fr32 fr32
    fge64   wb8  fr64 fr64
    fge80   wb8  fr80 fr80

Greater than or Equal operation.  Signed integer, unsigned integer and floating versions.

#### System Calls

	syscall0  w64 r64
	syscall1  w64 r64 r64
	syscall2  w64 r64 r64 r64
	syscall3  w64 r64 r64 r64 r64                op1 = syscall(op2, ..., opn)
	syscall4  w64 r64 r64 r64 r64 r64
	syscall5  w64 r64 r64 r64 r64 r64 r64
	syscall6  w64 r64 r64 r64 r64 r64 r64 r64

The number after the opcode is the number of parameters to the system call.  op1 stores the return value.  op2 is the system call number.  op3 through to op3+n are the parameters to the system call.

### Design Notes (optional)

The primary purpose of this assignment and the CY86 language is to provide a simple wrapper for you to write and test your x86-64 Assembler component.

The recommended PA9 solution design is as follows:

    PA5Preproc
       |
       | Tokens            (eg TT_IDENTIFIER)
       v
    CY86Parser
       |
       | CY86Instructions  (eg move64 x64 y64)
       v
    CY86ToX86Translator
       |
       | X86Instructions   (eg mov r12 r13)
       v
    X86Assembler
       |
       | X86MachineCode    (eg 4D 89 EC)
       v
    X86Program

For later assignments you will then reuse and extend your X86Instruction and X86Assembler components in the final stages of your real backend.

As you will discover the CY86Parser and CY86ToX86Translator components are small and just for testing.  In particular, the CY86 language is not intended to be an intermediate form used in PA10 and future assignments.  You may decide later to extend the CY86 language to serve as an intermediate form in your future designs, but this is not required or recommended.

The CY86Parser is a very simplified version of a parser like the one you wrote for PA6.  The CY86 language has only eight non-terminals and is a regular language so this will be trivial.  As output from your CY86Parser you will want to have a sequence of CY86Instructions that form an object model of the various CY86 opcodes and operands.  You can use the usual object-oriented techniques to model the different kinds of operands and their association with the instruction that contains them.

You will also want to perform a short semantic analysis phase to tie up labels and to check operand type and transform operand widths and so on.

The next step is to plan out which x86 instructions you will want to use to translate CY86 into, and how you will use registers/memory.  Each CY86 instruction can be translated into a short fixed finite sequence of x86 instructions.  There are multiple ways to do this, but as a recommended simple design we suggest the following:

You can directly back the CY86 registers with x86 registers as follows:

    cy86    x86
    ----    ---
    sp      rsp
    bp      rbp
    x64     r12
    y64     r13
    z64     r14
    t64     r15

The stack register and base register are obvious.  The reason r12 to r15 are recommended for the other registers is that they are not used for anything else in other x86 instructions or in system calls, so this is conveniant as they are never clobbered.

Then to translate a CY86 instructions you can use the other x86 registers freely.  Generally speaking the way we recommend to translate a non-floating-point CY86 instruction is as follows:

1. load the input operands into some fixed x86 registers for the instruction (like rax, rbx, etc)
2. execute x86 instructions on those fixed x86 registers to a fixed x86 output register
3. store the write operand from the set x86 output register to the cy86 output operand

For x87 floating point operations this is similiar but you can use the x87 register stack:

1. push the cy86 input operands onto the x87 floating stack
2. execute the x87 floating point instruction
3. pop the x87 floating stack into the cy86 output operand

If you need a temporary memory operand (as you may for some floating point instructions) you can use the _red zone_, this is the area just logically above / numerically below the stack pointer.  For example the eight bytes at `[rsp-8]` are in the red zone.  Be careful with instructions that modify the cy86 `sp` register and the use of the red zone.

As you will have learned in Reading Assignment B, x86 instructions are classified first by a mneumonic (each of which has its own page in the Intel manual), and for every mneumonic there are several forms.  You will only need to use a subset of the mneumonics and for each only a subset of their forms.

As a hint, here are most of the x86 instructions that `cy86-ref` uses:

    MOV AND OR XOR ADD SUB CMP
    FADDP FSUBP FMULP FDIVP FCOMIP
    SYSCALL SETcc NOT SHL SHR SAR
    MUL IMUL DIV IDIV JMP Jcc
    FILD FLD FISTP FSTP

And for each mneumonic only a subset of the forms are used.  You may want to use these or additional/different ones.

Once you have determined which mneumonics and forms of those mneumonics you will need you should then work through the pages for them in the Intel manual.  Figure out their semantics and then transcribe the forms into C++ data structures.  You will quickly notice that many instructions use similar patterns, and you can create an abstract base for those instructions that share properties.

You should keep in mind that you may want to extend your x86 assembler later with more instructions, so you should structure it in such a way as to make it easy to extend.

Like for CY86 operands, you will want to develop an object model for x86 operands.  They have a similar structure (immediates, registers, memory addresses), but x86 has more addressing forms (like scale-index-base `[rax+rcx*8+42]`).  You don't need to support them all internally, but can if you wish.

Once you have your x86 instructions and x86 operands in an object model you will then need to write your x86 machine code generator.  In general an x86 instruction can have prefixes, like operand size prefixes, a REX byte with REX.W, REX.R, REX.X and REX.B bits, some number of opcode bytes, a modr/m byte with reg, mod and rm fields, and a sib byte with scale, index, base fields, and displacement and immediate byte sequences.  All of those parts apart from the opcode byte may be (and usually are) absent from an instruction.  You will want to create a data structure to represent an x86 machine code instruction in full generality, and then write a function that converts from your X86Instruction object model to that data structure.  From the data structure you will then write a function that creates an array of bytes that is the final machine code.

Because final label values will depend on the byte size of instructions, and in general this is not known until the machine code is generated, you will need to incorporate a way to relocate the values as per your PA8 linker.  One way to do this is to assemble some marker value into the immediate, storing its location separately, and then making another pass of the machine code to patch them.  There is some discussion about this and related techniques around section 6.7 of the dragon book.

It is helpful in selecting instructions, and also to check your assembler, to compare it against a production toolchain.  The best way to see what, for example, gcc uses to implement an instruction is to create a test translation unit and then decompile.

For example, let's find out what instructions gcc uses to add two long doubles:

    void f(long double* res, long double* op1, long double* op2)
    {
	    *res = *op1 + *op2;
    }

Compile this with:

    $ gcc -c -O3 test.c

And then use:

    $ objdump -d -M intel test.o

The `-M intel` switch will give you intel syntax, which is much clearer for human-reading and will match up better with what you read in the Intel manual.

The objdump gives us:

    0:	db 2e                	fld    TBYTE PTR [rsi]
    2:	db 2a                	fld    TBYTE PTR [rdx]
    4:	de c1                	faddp  st(1),st
    6:	db 3f                	fstp   TBYTE PTR [rdi]
    8:	c3                   	ret  

So we can see the pointers are passed in `rdi`, `rsi` and `rdx`.  The first two FLD instructions load from the memory addresses `[rsi]` and `[rdi]` onto the x87 FPU register stack.  The FADDP instructions adds the two top values.  The FSTP then stores the result to the memory address.  The type TBYTE is for 10 bytes or 80 bits.  The prefix `TBYTE PTR` explicitly states the operand size.  If the assembler can determine this from the context this prefix can be omitted.

To compare your x86 instruction to machine code translator to a production assembler you can use a gas input file like this:

    .intel_syntax noprefix
    .global _start
    _start:
        XXX
        
Where XXX is the instruction in Intel syntax.  For example:

    .intel_syntax noprefix
    .global _start
    _start:
        mov eax, [rbx]

Save this to `test.s` and then execute:

    $ gcc -nostdlib test.s
    $ objdump -d -M intel a.out

You will see the machine code for your instruction:

    4000d4:	8b 03                	mov    eax,DWORD PTR [rbx]

The machine code is on the left, hex `8B 03`.

Notice that the assembler has automatically added the unnecessary `DWORD PTR` prefix to the memory operand. `DWORD` in Intel syntax is 32-bits (1 byte = BYTE, 2 bytes = WORD, 4 bytes = DWORD (doubleword), 8 bytes = QWORD (quadword), 10 bytes = TBYTE).   The operand size was infered from the use of the `eax` register which is 32-bits.

Here are some useful GDB commands to work with raw programs at the bare metal machine-code level:

    $ readelf -a myprogram
    Entry Point Address: 0x400123
    $ gdb ./myprogram
    gdb> break *0x400123                   <--- set breakpoint at address 0x400123
    gdb> run < test.stdin
    breakpoint hit
    gdb> set disassembly-flavor intel      <--- intel syntax
    gdb> x/20i $pc                         <--- disassemble current and following 20 instructions
    gdb> stepi                             <--- step one instruction
    gdb> info registers                    <--- the x86 general purpose register values
    gdb> info all-registers                <--- all registers, including x87 registers
    gdb> print $rax                        <--- print value of rax register
 
The Examining Memory page of the GDB manual is very useful: [https://sourceware.org/gdb/onlinedocs/gdb/Memory.html](https://sourceware.org/gdb/onlinedocs/gdb/Memory.html), as is the rest of the manual.
