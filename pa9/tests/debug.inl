#include "io.inl"

#define DUMP64_CHAR(n) \
	move64 t64 [bp-8]; \
	urshift64 t64 t64 n; \
	and64 t64 t64 0xF; \
	iadd64 t64 t64 hex_digits; \
	write(t64, stderr, t64, 1)

hex_digits: '0'; '1'; '2'; '3'; '4'; '5'; '6'; '7'; '8'; '9'; 'A'; 'B'; 'C'; 'D'; 'E'; 'F'; '\n';
dump64:
	DUMP64_CHAR(60);
	DUMP64_CHAR(56);
	DUMP64_CHAR(52);
	DUMP64_CHAR(48);
	DUMP64_CHAR(44);
	DUMP64_CHAR(40);
	DUMP64_CHAR(36);
	DUMP64_CHAR(32);
	DUMP64_CHAR(28);
	DUMP64_CHAR(24);
	DUMP64_CHAR(20);
	DUMP64_CHAR(16);
	DUMP64_CHAR(12);
	DUMP64_CHAR(8);
	DUMP64_CHAR(4);
	DUMP64_CHAR(0);

	iadd64 t64 16 hex_digits;
	write(t64, stderr, t64, 1);

	ret;

#define DUMP64(x) \
	move64 bp sp; \
	isub64 sp sp 8; \
	move64 [bp-8] x; \
	call dump64; \
	iadd64 sp sp 8 

