COMPILER = gcc
FLAGS= -O3 --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=

FLAGS+= `sdl2-config --cflags`
LDFLAGS+= `sdl2-config --libs`

all: bd_test 

clean:
	rm -f bd_test
	rm -f bd.exe
	rm -f bd_windows.zip

bd_test: bd_icon.c main.c sdl_util.c sdl_util.h bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) $(FLAGS) main.c bd_lib.c bd_game.c sdl_util.c $(LDFLAGS) -o bd_test 

bd_osx: bd_icon.c main.c sdl_util.c sdl_util.h bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) -Wl,-rpath @executable_path/../Frameworks -framework SDL2 -ISDL2.framework/Headers --std=gnu99 main.c bd_lib.c bd_game.c sdl_util.c -o bd_test 

bd.exe: bd_icon.c bd.ico bd.rc main.c sdl_util.c sdl_util.h bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile SDL2-2.0.4
	i686-w64-mingw32-windres bd.rc bdrc.o
	i686-w64-mingw32-gcc -static -std=gnu99 -ISDL2-2.0.4/i686-w64-mingw32/include/SDL2 -D_GNU_SOURCE=1 -Dmain=SDL_main -LSDL2-2.0.4/i686-w64-mingw32/lib  main.c bd_lib.c bd_game.c sdl_util.c -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc  bdrc.o -o bd.exe

bd_windows.zip: bd.exe
	zip -j bd_windows.zip bd.exe 
	

SDL2-2.0.4:
	wget https://www.libsdl.org/tmp/release/SDL2-devel-2.0.4-mingw.tar.gz
	tar -xzf SDL2-devel-2.0.4-mingw.tar.gz
	rm -f SDL2-devel-2.0.4-mingw.tar.gz


.PHONY : clean all

