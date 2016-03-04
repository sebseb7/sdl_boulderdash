
#include "SDL.h"
#include "sdl_util.h"


static unsigned int* pixelbuffer;
static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;
static int initialized = 0;
static int fpsMill;
static int row;

static void SetSDLIcon(SDL_Window* window)
{
	// this will "paste" the struct my_icon into this function
#include "bd_icon.c"

	// these masks are needed to tell SDL_CreateRGBSurface(From)
	// to assume the data it gets is byte-wise RGB(A) data
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (bd_icon.bytes_per_pixel == 3) ? 8 : 0;
	rmask = 0xff000000 >> shift;
	gmask = 0x00ff0000 >> shift;
	bmask = 0x0000ff00 >> shift;
	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = (bd_icon.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

	SDL_Surface* icon = SDL_CreateRGBSurfaceFrom((void*)bd_icon.pixel_data, bd_icon.width,
			bd_icon.height, bd_icon.bytes_per_pixel*8, bd_icon.bytes_per_pixel*bd_icon.width,
			rmask, gmask, bmask, amask);
	SDL_SetWindowIcon(window, icon);

	SDL_FreeSurface(icon);
}

unsigned int* sdl_init(int h, int v,const char* title, int fps)
{
	pixelbuffer = malloc(h*v*sizeof(uint32_t));

	SDL_EnableScreenSaver();
	window = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, h,v, SDL_WINDOW_SHOWN );
	SetSDLIcon(window);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,h,v);
	fpsMill = 1000/fps;
	initialized=1;
	row=h;

	return pixelbuffer;
};

void sdl_windowsize(int x,int y)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_SetWindowSize(window,x,y);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,x,y);
	row=x;
}

void sdl_deinit(void)
{
	initialized=0;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	free(pixelbuffer);
}


static int keymap;
static int releasemap;
static int ackmap;

int getkey(int key)
{
	if((keymap >> key) & 1)
	{
		ackmap |= (1 << key);
		return 1;
	}
	return 0;
}

void release_upped_keys(void)
{
	for(int i = 0; i < 8;i++)
	{
		if(releasemap & (1<<i))
		{
			keymap &= ~(1 << i);
		}
	}
	releasemap = 0;
}


static void keyup(int key)
{
	if(ackmap & (1<<key))
	{
		keymap &= ~(1 << key);
	}
	else
	{
		releasemap |= (1 << key);
	}
}

static void keydown(int key)
{
	keymap = 1 << key;
	ackmap &= ~(1 << key);
}

static unsigned int touch_avail = 0;
static unsigned int motion_start = 0;
static int motion_x = 0;
static int motion_y = 0;
static unsigned int touch_x = 0;
static unsigned int touch_y = 0;

static void touch_event(unsigned int x,unsigned int y)
{
	touch_x = x;
	touch_y = y;
	touch_avail=1;
}

static void touch_end_move(void)
{
	motion_start=0;
	keyup(0);
	keyup(1);
	keyup(2);
	keyup(3);
}

unsigned int get_touch(unsigned int *x, unsigned int *y)
{
	if(touch_avail == 1)
	{
		*x = touch_x;
		*y = touch_y;
		touch_avail = 0;
		return 1;
	}
	return 0;
}


static int init_tick;

int sdl_handle_events(const void* pixels)
{

	SDL_UpdateTexture(texture, NULL, pixels, row * sizeof(Uint32));//update only the updated rects
	//	SDL_RenderClear(renderer);//neccessary?
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	// limit to fps
	int current_tick = SDL_GetTicks();
	if( (current_tick - init_tick) < fpsMill)
	{
		SDL_Delay(fpsMill - (current_tick - init_tick));
	}
	init_tick = SDL_GetTicks();

	SDL_Event ev;
	while(SDL_PollEvent(&ev)) 
	{
		switch(ev.type) {
			case SDL_QUIT:
				return 0;
				break;
			case SDL_MOUSEMOTION:
				if(motion_start==1)
				{
					motion_x+=ev.motion.xrel;
					motion_y+=ev.motion.yrel;
					if(motion_y > 40)
					{
						keyup(0);
						keyup(1);
						keyup(2);
						keyup(3);
						keydown(2);
						motion_x=0;
						motion_y=0;
					}
					if(motion_y < -40)
					{
						keyup(0);
						keyup(1);
						keyup(2);
						keyup(3);
						keydown(0);
						motion_x=0;
						motion_y=0;
					}
					if(motion_x > 40)
					{
						keyup(0);
						keyup(1);
						keyup(2);
						keyup(3);
						keydown(1);
						motion_x=0;
						motion_y=0;
					}
					if(motion_x < -40)
					{
						keyup(0);
						keyup(1);
						keyup(2);
						keyup(3);
						keydown(3);
						motion_x=0;
						motion_y=0;
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(ev.button.button == SDL_BUTTON_LEFT)
				{
					motion_start=1;
					motion_x=0;
					motion_y=0;
					touch_event(ev.button.x,ev.button.y);
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(ev.button.button == SDL_BUTTON_LEFT)
				{
					touch_end_move();
				}
				break;
			case SDL_KEYUP:
				switch(ev.key.keysym.sym) 
				{
					case SDLK_UP:
						keyup(0);
						break;
					case SDLK_RIGHT:
						keyup(1);
						break;
					case SDLK_DOWN:
						keyup(2);
						break;
					case SDLK_LEFT:
						keyup(3);
						break;
					case SDLK_F1:
						keyup(4);
						break;
					case SDLK_F2:
						keyup(5);
						break;
					case SDLK_F3:
						keyup(6);
						break;
					case SDLK_F4:
						keyup(7);
						break;
					default: break;
				}
				break;
			case SDL_KEYDOWN:
				switch(ev.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						return 0;
						break;
					case SDLK_UP:
						keydown(0);
						break;
					case SDLK_RIGHT:
						keydown(1);
						break;
					case SDLK_DOWN:
						keydown(2);
						break;
					case SDLK_LEFT:
						keydown(3);
						break;
					case SDLK_F1:
						keydown(4);
						break;
					case SDLK_F2:
						keydown(5);
						break;
					case SDLK_F3:
						keydown(6);
						break;
					case SDLK_F4:
						keydown(7);
						/*
						   zoom=20;
						   sdl_windowsize(CAVE_WIDTH*zoom, CAVE_HEIGHT*zoom);


						   free(pixelarray);
						   pixelarray = malloc (CAVE_HEIGHT *zoom * CAVE_WIDTH * zoom * sizeof(uint32_t));
						   memset(pixelarray,0,CAVE_HEIGHT *zoom * CAVE_WIDTH * zoom * sizeof(uint32_t));*/
						break;
					default: break;
				}
			default: break;
		}
	}
	return 1;

}
int sdl_limit_fps(int* limiter,int fps)
{
	if( *limiter == 0)
	{
		*limiter = SDL_GetTicks();
	}

	int current_tick = SDL_GetTicks();

	if( (current_tick - *limiter) > 125)
	{
		current_tick = SDL_GetTicks();
		*limiter+=125;
		return 1;	
	}
	return 0;
};

