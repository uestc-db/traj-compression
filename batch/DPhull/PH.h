/*  				7-6-91      Jack Snoeyink
Declarations for path hulls
*/

#pragma once

#define HULL_MAX 10002
#define TWICE_HULL_MAX 20002
#define THRICE_HULL_MAX 30002

#define PUSH_OP 0		/* Operation names saved in history stack */
#define TOP_OP 1
#define BOT_OP 2

typedef struct {		/* Half of a Path Hull: \va{elt} is a double ended queue storing a convex hull, \va{top} and \va{bot} are the two ends.  The history stack is \va{helt} for points and \va{op} for operations, \va{hp} is the stack pointer. */
  int top, bot, 
  hp, op[THRICE_HULL_MAX];
  POINT *elt[TWICE_HULL_MAX], *helt[THRICE_HULL_MAX];
} PATH_HULL;


extern PATH_HULL *left, *right;	
extern double top,bot;

#define Hull_Push(h, e)		/* Push element $e$ onto path hull $h$ */\
  (h)->elt[++(h)->top] = (h)->elt[--(h)->bot] = (h)->helt[++(h)->hp] = e;\
  (h)->op[(h)->hp] = PUSH_OP
#define Hull_Pop_Top(h)		/* Pop from top */\
  (h)->helt[++(h)->hp] = (h)->elt[(h)->top--];\
  (h)->op[(h)->hp] = TOP_OP
#define Hull_Pop_Bot(h)		/* Pop from bottom */\
  (h)->helt[++(h)->hp] = (h)->elt[(h)->bot++];\
  (h)->op[(h)->hp] = BOT_OP
#define Hull_Init(h, e1, e2)	/* Initialize path hull and history  */\
  (h)->elt[HULL_MAX] = e1;\
  (h)->elt[(h)->top = HULL_MAX + 1] = \
  (h)->elt[(h)->bot = HULL_MAX - 1] = \
  (h)->helt[(h)->hp = 0] = e2;\
  (h)->op[0] = PUSH_OP;

#define LEFT_OF(a, b, c)	/* Determine if point c is left of line a to b */\
     (((*a)[XX] - (*c)[XX])*((*b)[YY] - (*c)[YY]) \
      >= ((*b)[XX] - (*c)[XX])*((*a)[YY] - (*c)[YY]))

#define SGN(a) (a >= 0)

//void Hull_Add(), Split(), Hull_Print(), Find_Extreme();
void Split(register PATH_HULL *h, POINT *e);
void Find_Extreme(register PATH_HULL *h, HOMOG line, POINT **e, register double *dist,POINT *i,POINT *j);
void Hull_Add(register PATH_HULL *h, POINT *p);
void Hull_Print(PATH_HULL *h);

