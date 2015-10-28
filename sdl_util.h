#ifndef _SDL_UTIL_H
#define _SDL_UTIL_H

void sdl_init(int h, int v,const char* title, int fps);
void sdl_deinit(void);
void sdl_loop(const void* pixels);
void sdl_windowsize(int x,int y);

#endif
