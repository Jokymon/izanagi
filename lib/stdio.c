#include <io.h>
#include <stdio.h>

static int cursor_x, cursor_y;
static int output_color;

int puts(const char *s)
{
    while (*s != 0)
    {
        putchar(*s++);
    }
    return 0;
}

void putchar(char c)
{
    if (c=='\r')
    {
        cursor_x = 0;
    }
    else if (c=='\n')
    {
        cursor_x = 0;
        cursor_y++;
    }
    else
    {
        volatile char* video = (volatile char*)(VIDEO_BASE_ADDRESS + 
                                                2*(80*cursor_y + cursor_x++));
        *video++ = c;
        *video = output_color;
    }

    gotoxy(cursor_x, cursor_y);
}

void gotoxy(int x, int y)
{
    unsigned short position=(y*80) + x;
 
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));

    cursor_x = x;
    cursor_y = y;
}

void clrscr()
{
    volatile char* video = (volatile char*)VIDEO_BASE_ADDRESS;
    int i;
    for (i=0; i<80*25; i++)
    {
        *video++ = ' ';
        *video++ = 7;
    }

    cursor_x = 0;
    cursor_y = 0;

    output_color = 7;
}

int printf(const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start(args, fmt);

	/*
	 * For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end(args);

	/* Print the string */
	puts(printbuffer);

	return i;
}

