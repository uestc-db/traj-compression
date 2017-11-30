/*  				1-26-94      Jack Snoeyink
Non-recursive implementation of the Douglas Peucker line simplification
algorithm.
*/
#include "DP.h"
#include "animate.h"

/* Assumes that the polygonal line is in a global array V. 
 * main() assumes also that a global variable n contains the number of
 * points in V.
 */

int stack[MAX_POINTS];		/* recursion stack */
int sp;				/* recursion stack pointer */

#define Stack_Push(e)		/* push element onto stack */\
  stack[++sp] = e
#define Stack_Pop()		/* pop element from stack (zero if none) */\
  stack[sp--]
#define Stack_Top()		/* top element on stack  */\
  stack[sp]
#define Stack_EmptyQ()		/* Is stack empty? */\
  (sp < 0)
#define Stack_Init()		/* initialize stack */\
  sp = -1


void Find_Split(i, j, split, dist) /* linear search for farthest point */
     int i, j, *split;		   /* from the segment Vi to Vj. returns */
     double *dist;	 	   /* squared distance and a pointer */
{
  int k;
  HOMOG q;
  double tmp;

#ifdef ANIMATE
  HOMOG l;

  CROSSPROD_2CCH(V[i], V[j], l);
  A_DrawLine(l);
#endif
  *dist = -1;
  if (i + 1 < j)
    {
      CROSSPROD_2CCH(V[i], V[j], q); /* out of loop portion */ 
				     /* of distance computation */
      for (k = i + 1; k < j; k++)
	{
	  tmp = DOTPROD_2CH(V[k], q); /* distance computation */
	  if (tmp < 0) tmp = - tmp; /* calling fabs() slows us down */
#ifdef ANIMATE
	  A_DrawPLdist(V[k], l);
#endif
	  if (tmp > *dist) 
	    {
	      *dist = tmp;	/* record the maximum */
	      *split = k;
	    }
	}
      *dist *= *dist/(q[XX]*q[XX] + q[YY]*q[YY]); /* correction for segment */
    }				   /* length---should be redone if can == 0 */
}



void DPbasic(i,j)		/* Basic DP line simplification */
     int i, j;
{
  int split; 
  double dist_sq;
  
#ifdef ANIMATE
  A_Mode(0);
#endif
  Stack_Init();
  Stack_Push(j);
  do
    {
      Find_Split(i, Stack_Top(), &split, &dist_sq);
      if (dist_sq > EPSILON_SQ)
	{
#ifdef ANIMATE
	  A_AddSplit(V+split);
#endif
	  Stack_Push(split);
	}
      else
	{
	  Output(i, Stack_Top()); /* output segment Vi to Vtop */
	  i = Stack_Pop();
	}
#ifdef ANIMATE
      A_Update();
#endif
    }
  while (!Stack_EmptyQ());
}




main(argc,argv)
    int argc;
    char **argv;
{

//#ifndef ANIMATE
 // register int i;
//#endif /* not ANIMATE */

  //Parse(argc, argv);
  //Init("DPfast");

  //do
   // {
      //puts("111111111111111");
    //  Get_Points();
//#ifdef ANIMATE
      //A_modes[0] = "FindExtr.";
     // A_modes[1] = "Split";
     // A_Setup(2);
//#endif 
      //double tstart, tend, tcost,ratio;
      //tstart=clock();
      // Start_Timing();
      
//#ifndef ANIMATE
      //for (i=0; i < 100; i++)	/* For timing purposes */
//#endif /* not ANIMATE */
	//{
	 // outFlag = TRUE;
	  //num_result = 0;
         // puts("before");
	 // DPbasic(0, n - 1);
	//}
         // puts("end");
      //End_Timing(1);
      //tend=clock();
     // tcost=(double)(tend-tstart)/100.0;
      //ratio=num_result*1.0/n;
      //printf("compression time =%lf s\n",tcost);
     //Print_Result(FALSE);
    //}
  //while (looping);
//#ifdef ANIMATE
  //A_Quit();
//#endif 

}
