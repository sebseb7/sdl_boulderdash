COMPILER = gcc
FLAGS= --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=


all: bd_test

clean:
	rm -f bd_test


bd_test: main.c bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) $(FLAGS) $(LDFLAGS) main.c bd_lib.c bd_game.c -o bd_test 

.PHONY : clean all 

