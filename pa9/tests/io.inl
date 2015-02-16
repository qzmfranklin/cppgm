#include "lib.h"

#define BUF_SIZE 0x10000000

inbuf_begin: data64 0;
inbuf_end: data64 0;
inbuf_len: data64 0;

outbuf_begin: data64 0;
outbuf_end: data64 0;
outbuf_len: data64 0;

// ===== ioerror: input/output error =====
ioerror_msg: "IO ERROR\n";
ioerror:
	isub64 x64 ioerror ioerror_msg;
	write(t64, stderr, ioerror_msg, x64);
	exit(1);

// =======================================


// ===== iobegin: setup buffers and read stdin =====
iobegin:
	// allocate inbuf and outbuf
	alloc([inbuf_begin], BUF_SIZE);
	move64 [inbuf_end] [inbuf_begin];
	alloc([outbuf_begin], BUF_SIZE);
	move64 [outbuf_end] [outbuf_begin];

        // remaining nbytes
	move64 x64 BUF_SIZE;

iobegin_read_loop:
	read(y64, stdin, [inbuf_end], x64);

	// check read error
	slt64 z8 y64 0;
	jumpif z8 ioerror;

	// check end of file
	ieq64 z8 0 y64;
	jumpif z8 iobegin_done;

	// update variables
	isub64 x64 x64 y64;
	iadd64 [inbuf_end] [inbuf_end] y64;

	// check out-of-memory
	ieq64 z8 0 x64;
	jumpif z8 ioerror;

	// continue
	jump iobegin_read_loop;
	
iobegin_done:
	isub64 [inbuf_len] [inbuf_end] [inbuf_begin];
	ret;

// ===================================================

// ===== ioend: write output buffer and exit =====
ioend:
	move64 y64 [outbuf_begin];
	move64 x64 [outbuf_len];

ioend_write_loop:
	write(t64, stdout, y64, x64);

	// check error
	slt64 z8 t64 0;
	jumpif z8 ioerror;

	// update ptr/length
	iadd64 y64 y64 t64;
	isub64 x64 x64 t64;

	// check done
	ieq64 z8 0 x64;
	jumpif z8 ioend_done;

	// continue
	jump ioend_write_loop;

ioend_done:
	exit(0);

// ===================================================

// ===== memcpy: dest, src, len =====

memcpy:
        move64 x64 0;

memcpy_loop:
        sge64 z8 x64 [bp-24];
        jumpif z8 memcpy_done;

        iadd64 y64 [bp-16] x64;
        move8 t8 [y64];

        iadd64 y64 [bp-8] x64;
        move8 [y64] t8;

        iadd64 x64 x64 1;

        jump memcpy_loop;

memcpy_done:
        ret;

// ==================================

