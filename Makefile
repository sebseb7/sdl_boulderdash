COMPILER = gcc
FLAGS= --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=

FLAGS+= `sdl-config --cflags`
LDFLAGS+= `sdl-config --libs`

all: bd_test 

clean:
	rm -f bd_test
	rm -f bd.exe
	rm -f bd_windows.zip

bd_test: main.c bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) $(FLAGS) main.c bd_lib.c bd_game.c $(LDFLAGS) -o bd_test 

bd.exe: main.c bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile SDL-1.2.15
	i686-w64-mingw32-gcc -std=gnu99 -ISDL-1.2.15/include -D_GNU_SOURCE=1 -Dmain=SDL_main main.c bd_lib.c bd_game.c -LSDL-1.2.15/lib  -lmingw32 -lSDLmain -lSDL -mwindows -o bd.exe

bd_windows.zip: bd.exe
	zip -j bd_windows.zip bd.exe SDL-1.2.15/bin/SDL.dll 
	

SDL-1.2.15:
	wget https://www.libsdl.org/release/SDL-devel-1.2.15-mingw32.tar.gz
	tar -xzf SDL-devel-1.2.15-mingw32.tar.gz
	rm -f SDL-devel-1.2.15-mingw32.tar.gz


.PHONY : clean all

