cc = gcc
obj = flashnul.o generate.o io.o display.o actions.o common.o detect.o advanced.o 
out = flashnul.exe
CFLAGS = --pipe -O2 -march=i686 -Wall

all: $(obj) colian/libcolian.a
	gcc $(obj) $(OPT) -L colian -lcolian  -lkernel32 -o $(out)
	strip $(out)
	cp flashnul.exe ..

colian/libcolian.a: 
	make -C colian

clean: 
	make clean -C colian
	rm *.o flashnul.exe

redo: clean all