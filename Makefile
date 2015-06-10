COMPILER = gcc
FLAGS= --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=


all: bd_test

clean:
	rm -f bd_test


bd_test: main.c bd_caves.h Makefile 
	@$(COMPILER) $(FLAGS) $(LDFLAGS) main.c bd_lib.c -o bd_test 

.PHONY : clean all 

