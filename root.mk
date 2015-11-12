#	PROJECT-WIDE COMMON COMPILING FLAGS
CC		:=gcc
CFLAGS 		:=-O3 -Wall -Drestrict=__restrict__ -DNDEBUG

CXX		:=g++
CXXFLAGS	:=-O3 -Wall -Drestrict=__restrict__ -DNDEBUG -std=c++11

#       PROJECT-WIDE DEFAULT LINKING LIBRARIES AND INCLUDE DIRECTORIES
INCS		:=-iquote $(ROOT)
LIBS		:=

#       SUPPRESS LENGTHY COMMANDLINE ECHOS
QUIET           :=@

#	INTERNAL VARIABLES
OBJ		:=# .o files
DEP		:=# .d files
ASM		:=# .s files
DEPFLAGS	:=-MMD -MP# preprocessor generates .d files
ASMFLAGS	:=-S# source code commented assembly code
.SUFFIXES:

#	INTERNAL VARIABLES
OBJ		:=# .o files
DEP		:=# .d files
ASM		:=# .s files
DEPFLAGS	:=-MMD -MP# preprocessor generates .d files
ASMFLAGS	:=-S# source code commented assembly code
.SUFFIXES:

#	COLORFUL SHELL ECHO!
NONE		:=[00m
#	Normal colors
BLACK		:=[00;30m
RED		:=[00;31m
GREEN		:=[00;32m
BROWN		:=[00;33m
BLUE		:=[00;34m
MAGENTA		:=[00;35m
CYAN		:=[00;36m
GREY		:=[00;37m
#	Bold
B_BLACK		:=[01;30m
B_RED		:=[01;31m
B_GREEN		:=[01;32m
B_BROWN		:=[01;33m
B_BLUE		:=[01;34m
B_MAGENTA	:=[01;35m
B_CYAN		:=[01;36m
B_GREY		:=[01;37m
#	Underlined
U_BLACK		:=[04;30m
U_RED		:=[04;31m
U_GREEN		:=[04;32m
U_BROWN		:=[04;33m
U_BLUE		:=[04;34m
U_MAGENTA	:=[04;35m
U_CYAN		:=[04;36m
U_GREY		:=[04;37m
#	Blinking
BLK_BLACK	:=[05;30m
BLK_RED		:=[05;31m
BLK_GREEN	:=[05;32m
BLK_BROWN	:=[05;33m
BLK_BLUE	:=[05;34m
BLK_MAGENTA	:=[05;35m
BLK_CYAN	:=[05;36m
BLK_GREY	:=[05;37m
#	Reversed foreground and background color
REV_BLACK	:=[07;30m
REV_RED		:=[07;31m
REV_GREEN	:=[07;32m
REV_BROWN	:=[07;33m
REV_BLUE	:=[07;34m
REV_MAGENTA	:=[07;35m
REV_CYAN	:=[07;36m
REV_GREY	:=[07;37m
