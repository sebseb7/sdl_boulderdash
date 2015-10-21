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

bd.exe: main.c bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile SDL2-2.0.3
	touch SDL2-2.0.3/i686-w64-mingw32/include/SDL2/winapifamily.h
	i686-w64-mingw32-gcc -static -std=gnu99 -ISDL2-2.0.3/i686-w64-mingw32/include/SDL2 -D_GNU_SOURCE=1 -Dmain=SDL_main -LSDL2-2.0.3/i686-w64-mingw32/lib  main.c bd_lib.c bd_game.c -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -o bd.exe

bd_windows.zip: bd.exe
	zip -j bd_windows.zip bd.exe 
	

SDL2-2.0.3:
	wget https://www.libsdl.org/release/SDL2-devel-2.0.3-mingw.tar.gz
	tar -xzf SDL2-devel-2.0.3-mingw.tar.gz
	rm -f SDL2-devel-2.0.3-mingw.tar.gz


.PHONY : clean all

