CC = gcc
C_FLAGS = -g
LNK_FLAGS =
INCLUDE = -Iinclude
LIB = 

C_FILES = $(wildcard src/*.c)
O_FILES = $(patsubst src/%.c, build/%.o, ${C_FILES})
TARGET = play

all: ${TARGET}

build:
	@mkdir -p build

build/%.o: src/%.c | build
	${CC} ${C_FLAGS} -c -o $@ $^ ${INCLUDE}


${TARGET}: ${O_FILES}
	${CC} ${LNK_FLAGS} -o $@ $^ ${LIB}

dot: ast.dot
	./${TARGET}
	dot -Tpng ast.dot > ast.png

clean: ${O_FILES}
	rm $^ ${TARGET}

rm_cores:
	rm vgcore.*
