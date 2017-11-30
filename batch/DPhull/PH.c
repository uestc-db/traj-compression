/*  				7-6-91      Jack Snoeyink
Path hulls for the Douglas Peucker line simplification algorithm.
*/
#include "DP.h"
#include "PH.h"
#include <math.h>
void Hull_Print(PATH_HULL *h)
{
  register int i;

  printf(" hull has %d points: ", h->top - h->bot);
  for (i = h->bot; i <= h->top; i++)
    printf(" <%.3lf %.3lf> ", (*h->elt[i])[XX], (*h->elt[i])[YY]);/**/
  printf("\n");
}


void Hull_Add(register PATH_HULL *h, POINT *p)		/* Add $p$ to the path hull $h$. Implements Melkman's convex hull algorithm. */
{
  register int topflag, botflag;
  topflag = LEFT_OF(h->elt[h->top], h->elt[h->top-1], p);
  botflag = LEFT_OF(h->elt[h->bot+1], h->elt[h->bot], p);

  if (topflag || botflag)
    {
      while (topflag)
	{
	  Hull_Pop_Top(h);
	  if (h->top -1 < h->bot)
		break;
	  topflag = LEFT_OF(h->elt[h->top], h->elt[h->top-1], p);
	}
      while (botflag)
	{
	  Hull_Pop_Bot(h);
	  if (h->bot +1 > h->top)
		break;
	  botflag = LEFT_OF(h->elt[h->bot+1], h->elt[h->bot], p);
	}
      Hull_Push(h, p);
    }
}


void Split(register PATH_HULL *h ,POINT *e)
{
  register POINT *tmpe;
  register int tmpo;

  while ((h->hp >= 0) 
	 && ((tmpo = h->op[h->hp]), 
	     ((tmpe = h->helt[h->hp]) != e) || (tmpo != PUSH_OP)))
    {
      h->hp--;
      switch (tmpo)
	{
	case PUSH_OP:
	  h->top--;
	  h->bot++;
	  break;
	case TOP_OP:
	  h->elt[++h->top] = tmpe;
	  break;
	case BOT_OP:
	  h->elt[--h->bot] = tmpe;
	  break;
	}
    }
}


#define SLOPE_SIGN(h, p, q, l)	/* Return the sign of the projection 
				   of $h[q] - h[p]$ onto the normal 
				   to line $l$ */ \
  SGN((l[XX])*((*h->elt[q])[XX] - (*h->elt[p])[XX]) \
      + (l[YY])*((*h->elt[q])[YY] - (*h->elt[p])[YY])) 


void Find_Extreme(register PATH_HULL *h, HOMOG line, POINT **e, register double *dist,POINT *i,POINT *j)
{
  register int 
    sbase, sbrk, mid,
    lo, m1, brk, m2, hi;
  double d1, d2;
  double xs = (*i)[XX],ys=(*i)[YY];
  double xe = (*j)[XX],ye=(*j)[YY];


  if ((h->top - h->bot) > 8) 
    {
      lo = h->bot; hi = h->top;
      sbase = SLOPE_SIGN(h, hi, lo, line);
      do
	{
	  brk = (lo + hi) / 2;

	  if (sbase == (sbrk = SLOPE_SIGN(h, brk, brk+1, line)))
	    if (sbase == (SLOPE_SIGN(h, lo, brk+1, line)))
	      lo = brk + 1;
	    else
	      hi = brk;
	}
      while (sbase == sbrk);
      
      m1 = brk;
      while (lo < m1)
	{
	  mid = (lo + m1) / 2;

	  if (sbase == (SLOPE_SIGN(h, mid, mid+1, line)))
	    lo = mid + 1;
	  else
	    m1 = mid;
	}
      
      m2 = brk;
      while (m2 < hi) 
	{
	  mid = (m2 + hi) / 2;

	  if (sbase == (SLOPE_SIGN(h, mid, mid+1, line)))
	    hi = mid;
	  else
	    m2 = mid + 1;
	}
      
      /*printf("Extremes: <%3lf %3lf>  <%3lf %3lf>\n", 
	     (*h->elt[lo])[XX],  (*h->elt[lo])[YY],
	     (*h->elt[m2])[XX],  (*h->elt[m2])[YY]); /**/
      //abs((ye-ys)*xb-(xe-xs)*yb+xe*ys-xs*ye)/np.sqrt((ye-ys)**2+(xe-xs)**2)      
      double x1 = (*h->elt[lo])[XX],y1=(*h->elt[lo])[YY];
      double x2 = (*h->elt[m2])[XX],y2=(*h->elt[m2])[YY];
      d1 = fabs((ye-ys)*x1-(xe-xs)*y1+xe*ys-xs*ye)/sqrt((ye-ys)*(ye-ys)+(xe-xs)*(xe-xs));
      d2 = fabs((ye-ys)*x2-(xe-xs)*y2+xe*ys-xs*ye)/sqrt((ye-ys)*(ye-ys)+(xe-xs)*(xe-xs));  
      
      //if ((d1 = DOTPROD_2CH(*h->elt[lo], line)) < 0) d1 = - d1;
      //if ((d2 = DOTPROD_2CH(*h->elt[m2], line)) < 0) d2 = - d2;
      *dist = (d1 >= d2 ? (*e = h->elt[lo], d1) : (*e = h->elt[m2], d2));
      //printf("dist = %lf\n",*dist);
    }
  else				/* Few points in hull */
    {
      *dist = 0.0;
      for (mid = h->bot; mid <= h->top; mid++)
	{

	  //if ((d1 = DOTPROD_2CH(*h->elt[mid], line)) < 0) d1 = - d1;
          double x1 = (*h->elt[mid])[XX],y1=(*h->elt[mid])[YY];
	  d1 = fabs((ye-ys)*x1-(xe-xs)*y1+xe*ys-xs*ye)/sqrt((ye-ys)*(ye-ys)+(xe-xs)*(xe-xs));
	  if (d1 > *dist)
	    {
	      *dist = d1;
	      *e = h->elt[mid];
	    }
	}
      //printf("dist = %lf\n",*dist);
    }
}	
  


