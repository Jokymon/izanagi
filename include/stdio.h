#ifndef __STDIO_H
#define __STDIO_H

#include <stdarg.h>
#include <console.h>

#define VIDEO_BASE_ADDRESS 0xb8000

/*
 * STDIO based functions (can always be used)
 */

int	sprintf(char *buf, const char *fmt, ...) __attribute__ ((format(__printf__, 2, 3)));
int	snprintf(char *buf, size_t size, const char *fmt, ...) __attribute__ ((format(__printf__, 3, 4)));
int	vsprintf(char *buf, const char *fmt, va_list args);
char	*asprintf(const char *fmt, ...) __attribute__ ((format(__printf__, 1, 2)));
char	*vasprintf(const char *fmt, va_list ap);
int	vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int	vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

#ifndef CONFIG_CONSOLE_NONE
/* stdin */
int	tstc(void);

/* stdout */
int	    getc(void);


int	printf(const char *fmt, ...) __attribute__ ((format(__printf__, 1, 2)));
int	vprintf(const char *fmt, va_list args);
#else
static inline int tstc(void)
{
	return 0;
}

static inline int getc(void)
{
	return -EINVAL;
}

int printf(const char *fmt, ...);


static inline int vprintf(const char *fmt, va_list args)
{
	return 0;
}

#ifndef ARCH_HAS_CTRLC
/* test if ctrl-c was pressed */
static inline int ctrlc (void)
{
	return 0;
}
#endif /* ARCH_HAS_CTRC */

#endif

int puts(const char *s);
void putchar(char c);
void gotoxy(int x, int y);
void clrscr();

/*
 * FILE based functions
 */

#define stdin		0
#define stdout		1
#define stderr		2
#define MAX_FILES	128

int	fprintf(int file, const char *fmt, ...) __attribute__ ((format(__printf__, 2, 3)));
int	fputs(int file, const char *s);
int	fputc(int file, const char c);
int	ftstc(int file);
int	fgetc(int file);

#endif /* __STDIO_H */
