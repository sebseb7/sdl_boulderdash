COMPILER = gcc
FLAGS= -O3 --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes 
LDFLAGS=

FLAGS+= `sdl2-config --cflags`
LDFLAGS+= `sdl2-config --libs`

all: bd_test 

clean:
	rm -f bd_test
	rm -f bd.exe
	rm -f bd_osx.zip
	rm -f bd_win.zip
	rm -f bd.rc
	rm -f bd.ico
	rm -rf Boulderdash.app/Contents/Frameworks/SDL2.framework
	rm -f Boulderdash.app/Contents/MacOS/bd_osx
	rm -f Boulderdash.app/Contents/Resources/iconfile.icns
	rm -f bdrc.o

bd_test: bd_icon.c main.c sdl_util.c sdl_util.h bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) $(FLAGS) main.c bd_lib.c bd_game.c sdl_util.c $(LDFLAGS) -o bd_test 

Boulderdash.app/Contents/MacOS/bd_osx: bd_icon.c main.c sdl_util.c sdl_util.h bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile 
	@$(COMPILER) $(FLAGS) main.c bd_lib.c bd_game.c sdl_util.c $(LDFLAGS) -o bd_osx
	@install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 bd_osx
	@makeicns -32 bd_36x36x4.png -out Boulderdash.app/Contents/Resources/iconfile.icns 
	@mv bd_osx Boulderdash.app/Contents/MacOS
	@tar -C /Library/Frameworks -c SDL2.framework | tar -C Boulderdash.app/Contents/Frameworks -x
	@touch Boulderdash.app

bd_osx.zip: Boulderdash.app/Contents/MacOS/bd_osx
	@zip -r bd_osx.zip Boulderdash.app

bd.ico: bd_36x36x4.png Makefile
	icotool -c -o bd.ico bd_36x36x4.png

bd.exe: bd_icon.c bd_36x36x4.png bd.ico main.c sdl_util.c sdl_util.h bd_lib.c bd_game.c bd_caves.h bd_game.h Makefile SDL2-2.0.4-mingw
	echo "0 ICON bd.ico" > bd.rc
	i686-w64-mingw32-windres bd.rc bdrc.o
	i686-w64-mingw32-gcc -static -std=gnu99 -ISDL2-2.0.4-mingw/i686-w64-mingw32/include/SDL2 -D_GNU_SOURCE=1 -Dmain=SDL_main -LSDL2-2.0.4-mingw/i686-w64-mingw32/lib  main.c bd_lib.c bd_game.c sdl_util.c -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc  bdrc.o -o bd.exe

bd_win.zip: bd.exe
	zip -j bd_win.zip bd.exe 
	
SDL2-2.0.4-mingw:
	wget https://www.libsdl.org/release/SDL2-devel-2.0.4-mingw.tar.gz
	tar -C SDL2-2.0.4-mingw -xzf SDL2-devel-2.0.4-mingw.tar.gz
	rm -f SDL2-devel-2.0.4-mingw.tar.gz

SDL2-2.0.4:
	wget https://www.libsdl.org/release/SDL2-2.0.4.tar.gz
	tar -xzf SDL2-2.0.4.tar.gz
	rm -f SDL2-2.0.4.tar.gz

.PHONY : clean all

