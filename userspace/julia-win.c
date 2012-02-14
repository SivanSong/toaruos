/*
 * Julia Fractal Generator
 */

#include <stdio.h>
#include <stdint.h>
#include <syscall.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "lib/window.h"

/*
 * Macros make verything easier.
 */
#define SPRITE(sprite,x,y) sprite->bitmap[sprite->width * (y) + (x)]

#define GFX_(xpt, ypt) ((uint32_t *)window->buffer)[DIRECT_OFFSET(xpt,ypt)]

/* Pointer to graphics memory */
window_t * window = NULL;

/* Julia fractals elements */
float conx = -0.74;  /* real part of c */
float cony = 0.1;    /* imag part of c */
float Maxx = 2;      /* X bounds */
float Minx = -2;
float Maxy = 1;      /* Y bounds */
float Miny = -1;
float initer = 1000; /* Iteration levels */
float pixcorx;       /* Internal values */
float pixcory;

int newcolor;        /* Color we're placing */
int lastcolor;       /* Last color we placed */
int no_repeat = 0;   /* Repeat colors? */

/*
 * Color table
 * These are orange/red shades from the Ubuntu platte.
 */
int colors[] = {
	0xeec73e,
	0xf0a513,
	0xfb8b00,
	0xf44800,
	0xffff99,
	0xffff00,
	0xfdca01,
	0x986601,
	0xf44800,
	0xfd3301,
	0xd40000,
	0x980101,
};

void julia(int xpt, int ypt) {
	long double x = xpt * pixcorx + Minx;
	long double y = Maxy - ypt * pixcory;
	long double xnew = 0;
	long double ynew = 0;

	int k = 0;
	for (k = 0; k <= initer; k++) {
		xnew = x * x - y * y + conx;
		ynew = 2 * x * y     + cony;
		x    = xnew;
		y    = ynew;
		if ((x * x + y * y) > 4)
			break;
	}

	int color;
	if (no_repeat) {
		color = 12 * k / initer;
	} else {
		color = k;
		if (color > 11) {
			color = color % 12;
		}
	}
	if (k >= initer) {
		((uint32_t *)window->buffer)[DIRECT_OFFSET(xpt,ypt)] = 0;
	} else {
		((uint32_t *)window->buffer)[DIRECT_OFFSET(xpt,ypt)] = colors[color];
	}
	newcolor = color;
}

int main(int argc, char ** argv) {
	if (argc < 5) {
		printf("usage: %s left top width height\n", argv[0]);
		return -1;
	}

	int left = atoi(argv[1]);
	int top = atoi(argv[2]);
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);

	setup_windowing();

	window = window_create(left, top, width, height);
	window_fill(window, rgb(127,127,127));

#if 0
	graphics_width  = syscall_getgraphicswidth();
	graphics_height = syscall_getgraphicsheight();
	graphics_depth  = syscall_getgraphicsdepth();
	gfx_mem = (void *)syscall_getgraphicsaddress();
#endif



#if 0
	if (argc > 1) {
		/* Read some arguments */
		int index, c;
		while ((c = getopt(argc, argv, "ni:x:X:c:C:")) != -1) {
			switch (c) {
				case 'n':
					no_repeat = 1;
					break;
				case 'i':
					initer = atof(optarg);
					break;
				case 'x':
					Minx = atof(optarg);
					break;
				case 'X':
					Maxx = atof(optarg);
					break;
				case 'c':
					conx = atof(optarg);
					break;
				case 'C':
					cony = atof(optarg);
					break;
				default:
					break;
			}
		}
	}
#endif
	
	printf("initer: %f\n", initer);
	printf("X: %f %f\n", Minx, Maxx);
	float _x = Maxx - Minx;
	float _y = _x / width * height;
	Miny = 0 - _y / 2;
	Maxy = _y / 2;
	printf("Y: %f %f\n", Miny, Maxy);
	printf("conx: %f cony: %f\n", conx, cony);

	pixcorx = (Maxx - Minx) / width;
	pixcory = (Maxy - Miny) / height;
	int j = 0;
	do {
		int i = 1;
		do {
			julia(i,j);
			if (lastcolor != newcolor) julia(i-1,j);
			else if (i > 0) GFX_(i-1,j) = colors[lastcolor];
			newcolor = lastcolor;
			i+= 2;
		} while ( i < width );
		++j;
		window_redraw_full(window);
	} while ( j < height );

	while (1) {
		
	}

	return 0;
}
