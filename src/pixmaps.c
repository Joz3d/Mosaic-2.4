/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#include "mosaic.h"
#include "pixmaps.h"


extern unsigned char *ProcessXpm3Data();


Pixmap IconPix[NUMBER_OF_FRAMES];
int IconsMade = 0;


static GC DrawGC = NULL;
static int IconWidth = 0;
static int IconHeight = 0;
static int WindowWidth = 0;
static int WindowHeight = 0;

static struct color_rec {
        int pixel[3];
        int pixelval;
        struct color_rec *hash_next;
} *Hash[256];


#if defined(__STDC__) || defined(__sgi)
#define MAKE_PIXMAP_FROM_ICON(num) \
  if ((num > 0)&&(num <= 100)) \
    { \
      unsigned char *data; \
      int indx, w, h, bg; \
      XColor colrs[256]; \
      data = ProcessXpm3Data(wid, icon##num, &w, &h, colrs, &bg); \
      indx = (num - 1); \
      IconPix[indx] = PixmapFromData(wid, data, w, h, colrs); \
      if ((IconWidth == 0)||(IconHeight == 0)) { \
        IconWidth = w; IconHeight = h; \
      } \
    }
#else
#define MAKE_PIXMAP_FROM_ICON(num) \
  if ((num > 0)&&(num <= 100)) \
    { \
      unsigned char *data; \
      int indx, w, h, bg; \
      XColor colrs[256]; \
      data = ProcessXpm3Data(wid, icon/**/num, &w, &h, colrs, &bg); \
      indx = (num - 1); \
      IconPix[indx] = PixmapFromData(wid, data, w, h, colrs); \
      if ((IconWidth == 0)||(IconHeight == 0)) { \
        IconWidth = w; IconHeight = h; \
      } \
    }
#endif /* __STDC__ */


/*
 * Find the closest color by allocating it, or picking an already allocated
 * color
 */
static void
FindIconColor(Display *dsp, Colormap colormap, XColor *colr)
{
	int i, match;
	int rd, gd, bd, dist, mindist;
	int cindx;
static XColor def_colrs[256];
static int init_colors = 1;

	if (init_colors)
	{
		for (i=0; i<256; i++)
		{
			def_colrs[i].pixel = -1;
			def_colrs[i].red = 0;
			def_colrs[i].green = 0;
			def_colrs[i].blue = 0;
		}
		init_colors = 0;
	}

	match = XAllocColor(dsp, colormap, colr);
	if (match == 0)
	{
		mindist = 196608;		/* 256 * 256 * 3 */
		cindx = colr->pixel;
		for (i=0; i<256; i++)
		{
			if (def_colrs[i].pixel == -1)
			{
				continue;
			}
			rd = ((int)(def_colrs[i].red >> 8) -
				(int)(colr->red >> 8));
			gd = ((int)(def_colrs[i].green >> 8) -
				(int)(colr->green >> 8));
			bd = ((int)(def_colrs[i].blue >> 8) -
				(int)(colr->blue >> 8));
			dist = (rd * rd) +
				(gd * gd) +
				(bd * bd);
			if (dist < mindist)
			{
				mindist = dist;
				cindx = def_colrs[i].pixel;
				if (dist == 0)
				{
					break;
				}
			}
		}
		colr->pixel = cindx;
		colr->red = def_colrs[cindx].red;
		colr->green = def_colrs[cindx].green;
		colr->blue = def_colrs[cindx].blue;
	}
	else
	{
		def_colrs[colr->pixel].pixel = colr->pixel;
		def_colrs[colr->pixel].red = colr->red;
		def_colrs[colr->pixel].green = colr->green;
		def_colrs[colr->pixel].blue = colr->blue;
	}
}


#define PixFindHash(red, green, blue, h_ptr) \
	h_ptr = Hash[((((red * 306) + (green * 601) + (blue * 117)) >> 10) >> 8)]; \
	while(h_ptr != NULL) \
	{ \
		if ((h_ptr->pixel[0] == red)&& \
		    (h_ptr->pixel[1] == green)&& \
		    (h_ptr->pixel[2] == blue)) \
		{ \
			break; \
		} \
		h_ptr = h_ptr->hash_next; \
	}


static void
PixAddHash(int red, int green, int blue, int pixval)
{
	int lum;
	struct color_rec *hash_ptr;

	lum = ((((red * 306) + (green * 601) + (blue * 117)) >> 10) >> 8);

	hash_ptr = (struct color_rec *)XtMalloc(sizeof(struct color_rec));
	if (hash_ptr == NULL)
	{
		return;
	}
	hash_ptr->pixel[0] = red;
	hash_ptr->pixel[1] = green;
	hash_ptr->pixel[2] = blue;
	hash_ptr->pixelval = pixval;
	hash_ptr->hash_next = Hash[lum];
	Hash[lum] = hash_ptr;
}


static void
InitHash(void)
{
	int i;

	for (i=0; i<256; i++)
	{
		Hash[i] = NULL;
	}
}


static int
highbit(ul)
unsigned long ul;
{
	/*
	 * returns position of highest set bit in 'ul' as an integer (0-31),
	 * or -1 if none.
	 */
 
	int i;
	for (i=31; ((ul&0x80000000) == 0) && i>=0;  i--, ul<<=1);
	return i;
}


static Pixmap
PixmapFromData(Widget wid, unsigned char *data, int width, int height, XColor *colrs)
{
	int i;
	int linepad, shiftnum;
	int shiftstart, shiftstop, shiftinc;
	int bytesperline;
	int temp;
	int w, h;
	XImage *newimage;
	unsigned char *bit_data, *bitp, *datap;
	unsigned char *tmpdata;
	Pixmap pix;
	int Mapping[256];
	XColor tmpcolr;
	int size;
	int depth;
	int Vclass;
	XVisualInfo vinfo, *vptr;
	Visual *theVisual;
	int bmap_order;
	unsigned long c;
	int rshift, gshift, bshift;

	if (data == NULL)
	{
		return(NULL);
	}

	/* find the visual class. */
	vinfo.visualid = XVisualIDFromVisual(DefaultVisual(XtDisplay(wid),
		DefaultScreen(XtDisplay(wid))));
	vptr = XGetVisualInfo(XtDisplay(wid), VisualIDMask, &vinfo, &i);
	Vclass = vptr->class;
	XFree((char *)vptr);

	depth = DefaultDepthOfScreen(XtScreen(wid));

        for (i=0; i < 256; i++)
        {
		struct color_rec *hash_ptr;

                tmpcolr.red = colrs[i].red;
                tmpcolr.green = colrs[i].green;
                tmpcolr.blue = colrs[i].blue;
                tmpcolr.flags = DoRed|DoGreen|DoBlue;
                if ((Vclass == TrueColor) || (Vclass == DirectColor))
                {
                        Mapping[i] = i;
                }
                else
                {
			PixFindHash(tmpcolr.red, tmpcolr.green, tmpcolr.blue,
				hash_ptr);
			if (hash_ptr == NULL)
			{
				FindIconColor(XtDisplay(wid),
					DefaultColormapOfScreen(XtScreen(wid)),
					&tmpcolr);
				PixAddHash(colrs[i].red, colrs[i].green,
					colrs[i].blue, tmpcolr.pixel);
				Mapping[i] = tmpcolr.pixel;
			}
			else
			{
				Mapping[i] = hash_ptr->pixelval;
			}
                }
        }

	size = width * height;
	tmpdata = (unsigned char *)malloc(size);
	datap = data;
	bitp = tmpdata;
	for (i=0; i < size; i++)
	{
		*bitp++ = (unsigned char)Mapping[(int)*datap];
		datap++;
	}
	free((char *)data);
	data = tmpdata;

	switch(depth)
	{
	    case 6:
	    case 8:
		bit_data = (unsigned char *)malloc(size);
		bcopy(data, bit_data, size);
		bytesperline = width;
		newimage = XCreateImage(XtDisplay(wid),
			DefaultVisual(XtDisplay(wid),
				DefaultScreen(XtDisplay(wid))),
			depth, ZPixmap, 0, (char *)bit_data,
			width, height, 8, bytesperline);
		break;
	    case 1:
	    case 2:
	    case 4:
		if (BitmapBitOrder(XtDisplay(wid)) == LSBFirst)
		{
			shiftstart = 0;
			shiftstop = 8;
			shiftinc = depth;
		}
		else
		{
			shiftstart = 8 - depth;
			shiftstop = -depth;
			shiftinc = -depth;
		}
		linepad = 8 - (width % 8);
		bit_data = (unsigned char *)malloc(((width + linepad) * height)
				+ 1);
		bitp = bit_data;
		datap = data;
		*bitp = 0;
		shiftnum = shiftstart;
		for (h=0; h<height; h++)
		{
			for (w=0; w<width; w++)
			{
				temp = *datap++ << shiftnum;
				*bitp = *bitp | temp;
				shiftnum = shiftnum + shiftinc;
				if (shiftnum == shiftstop)
				{
					shiftnum = shiftstart;
					bitp++;
					*bitp = 0;
				}
			}
			for (w=0; w<linepad; w++)
			{
				shiftnum = shiftnum + shiftinc;
				if (shiftnum == shiftstop)
				{
					shiftnum = shiftstart;
					bitp++;
					*bitp = 0;
				}
			}
		}
		bytesperline = (width + linepad) * depth / 8;
		newimage = XCreateImage(XtDisplay(wid),
			DefaultVisual(XtDisplay(wid),
				DefaultScreen(XtDisplay(wid))),
			depth, ZPixmap, 0, (char *)bit_data,
			(width + linepad), height, 8, bytesperline);
		break;
	    /*
	     * WARNING:  This depth 16 code is donated code for 16 but
	     * TrueColor displays.  I have no access to such displays, so I
	     * can't really test it.
	     * Donated by - andrew@icarus.demon.co.uk
	     */
	    case 16:
		bit_data = (unsigned char *)malloc(size * 2);
		bitp = bit_data;
		datap = data;
		for (w = size; w > 0; w--)
		{
			temp = (((colrs[(int)*datap].red   >> 1) & 0x7c00) |
				((colrs[(int)*datap].green >> 6) & 0x03e0) |
				((colrs[(int)*datap].blue  >> 11) & 0x001f));

			if (BitmapBitOrder(XtDisplay(wid)) == MSBFirst)
			{
				*bitp++ = (temp >> 8) & 0xff;
				*bitp++ = temp & 0xff;
			}
			else
			{
				*bitp++ = temp & 0xff;
				*bitp++ = (temp >> 8) & 0xff;
			}

			datap++;
		}

		newimage = XCreateImage(XtDisplay(wid),
			DefaultVisual(XtDisplay(wid),
				DefaultScreen(XtDisplay(wid))),
			depth, ZPixmap, 0, (char *)bit_data,
			width, height, 16, 0);
		break;
	    case 24:
		bit_data = (unsigned char *)malloc(size * 4);

		theVisual = DefaultVisual(XtDisplay(wid),
			DefaultScreen(XtDisplay(wid)));
		rshift = highbit(theVisual->red_mask) - 7;
		gshift = highbit(theVisual->green_mask) - 7;
		bshift = highbit(theVisual->blue_mask) - 7;
		bmap_order = BitmapBitOrder(XtDisplay(wid));

		bitp = bit_data;
		datap = data;
		for (w = size; w > 0; w--)
		{
			c =
			  (((colrs[(int)*datap].red >> 8) & 0xff) << rshift) |
			  (((colrs[(int)*datap].green >> 8) & 0xff) << gshift) |
			  (((colrs[(int)*datap].blue >> 8) & 0xff) << bshift);

			datap++;

			if (bmap_order == MSBFirst)
			{
				*bitp++ = (unsigned char)((c >> 24) & 0xff);
				*bitp++ = (unsigned char)((c >> 16) & 0xff);
				*bitp++ = (unsigned char)((c >> 8) & 0xff);
				*bitp++ = (unsigned char)(c & 0xff);
			}
			else
			{
				*bitp++ = (unsigned char)(c & 0xff);
				*bitp++ = (unsigned char)((c >> 8) & 0xff);
				*bitp++ = (unsigned char)((c >> 16) & 0xff);
				*bitp++ = (unsigned char)((c >> 24) & 0xff);
			}
		}

		newimage = XCreateImage(XtDisplay(wid),
			DefaultVisual(XtDisplay(wid),
				DefaultScreen(XtDisplay(wid))),
			depth, ZPixmap, 0, (char *)bit_data,
			width, height, 32, 0);
		break;
	    default:
		fprintf(stderr, "Don't know how to format image for display of depth %d\n", depth);
		newimage = NULL;
	}
	free((char *)data);

	if (newimage != NULL)
	{
		GC drawGC;

		pix = XCreatePixmap(XtDisplay(wid), XtWindow(wid),
			width, height, depth);
		drawGC = XCreateGC(XtDisplay(wid), XtWindow(wid), 0, NULL);
		XSetFunction(XtDisplay(wid), drawGC, GXcopy);

		XPutImage(XtDisplay(wid), pix, drawGC, newimage, 0, 0,
			0, 0, width, height);
		XFreeGC(XtDisplay(wid), drawGC);
		XDestroyImage(newimage);
		return(pix);
	}
	else
	{
		return(NULL);
	}
}


void
MakeAnimationPixmaps(Widget wid)
{
	int i;

	for (i=0; i<NUMBER_OF_FRAMES; i++)
	{
		IconPix[i] = NULL;
	}

	InitHash();

	MAKE_PIXMAP_FROM_ICON(1);
	MAKE_PIXMAP_FROM_ICON(2);
	MAKE_PIXMAP_FROM_ICON(3);
	MAKE_PIXMAP_FROM_ICON(4);
	MAKE_PIXMAP_FROM_ICON(5);
	MAKE_PIXMAP_FROM_ICON(6);
	MAKE_PIXMAP_FROM_ICON(7);
	MAKE_PIXMAP_FROM_ICON(8);
	MAKE_PIXMAP_FROM_ICON(9);
	MAKE_PIXMAP_FROM_ICON(10);
	MAKE_PIXMAP_FROM_ICON(11);
	MAKE_PIXMAP_FROM_ICON(12);
	MAKE_PIXMAP_FROM_ICON(13);
	MAKE_PIXMAP_FROM_ICON(14);
	MAKE_PIXMAP_FROM_ICON(15);
	MAKE_PIXMAP_FROM_ICON(16);
	MAKE_PIXMAP_FROM_ICON(17);
	MAKE_PIXMAP_FROM_ICON(18);
	MAKE_PIXMAP_FROM_ICON(19);
	MAKE_PIXMAP_FROM_ICON(20);
	MAKE_PIXMAP_FROM_ICON(21);
	MAKE_PIXMAP_FROM_ICON(22);
	MAKE_PIXMAP_FROM_ICON(23);
	MAKE_PIXMAP_FROM_ICON(24);
	MAKE_PIXMAP_FROM_ICON(25);
}


void
AnimatePixmapInWidget(Widget wid, Pixmap pix)
{
	Cardinal argcnt;
	Arg arg[5];
	int x, y;

	if ((WindowWidth == 0)||(WindowHeight == 0))
	{
		Dimension w, h;

		argcnt = 0;
		XtSetArg(arg[argcnt], XtNwidth, &w); argcnt++;
		XtSetArg(arg[argcnt], XtNheight, &h); argcnt++;
		XtGetValues(wid, arg, argcnt);
		WindowWidth = w;
		WindowHeight = h;
	}

	if (DrawGC == NULL)
	{
		DrawGC = XCreateGC(XtDisplay(wid), XtWindow(wid), 0, NULL);
		XSetFunction(XtDisplay(wid), DrawGC, GXcopy);
	}
	x = (WindowWidth - IconWidth) / 2;
	if (x < 0)
	{
		x = 0;
	}
	y = (WindowHeight - IconHeight) / 2;
	if (y < 0)
	{
		y = 0;
	}
	XCopyArea(XtDisplay(wid),
		pix, XtWindow(wid), DrawGC,
		0, 0, IconWidth, IconHeight, x, y);
}

