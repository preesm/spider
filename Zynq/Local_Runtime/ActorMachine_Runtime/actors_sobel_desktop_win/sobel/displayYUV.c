/*
	============================================================================
	Name        : displayYUV.c
	Author      : mpelcat
	Version     :
	Copyright   : Displaying YUV frames one next to another in a row
	Description :
	============================================================================
*/

#include "../sobel.h"
#include <SDL/SDL.h>
#include <SDL/SDL_error.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_video.h>
#include <time.h>
#include <lrt.h>
#include <lrt_prototypes.h>

#define TFF_FILE_PATH	"C:\\work\\COMPA\\arial.ttf"

/**
* Structure representing one display
*/
typedef struct YuvDisplay{
    SDL_Overlay* overlay;	// One overlay per frame
    SDL_Surface* screen;	// SDL surface where to display
    int initialized;		// Initialization done ?
} YuvDisplay;


// State of the yuvDisplay actor: an overlay of fixed size
static YuvDisplay display;
static TTF_Font* font;
static SDL_Color foregroundColor = { 255, 255, 255 };

/**
* Initializes a display frame. Be careful, once a window size has been chosen,
* all videos must share the same window size
*
* @param id display unique identifier
* @param xsize width
* @param ysize heigth
*/
void displayYUVInit (int xsize, int ysize){
    if(ysize > DISPLAY_H){
        fprintf(stderr, "SDL screen is not high enough.");
        exit(1);
    }else if(xsize > DISPLAY_W){
        fprintf(stderr, "SDL screen is not wide enough.");
        exit(1);
    }

    if(display.initialized==0){
        // Generating window name
        char* name = "Display";
        display.initialized = 1;

        if(SDL_Init(SDL_INIT_VIDEO)){
            fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
            exit(1);
        }

        display.screen = SDL_SetVideoMode(DISPLAY_W, DISPLAY_H, 32, SDL_HWSURFACE);
        SDL_WM_SetCaption(name, name);
        if(!display.screen){
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }
        display.overlay = SDL_CreateYUVOverlay(DISPLAY_W, DISPLAY_H,
        		SDL_YV12_OVERLAY, display.screen);
    }
    memset(display.overlay->pixels[0], 0, display.overlay->pitches[0]);
    memset(display.overlay->pixels[1], 0, display.overlay->pitches[1]);
    memset(display.overlay->pixels[2], 0, display.overlay->pitches[2]);

	TTF_Init();
	font = TTF_OpenFont(TFF_FILE_PATH, 30);
	if(font == NULL) printf("error loading arial.ttf: %s\n",TTF_GetError());
	printf("TTF: ok!\n");
}
/**
* Display one YUV frame
*
* @param id display unique identifier
* @param y luma
* @param u chroma U
* @param v chroma V
*/
void displayYUV(){
//    SDL_Event event;
	unsigned char y[DISPLAY_W*DISPLAY_H];
	unsigned char u[DISPLAY_W*DISPLAY_H/4];
	unsigned char v[DISPLAY_W*DISPLAY_H/4];

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	if(action->nb_param != 3 || action->nb_fifo_in != 3 || action->nb_fifo_out != 0){
		printf("Display: Error in parameters or fifos count\n");
		printf("param = %d (3), in = %d (3), out = %d (0)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}
	int width  = action->param_value[0];
	int height = action->param_value[1];
	int color = action->param_value[2];
	int imageSize = width*height;

	read_input_fifo(action->fifo_in_id[0], imageSize, y);
	read_input_fifo(action->fifo_in_id[1], color*imageSize/4, u);
	read_input_fifo(action->fifo_in_id[2], color*imageSize/4, v);

	displayYUVargs(y, u, v, width, height, color);
}

void displayYUVargs(unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color){
    SDL_Rect video_rect = {0,0, width, height};	// SDL frame position and size (x, y, w, h)
    SDL_Event event;
	SDL_Surface* textSurface;
	char text[3];
	int i,j;

    static time_t start, end;
	long fps, seconds, useconds;

    int imageSize = width*height;

    if (SDL_LockYUVOverlay(display.overlay) < 0){
        fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
        system("PAUSE");
    }


    memcpy(display.overlay->pixels[0], y, imageSize);

    if(color){
		memcpy(display.overlay->pixels[1], v, imageSize/4);
		memcpy(display.overlay->pixels[2], u, imageSize/4);
    }else{
		memset(display.overlay->pixels[1], 128, imageSize/4);
		memset(display.overlay->pixels[2], 128, imageSize/4);
    }


	time(&end);
	seconds = difftime(end, start);
//	seconds  = end.tv_sec  - start.tv_sec;
//	useconds = end.tv_usec - start.tv_usec;
	fps = 1000/(((seconds) * 1000) + 0.5);
//	fps = 1000/(((seconds) * 1000 + useconds/1000.0) + 0.5);
//	start.tv_sec = end.tv_sec;
//	start.tv_usec = end.tv_usec;
	start = end;

    sprintf(text, "%ld", fps);
	textSurface = TTF_RenderText_Solid(font, text, foregroundColor);
	unsigned char* pixels = textSurface->pixels;

	for(j=0; j<textSurface->h; j++){
		int J = height-textSurface->h+j;

		for(i=0; i<textSurface->w; i++){
			int I = width-textSurface->w+i;

			display.overlay->pixels[0][I+J*width] = pixels[i+j*textSurface->pitch]>0?255:0;   /* Y */
			display.overlay->pixels[1][((I/2)+(J/2)*width/2)] = 127;  /* V */
			display.overlay->pixels[2][((I/2)+(J/2)*width/2)] = 127; /* U */

		}
	}
	SDL_FreeSurface(textSurface);


    SDL_UnlockYUVOverlay(display.overlay);



//	SDL_Flip(display.screen);

	SDL_DisplayYUVOverlay(display.overlay, &video_rect);

    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(0);
            break;
        default:
            break;
        }
    }
}
