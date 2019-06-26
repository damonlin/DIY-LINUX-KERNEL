#define ORIG_X			(*(unsigned char *)0x90000)
#define ORIG_Y			(*(unsigned char *)0x90001)
#define ORIG_VIDEO_COLS 	(80)
#define ORIG_VIDEO_LINES	(25)

static unsigned long 	x,y;
static unsigned long 	video_mem_start;
static unsigned char	attr = 0x07;	// fg = white, bg = black

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