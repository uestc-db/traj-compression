/*  				7-6-91      Jack Snoeyink
Declarations for the Douglas Peucker line simplification algorithm.
*/
#pragma once

#include <stdio.h>
#include <math.h>

#define FALSE 0
#define TRUE 1

#define MAX_POINTS 1001
#define TWICE_MAX_POINTS 2002
#define MAXPTS 500002

typedef double POINT[3];	/* Most data is cartesian points */
typedef double HOMOG[3];	/* Some partial calculations are homogeneous */

int n_split;
POINT *splits[MAXPTS];

#define XX 0
#define YY 1
#define WW 2
#define TT 2

#define CROSSPROD_2CCH(p, q, r) /* 2-d cartesian to homog cross product */\
 (r)[WW] = (p)[XX] * (q)[YY] - (p)[YY] * (q)[XX];\
 (r)[XX] = - (q)[YY] + (p)[YY];\
 (r)[YY] =   (q)[XX] - (p)[XX];

#define DOTPROD_2CH(p, q)	/* 2-d cartesian to homog dot product */\
 (q)[WW] + (p)[XX]*(q)[XX] + (p)[YY]*(q)[YY]


#define DOTPROD_2C(p, q)	/* 2-d cartesian  dot product */\
 (p)[XX]*(q)[XX] + (p)[YY]*(q)[YY]

#define LINCOMB_2C(a, p, b, q, r) /* 2-d cartesian linear combination */\
 (r)[XX] = (a) * (p)[XX] + (b) * (q)[XX];\
 (r)[YY] = (a) * (p)[YY] + (b) * (q)[YY];


#define MIN(a,b) ( a < b ? a : b)
#define MAX(a,b) ( a > b ? a : b)

#define OutputVertex(v) R[num_result++] = v;

extern POINT *V,		/* V is the array of input points */
   **R;				/* R is the array of output pointers to V */

extern int n,			/* number of elements in V */
  num_result,			/* number of elements in R */
  outFlag, looping;

extern double EPSILON,		/* error tolerance */
  EPSILON_SQ;			/* error tolerance squared */


void Print_Points();
POINT *Alloc_Points();		/* alloc memory */
void Get_Points(char* filename);		/* create test cases */
double Distance();
//void Init(), Output(), Print_Result(), Start_Timing(), End_Timing();
void Parse(int argc, char **argv);
void swapV(int i, int j);
int intersect(POINT a, POINT b, POINT c, POINT d);
void fatalError(char *msg, char *var);
void Print_Points(POINT **P, int n, int flag);
POINT *Alloc_Points(int n);
void Init(char *name);
void Output(int i, int j);
void AddSplit(POINT *split);
void Print_Result(int flag);

