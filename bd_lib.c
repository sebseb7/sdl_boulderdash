#include "bd_lib.h"

extern int bd_cave_data[];
extern int bd_cave_start_idx[];

static void NextRandom(int *RandSeed1, int *RandSeed2)
{
	short TempRand1;
	short TempRand2;
	short carry;
	short result;


	TempRand1 = (*RandSeed1 & 0x0001) * 0x0080;
	TempRand2 = (*RandSeed2 >> 1) & 0x007F;
	result = (*RandSeed2) + (*RandSeed2 & 0x0001) * 0x0080;
	carry = (result > 0x00FF);
	result = result & 0x00FF;
	result = result + carry + 0x13;
	carry = (result > 0x00FF);
	*RandSeed2 = result & 0x00FF;
	result = *RandSeed1 + carry + TempRand1;
	carry = (result > 0x00FF);
	result = result & 0x00FF;
	result = result + carry + TempRand2;
	*RandSeed1 = result & 0x00FF;
}
			
static void draw_point(int x,int y,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	field[x][y]=item;
}
static void draw_fillrect(int x1,int y1,int x2, int y2,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=x1;x<=x2;x++)
	{
		for(int y=y1;y<=y2;y++)
		{
			field[x][y]=item;
		}
	}
}
static void draw_rect(int x1,int y1,int x2, int y2,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=x1;x<=x2;x++)
	{
		for(int y=y1;y<=y2;y++)
		{
			if((x==x1)||(x==x2)||(y==y1)||(y==y2))
				field[x][y]=item;
		}
	}
}
static void draw_fillrect2(int x1,int y1,int x2, int y2,int item,int item2,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=x1;x<=x2;x++)
	{
		for(int y=y1;y<=y2;y++)
		{
			if((x==x1)||(x==x2)||(y==y1)||(y==y2))
				field[x][y]=item;
			else
				field[x][y]=item2;
		}
	}
}
static void draw_raster(int x1,int y1,int count_x, int count_y,int offset_x,int offset_y,int item,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	for(int x=0;x<count_x;x++)
	{
		for(int y=0;y<count_y;y++)
		{
			field[x1+(x*offset_x)][y1+(y*offset_y)]=item;
		}
	}
}

int render_field(int cave,int level,int field[CAVE_WIDTH][CAVE_HEIGHT])
{
	struct cave_struct_t *cavedata;

	cavedata = (struct cave_struct_t*)&bd_cave_data[bd_cave_start_idx[cave]]; 

	int RandSeed1 = 0;
	int RandSeed2 = cavedata->RandSeed[level];

	for(int x = 0; x < CAVE_WIDTH; x++) {
		for (int y = 0; y < CAVE_HEIGHT; y++) {
			field[x][y]=BD_STEELWALL;
		}
	}

	for(int y = 0; y < CAVE_HEIGHT-2; y++) {
		for(int x = 0; x < CAVE_WIDTH; x++) {
			NextRandom(&RandSeed1, &RandSeed2);
			int store=BD_DIRT;
			for (int caveDataaddIndex = 0; caveDataaddIndex < 4; caveDataaddIndex++) {
				if (RandSeed1 < cavedata->RandomFill[caveDataaddIndex*2+1]) {
					store = cavedata->RandomFill[caveDataaddIndex*2];
				}
			}
			if((x>0)&&(x<CAVE_WIDTH-1))
				field[x][y+1]=store;
		}     
	} 

	int *drawidx = (&cavedata->DrawItems)+1;
	int offset=0;

	for(int item=0;item<cavedata->DrawItems;item++)
	{
		int type = *(drawidx+offset);

		switch(type)
		{
			case BD_DRAW_LINE:
			case BD_DRAW_FILLRECT:
				draw_fillrect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),field);
				offset+=6;
				break;
			case BD_DRAW_FILLRECT2:
				draw_fillrect2(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),field);
				offset+=7;
				break;
			case BD_DRAW_RECTANGLE:
				draw_rect(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),field);
				offset+=6;
				break;
			case BD_DRAW_POINT:
				draw_point(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),field);
				offset+=4;
				break;
			case BD_DRAW_RASTER:
				draw_raster(*(drawidx+offset+1),*(drawidx+offset+2),*(drawidx+offset+3),*(drawidx+offset+4),*(drawidx+offset+5),*(drawidx+offset+6),*(drawidx+offset+7),field);
				offset+=8;
				break;
		}

	}
	return offset;
}

