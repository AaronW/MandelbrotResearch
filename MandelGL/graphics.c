//
//  Senior Research Project: Mandelbrot Dispatched
//  Programmer: Aaron Weinberger
//  Technical Advisor: Dr. Hastings
//  February 22, 2011
//  graphics.c
//
#include <stdio.h>
#include "graphics.h"
#include "main.h"
// Print to file instead of stdout
void color(FILE *fp, int red, int green, int blue)  {
	fputc((char)red,fp);
	fputc((char)green,fp);
	fputc((char)blue,fp);
}

// Creates a .ppm file from the given implementation's results.
// Based on work by Eric R. Weeks http://www.physics.emory.edu/~weeks/software/mand.html
void ppmArray(int imageArray[IMAGEWIDTH][IMAGEHEIGHT]) {
    FILE *fp;                                               // Pointer for the file we're creating or overwriting
    fp = fopen("/Volumes/Macintosh HD/Users/aaron/Documents/MandelGL/MandelGL/mandelbrot.ppm", "w"); // NEEDS FULL PATH!!! NOT EVEN ~ WORKS!!!
	
	// Header for PPM output
	fprintf(fp,"P6\n# CREATOR: Aaron Weinberger/MandelbrotPPM based on work by Eric R. Weeks\n");
	fprintf(fp,"%d %d\n255\n",IMAGEWIDTH,IMAGEHEIGHT);
    
    for(int i=0; i<IMAGEWIDTH; i++) {
        for(int j=0; j<IMAGEHEIGHT; j++) {
            int iterationCount = imageArray[j][i];  // TODO Why is this printing seemingly backwards?
            if(iterationCount<MAXITER)              // TODO Remove magic numbers, possibly with something like #define colorOffset
                color(fp,iterationCount+COLOROFFSET*2,iterationCount+COLOROFFSET/2,iterationCount+COLOROFFSET);     // Escapes!
			else
                color(fp,0,0,0);                    // Doesn't escape
        }
    }
    fflush(fp);                     // Always flush the buffer to ensure it's all written out.
    fclose(fp);                     // Explicitly close the file now.
}