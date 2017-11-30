#pragma once

#ifdef ANIMATE
//#include <gl/gl.h>

#define NAPTIME 15L		/* 100ths of a second */
#define MAXPTS 500002

#define SQRhf 0.70710678


#define myBLACK   0x00000000L
#define myRED     0x9f00009fL
#define myGREEN   0x9f009f00L
#define myYELLOW  0x9f009f9fL
#define myBLUE    0x9f9f0000L
#define myMAGENTA 0x9f9f009fL
#define myCYAN    0x9f9f9f00L
#define myWHITE   0x9f9f9f9fL
#define myGREY    0x9f4f4f4fL
#define myTRANSP1 0x80dfdfdf & myCYAN 
#define myTRANSP2 0x80dfdfdf & myGREEN
#define mycolor(c) cpack(c)

extern int A_delay, A_swapinterval, A_modeq, A_ack;
extern char *A_modes[5];

extern double aspect;			/* aspect ration x/y */

void A_Init(), A_Quit(), A_Setup(), 
  A_Clear(), A_ClearBack(), A_SwapBuffers(), 
  A_DrawLine(), A_DrawSeg(), A_DrawPLdist(),
  A_DrawChains(), A_DrawHull(), A_Update(), A_AddSplit(),
  A_UpdateH();

#endif /* ANIMATE */

