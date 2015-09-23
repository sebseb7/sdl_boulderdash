COMPILER = gcc
FLAGS= --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
FLAGSW= --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=

FLAGS+= `sdl-config --cflags`
FLAGSW+= `SDL-1.2.15/bin/sdl-config --cflags`
LDFLAGS+= `sdl-config --libs`
LDFLAGSW= `SDL-1.2.15/bin/sdl-config --libs`

all: bd_test win

clean:
	rm -f bd_test
	rm -f bd.exe

bd_test: main.c bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) $(FLAGS) main.c bd_lib.c bd_game.c $(LDFLAGS) -o bd_test 

win: main.c bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	i686-w64-mingw32-gcc -ISDL-1.2.15/include $(FLAGSW) main.c bd_lib.c bd_game.c -LSDL-1.2.15/lib  -lmingw32 -lSDLmain -lSDL -mwindows -o bd.exe

.PHONY : clean all win

