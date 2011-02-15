//
//  main.h
//  MandelGL
//
//  Created by Aaron on 2/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#define IMAGEHEIGHT 480         //#define ImageHeight 480
#define IMAGEWIDTH 640          //#define ImageWidth 640
#define FALSE 0
#define TRUE 1
#define MAXITER 100
#define NUM_THREADS 2
int guiMethod;
// Necessary function prototypes
void mandelPthread();
void mandelSingle();
void printArray();