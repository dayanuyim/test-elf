CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INCS = -I.
LIBS = -L.

OBJS = utils.o elf_bin.o elfparser.o
HDRS = utils.h elf_bin.h

EXE = elfparser

.SUFFIXS:
.SUFFIXS: .cpp .o

all: ${EXE}

${EXE}: ${OBJS}
	${CC} ${LIBS} -o $@ $^ 

%.o: %.cpp ${HDRS}
	${CC} ${CFLAGS} ${INCS} -c $<

clean:
	@rm -f ${EXE} *.o
