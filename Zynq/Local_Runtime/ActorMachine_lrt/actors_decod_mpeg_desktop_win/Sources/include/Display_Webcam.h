#ifndef POCKET_PC
#include <windows.h>
#include <process.h>


#include <vfw.h>
#include <memory.h>

typedef struct
{
  HANDLE    hThread ;
  HANDLE    hEvent ;
  HWND      hWnd ;
  MSG       msg ;
  WNDCLASS  wc ;
  char      lpszAppName [15];
  DWORD     dwThreadID ;
  int       width, height ;
  
  // int zoom, oldzoom;
  int       windowDismissed ;
} T_VDWINDOW_Webcam ;

#define VIDEO_BEGIN (WM_USER + 0)
#define VIDEO_DRAW_FRAME (WM_USER + 1)
#define VIDEO_REDRAW_FRAME (WM_USER + 2)
#define VIDEO_END (WM_USER + 3)
#define VIDEO_RESIZE (WM_USER + 4)
void                    WebCam_YUV (unsigned char *Y, unsigned char *U, unsigned char *V) ;
void                    WebCam_YUV_init (int xsize, int ysize) ;
int                     initDisplay_Webcam (int pels, int lines) ;
int                     InitDisplayWindowThread_Webcam (int width, int height) ;
void                    DisplayWinMain_Webcam () ;
LRESULT                 CALLBACK capVideoStreamCallback (HWND hWnd, LPVIDEOHDR lpVHdr) ;
LRESULT                 CALLBACK WindowProcedure (HWND hwnd, UINT message
  , WPARAM wParam, LPARAM lParam) ;
LRESULT                 PASCAL ErrorCallbackProc (HWND hWnd, int nErrID, LPSTR lpErrorText) ;

/* To display a picture */
LONG APIENTRY MainWndProc_Webcam (HWND, UINT, UINT, LONG) ;

/* close the Display */
void                    Display_Webcam_end () ;
int                     closeDisplay_Webcam () ;


#endif
