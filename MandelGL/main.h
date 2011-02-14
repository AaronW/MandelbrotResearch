//
//  main.h
//  MandelGL
//
//  Created by Aaron on 2/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#define ImageHeight 480         //#define ImageHeight 480
#define ImageWidth 640          //#define ImageWidth 640
#define false 0
#define true 1
#define MAXITER 100
#define NUM_THREADS 2
int guiMethod;
// Necessary function prototypes
void mandelPthread();
void mandelbrot();
void printArray();