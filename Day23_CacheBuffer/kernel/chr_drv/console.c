#include <asm/io.h>

#define ORIG_X			(*(unsigned char *)0x90000)
#define ORIG_Y			(*(unsigned char *)0x90001)
#define ORIG_VIDEO_COLS 	(80)
#define ORIG_VIDEO_LINES	(25)

static unsigned long 	x,y;
static unsigned long 	video_mem_start;
static unsigned long	pos;
static unsigned char	attr = 0x07;	// fg = white, bg = black
static unsigned short   video_port_reg	= 0x3d4;
static unsigned short	video_port_val	= 0x3d5;

static inline void gotoxy(unsigned int new_x,unsigned int new_y)
{
	if (x > ORIG_VIDEO_COLS || y >= ORIG_VIDEO_LINES)
		return;

	x = new_x;
	y = new_y;
	pos = video_mem_start + y*ORIG_VIDEO_COLS*2 + (x<<1);
}

static inline void set_cursor()
{        	
	outb_p(14, video_port_reg);
	outb_p(0xff&((pos-video_mem_start)>>9), video_port_val);
	outb_p(15, video_port_reg);
	outb_p(0xff&((pos-video_mem_start)>>1), video_port_val);	
}

static void putchar(char c)
{
	if( c != '\n' )
	{
		const int index = y * ORIG_VIDEO_COLS + x;
		*((unsigned short*)video_mem_start + index) = c | (attr << 8);
	}

	if ( ++x == ORIG_VIDEO_COLS || c == '\n')
	{
		x = 0;
		if ( ++y == ORIG_VIDEO_LINES )
		{
			y = 0;
		}
	}    	

	gotoxy(x, y);
	set_cursor();
}

void con_write(const char* data)
{	
	char* str = (char*)data;
	while(*str)
	{
		putchar(*str);
		str++;
	}
}

void con_init(void)
{
	x = ORIG_X;
	y = ORIG_Y;
	video_mem_start = 0xB8000;
}