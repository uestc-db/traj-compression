/*  				7-6-91      Jack Snoeyink
				Recursive implementation of the Douglas Peucker line simplification
				algorithm based on path hulls
				*/
#include "DP.h"
#include "PHfast.h"

PATH_HULL left, right;	/* Path Hull: \va{left} hull and \va{right} hull and tag vertex \va{PHtag}. */
POINT *PHtag;


void Build(i, j)		/* Build the Path Hull for the chain from vertex $i$ to vertex $j$.   */
     POINT *i, *j;
{
  register POINT *k;
  register int topflag, botflag;
  
  PHtag = i + (j - i) / 2;	/* Assign tag vertex */
  
  Hull_Init(left, PHtag, PHtag - 1); /* \va{left} hull */
  for (k = PHtag - 2; k >= i; k--)
    {
      topflag = LEFT_OF(left.elt[left.top], left.elt[left.top-1], k);
      botflag = LEFT_OF(left.elt[left.bot+1], left.elt[left.bot], k);
      if (topflag || botflag)
	{
	  while (topflag)
	    {
	      Hull_Pop_Top(left);
	      topflag = LEFT_OF(left.elt[left.top], left.elt[left.top-1], k);
	    }
	  while (botflag)
	    {
	      Hull_Pop_Bot(left);
	      botflag = LEFT_OF(left.elt[left.bot+1], left.elt[left.bot], k);
	    }
	  Hull_Push(left, k);
	}
    }
  Hull_Init(right, PHtag, PHtag + 1); /* \va{right} hull */
  for (k = PHtag + 2; k <= j; k++)
    {
      topflag = LEFT_OF(right.elt[right.top], right.elt[right.top-1], k);
      botflag = LEFT_OF(right.elt[right.bot+1], right.elt[right.bot], k);
      if (topflag || botflag)
	{
	  while (topflag)
	    {
	      Hull_Pop_Top(right);
	      topflag = LEFT_OF(right.elt[right.top], right.elt[right.top-1], k);
	    }
	  while (botflag)
	    {
	      Hull_Pop_Bot(right);
	      botflag = LEFT_OF(right.elt[right.bot+1], right.elt[right.bot], k);
	    }
	  Hull_Push(right, k);
	}
    }
}  

POINT *DP(i, j)
     POINT *i, *j;
{				/* DP */
  static double ld, rd, len_sq;
  static HOMOG l;
  register POINT *le, *re;
  POINT *tmp;
  
  CROSSPROD_2CCH(*i, *j, l);
  len_sq = l[XX] * l[XX] + l[YY] * l[YY];
  
  if (j - i < 8)
    {		/* chain small */
      rd  = 0.0;
      for (le = i + 1; le < j; le++)
	{
	  ld = DOTPROD_2CH(*le, l);
	  if (ld < 0) ld = - ld;
	  if (ld > rd) 
	    {
	      rd = ld;
	      re = le;
	    }
	}
      if (rd * rd > EPSILON_SQ * len_sq)
	{
	  OutputVertex(DP(i, re)); 
	  return(DP(re, j));
	}
      else
	return(j);
    }
  else
    {				/* chain large */
      register int 
	sbase, sbrk, mid,
	lo, m1, brk, m2, hi;
      double d1, d2;
      if ((left.top - left.bot) > 8) 
	{			/* left hull large */
	  lo = left.bot; hi = left.top - 1;
	  sbase = SLOPE_SIGN(left, hi, lo, l);
	  do
	    {
	      brk = (lo + hi) / 2;
	      if (sbase == (sbrk = SLOPE_SIGN(left, brk, brk+1, l)))
		if (sbase == (SLOPE_SIGN(left, lo, brk+1, l)))
		  lo = brk + 1;
		else
		  hi = brk;
	    }
	  while (sbase == sbrk);
	  
	  m1 = brk;
	  while (lo < m1)
	    {
	      mid = (lo + m1) / 2;
	      if (sbase == (SLOPE_SIGN(left, mid, mid+1, l)))
		lo = mid + 1;
	      else
		m1 = mid;
	    }
	  
	  m2 = brk;
	  while (m2 < hi) 
	    {
	      mid = (m2 + hi) / 2;
	      if (sbase == (SLOPE_SIGN(left, mid, mid+1, l)))
		hi = mid;
	      else
		m2 = mid + 1;
	    };
	  
	  /*      printf("Extremes: <%3lf %3lf>  <%3lf %3lf>\n", 
		  (*left.elt[lo])[XX],  (*left.elt[lo])[YY],
		  (*left.elt[m2])[XX],  (*left.elt[m2])[YY]); /**/
	  
	  if ((d1 = DOTPROD_2CH(*left.elt[lo], l)) < 0) d1 = - d1;
	  if ((d2 = DOTPROD_2CH(*left.elt[m2], l)) < 0) d2 = - d2;
	  ld = (d1 > d2 ? (le = left.elt[lo], d1) : (le = left.elt[m2], d2));
	}
      else
	{			/* Few points in left hull */
	  ld = 0.0;
	  for (mid = left.bot; mid < left.top; mid++)
	    {
	      if ((d1 = DOTPROD_2CH(*left.elt[mid], l)) < 0) d1 = - d1;
	      if (d1 > ld)
		{
		  ld = d1;
		  le = left.elt[mid];
		}
	    }
	}
      
      if ((right.top - right.bot) > 8)
	{			/* right hull large */
	  lo = right.bot; hi = right.top - 1;
	  sbase = SLOPE_SIGN(right, hi, lo, l);
	  do
	    {
	      brk = (lo + hi) / 2;
	      if (sbase == (sbrk = SLOPE_SIGN(right, brk, brk+1, l)))
		if (sbase == (SLOPE_SIGN(right, lo, brk+1, l)))
		  lo = brk + 1;
		else
		  hi = brk;
	    }
	  while (sbase == sbrk);
	  
	  m1 = brk;
	  while (lo < m1)
	    {
	      mid = (lo + m1) / 2;
	      if (sbase == (SLOPE_SIGN(right, mid, mid+1, l)))
		lo = mid + 1;
	      else
		m1 = mid;
	    }
	  
	  m2 = brk;
	  while (m2 < hi) 
	    {
	      mid = (m2 + hi) / 2;
	      if (sbase == (SLOPE_SIGN(right, mid, mid+1, l)))
		hi = mid;
	      else
		m2 = mid + 1;
	    };
	  
	  /*      printf("Extremes: <%3lf %3lf>  <%3lf %3lf>\n", 
		  (*right.elt[lo])[XX],  (*right.elt[lo])[YY],
		  (*right.elt[m2])[XX],  (*right.elt[m2])[YY]); /**/
	  
	  if ((d1 = DOTPROD_2CH(*right.elt[lo], l)) < 0) d1 = - d1;
	  if ((d2 = DOTPROD_2CH(*right.elt[m2], l)) < 0) d2 = - d2;
	  rd = (d1 > d2 ? (re = right.elt[lo], d1) : (re = right.elt[m2], d2));
	}
      else
	{			/* Few points in righthull */
	  rd = 0.0;
	  for (mid = right.bot; mid < right.top; mid++)
	    {
	      if ((d1 = DOTPROD_2CH(*right.elt[mid], l)) < 0) d1 = - d1;
	      if (d1 > rd)
		{
		  rd = d1;
		  re = right.elt[mid];
		}
	    }
	}
    }

  
  if (ld > rd)
    if (ld * ld > EPSILON_SQ * len_sq)
      {				/* split left */
	register int tmpo; 
	
	while ((left.hp >= 0) 
	       && ((tmpo = left.op[left.hp]), 
		   ((re = left.helt[left.hp]) != le) || (tmpo != PUSH_OP)))
	  {
	    left.hp--;
	    switch (tmpo)
	      {
	      case PUSH_OP:
		left.top--;
		left.bot++;
		break;
	      case TOP_OP:
		left.elt[++left.top] = re;
		break;
	      case BOT_OP:
		left.elt[--left.bot] = re;
		break;
	      }
	  }
	
	
	/*	    printf("LEFT Backup "); Hull_Print(&left);/**/
	tmp = DP(le, j);
	Build(i, le);
	OutputVertex(DP(i, le));
	return(tmp);
      }
    else
      return(j);
  else				/* extreme on right */
    if (rd * rd > EPSILON_SQ * len_sq)
      {				/* split right or both */
	if (PHtag == re)
	  Build(i, re);
	else
	  {			/* split right */
	    register int tmpo;
	    
	    while ((right.hp >= 0) 
		   && ((tmpo = right.op[right.hp]), 
		       ((le = right.helt[right.hp]) != re) || (tmpo != PUSH_OP)))
	      {
		right.hp--;
		switch (tmpo)
		  {
		  case PUSH_OP:
		    right.top--;
		    right.bot++;
		    break;
		  case TOP_OP:
		    right.elt[++right.top] = le;
		    break;
		  case BOT_OP:
		    right.elt[--right.bot] = le;
		    break;
		  }
	      }
	  }
	/*	    printf("RIGHT Backup "); Hull_Print(&right);/**/
	OutputVertex(DP(i, re));
	Build(re, j);
	return(DP(re, j));
      }
    else
      return(j);
}



void main(argc,argv)
     int argc;
     char **argv;
{
  //register int i;
  
 // Parse(argc, argv);
  //Init("DPhull");
  
  /*  left = (PATH_HULL *) malloc(sizeof(PATH_HULL));
      right = (PATH_HULL *) malloc(sizeof(PATH_HULL));/**/
  
  //do
    {
      //Get_Points();
    //  Start_Timing();
      
      //for (i=0; i < 100; i++) {
	//outFlag = TRUE;
	//num_result = 0;
	//Build(V, V + n - 1);	/* Build the initial path hull */
	//OutputVertex(V);
	//OutputVertex(DP(V, V + n - 1)); /* Simplify */
     // }
     // 
      //End_Timing(2);
      //Print_Result(FALSE); /**/
   // }
  //while (looping);
}



