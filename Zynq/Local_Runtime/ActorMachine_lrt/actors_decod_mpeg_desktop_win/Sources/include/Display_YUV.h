/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *  This program is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
   *
   *  This program is distributed in the hope that it will be useful,
   *  but WITHOUT ANY WARRANTY; without even the implied warranty of
   *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   *  GNU General Public License for more details.
   *
   *  You should have received a copy of the GNU General Public License
   *  along with this program; if not, write to the Free Software
   *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
   *
   * $Id$
   *
   **************************************************************************/
/************************************************************************
   *
   *  yuvrgb24.c, colour space conversion for tmndecode (H.263 decoder)
   *  Copyright (C) 1995, 1996  Telenor R&D, Norway
   *
   *
   *  Copyright (C) 1997  University of BC, Canada
   *  Modified by: Michael Gallant <mikeg@ee.ubc.ca>
   *               Guy Cote <guyc@ee.ubc.ca>
   *               Berna Erol <bernae@ee.ubc.ca>
   *
   *  Contacts:
   *  Michael Gallant                   <mikeg@ee.ubc.ca>
   *
   *
   ************************************************************************/
#ifndef POCKET_PC
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <vfw.h>

#include <memory.h>

typedef struct
{
   HANDLE            hThread ;
   HANDLE            hEvent ;
   HWND              hWnd ;
   MSG               msg ;
   WNDCLASS          wc ;
   HDRAWDIB          hDrawDib ;
   HDC               hDC ;
   BITMAPINFOHEADER  biHeader ;
   char              lpszAppName [30];
   DWORD             dwThreadID ;
   BOOL              imageIsReady ;
   unsigned char     *bufRGB ;
   RECT              rect ;
   unsigned char     *src [3];
   int               width, height ;
   int               zoom, oldzoom ;
   int               windowDismissed ;
   int				 edge;
}  T_VDWINDOW_YUV ;

#define VIDEO_BEGIN (WM_USER + 0)
#define VIDEO_DRAW_FRAME (WM_USER + 1)
#define VIDEO_REDRAW_FRAME (WM_USER + 2)
#define VIDEO_END (WM_USER + 3)
#define VIDEO_RESIZE (WM_USER + 4)
void           Display_YUV_init (int Number, int xsize, int ysize, char *comment) ;
int            initDisplay_YUV (int Number, int pels, int lines, char *comment) ;
int            InitDisplayWindowThread_YUV (int Number, int width, int height, char *comment) ;
void           DisplayWinMain_YUV (int Number) ;
void           init_dither_tab () ;

/* To display a picture */
void           Display_YUV (int Number, unsigned char *Y, unsigned char *Cb
   , unsigned char *Cr) ;
void           Display_Dyna_YUV ( int Number, int edge, int xsize, int ysize, unsigned char *Y, unsigned char *Cb
   , unsigned char *Cr );
int            displayImage_YUV (int Number, unsigned char *lum
   , unsigned char *Cb, unsigned char *Cr) ;
LONG APIENTRY  MainWndProc_YUV(HWND, UINT, UINT, LONG);
int            DrawDIB_YUV (int Number) ;
void           ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1
   , unsigned char *src2, unsigned char *dst_ori, int width, int height) ;

/* close the Display */
void           Display_YUV_end (int Number) ;
int            closeDisplay_YUV (int Number) ;

#endif
