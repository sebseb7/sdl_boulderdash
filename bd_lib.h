#ifndef _BD_LIB_H
#define _BD_LIB_H


enum {
	BD_WALL=1,
	BD_DIRT,
	BD_STEELWALL,
	BD_MAGICWALL,
	BD_FIREFLYr,
	BD_FIREFLYl,
	BD_FIREFLYt,
	BD_FIREFLYd,
	BD_BOULDER,
	BD_DIAMOND,
	BD_SPACE,
	BD_INBOX,
	BD_OUTBOX,
	BD_AMOEBA,
	BD_Rockford,
	BD_DRAW_LINE,
	BD_DRAW_POINT,
	BD_DRAW_FILLRECT,
	BD_DRAW_FILLRECT2,
	BD_DRAW_RASTER,
	BD_DRAW_RECTANGLE,
	BD_COLOR_ORANGE,
	BD_COLOR_GRAY1,
	BD_COLOR_WHITE,
	BD_COLOR_PURPLE,
	BD_COLOR_GREEN
};


struct cave_struct_t 
{
	int DiamonValue;
	int DiamonValueBonus;
	int DiamondsRequired[5];
	int CaveTime[5];
	int Colors[3];
	int RandSeed[5];
	int RandomFill[8];
	int MagicWallTime;
	int AmoebaTime;
	int DrawItems;
} __attribute__ ((packed));


#define CAVE_WIDTH 40
#define CAVE_HEIGHT 22

int render_field(int cave,int level,int field[CAVE_WIDTH][CAVE_HEIGHT]);

void NextRandom(int *RandSeed1, int *RandSeed2);
void draw_fillrect(int x1,int y1,int x2, int y2,int item,int field[CAVE_WIDTH][CAVE_HEIGHT]);
void draw_fillrect2(int x1,int y1,int x2, int y2,int item,int item2,int field[CAVE_WIDTH][CAVE_HEIGHT]);
void draw_point(int x,int y,int item,int field[CAVE_WIDTH][CAVE_HEIGHT]);
void draw_raster(int x1,int y1,int count_x, int count_y,int offset_x,int offset_y,int item,int field[CAVE_WIDTH][CAVE_HEIGHT]);
void draw_rect(int x1,int y1,int x2, int y2,int item,int field[CAVE_WIDTH][CAVE_HEIGHT]);


int firefly_left(int firefly);
int firefly_right(int firefly);
int firefly_x(int firefly);
int firefly_y(int firefly);

#endif
