CC=gcc
CFLAGS =-O3 --pipe 
#SRC = base.c cmd.c dbg.c  get.c group.c init-close.c join.c print.c read.c write.c env.c
OBJ = base.o cmd.o dbg.o  group.o init-close.o join.o print.o read.o write.o env.o

all: libcolian.a

libcolian.a: $(OBJ)
	ar cr libcolian.a $(OBJ)
	ranlib libcolian.a

clean:
	rm $(OBJ)
	rm libcolian.a