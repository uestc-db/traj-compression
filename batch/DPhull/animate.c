 /* 28 Jan 92 				Jack Snoeyink
 * Animation routines for Douglas Peucker line simplification
 */

#include "DP.h"
#include "PH.h"
#include "animate.h"
//#include <gl/gl.h>
//#include <gl/device.h>
//#include <fmclient.h>

#define LINEWIDTH 5
#define POINTSIZE .2
#define MAXPTS 500002

double top, bot;
int A_delay = 0;
int A_swapinterval = 2;
int A_modeq = 0;
int A_ack = 0;
int A_nmodes = 0;

char *A_modes[5];

//fmfonthandle font1;

int n_split;
POINT *splits[MAXPTS];
double aspect;			/* aspect ration x/y */

void A_Clear()
{
 /* mycolor(myBLACK);
  clear();    
  gflush();
*/
}


void A_ClearBack()
{
  /*frontbuffer(FALSE);
  A_Clear();
  */
}

void A_Init(name)
     char *name;
{
 /* 
  foreground();
  winopen(name);
  RGBmode();
  A_Clear();
  doublebuffer(); 
  //mmode(MVIEWING);

  subpixel(TRUE);
  swapinterval(A_swapinterval);
  //shademodel(FLAT);
  gconfig();

  A_ClearBack();

  //qdevice(REDRAW);
  //qdevice(WINQUIT);

  //qdevice(ESCKEY);
  //qdevice(AKEY);

  //qdevice(UPARROWKEY);
  //qdevice(DOWNARROWKEY);

  fminit();
  //font1 = fmfindfont("Helvetica");
  //font1 = fmscalefont(font1, 32.0);
  //fmsetfont(font1);
  */
}

void circle(p, r)
     POINT p;
     double r;
{
  //int i;
  //POINT tmp;
  //static POINT off[8] = {1,0, SQRhf,SQRhf, 0,1, -SQRhf,SQRhf, 
//				-1,0, -SQRhf,-SQRhf, 0,-1, SQRhf,-SQRhf};
 // bgnclosedline();
  //for (i = 0; i < 8; i++)
   // {
     // LINCOMB_2C(1.0, p, r, off[i], tmp);
   //   v2d(tmp);
   // }
 // endclosedline();
}

	
void A_DrawChains()
{
 /* int i;

  linewidth(LINEWIDTH);
  mycolor(myGREEN);
  bgnline();
  for (i = 0; i < n; i++)
    v2d(V[i]);
  endline();
  for (i = 0; i < n; i++)
    circle(V[i], POINTSIZE);

  mycolor(myRED);
  bgnline();
  for (i = 0; i < n_split; i++)
    v2d(*splits[i]);
  endline();
  for (i = 0; i < n_split; i++)
    circle((*splits[i]), POINTSIZE);
  linewidth(4);
  */
}

void A_DrawHull(h)
     PATH_HULL *h;
{
  /*int i,j;

  frontbuffer(FALSE);
  //blendfunction(BF_SA, BF_MSA);

  if (h->bot < h->top)
    {
      bgnqstrip();
      for (i = h->bot + (h->top - h->bot)/2, j = i+1; j <= h->top; i--, j++)
	{
	  v2d(*h->elt[i]);
	  v2d(*h->elt[j]);
	}
      endqstrip();
    }
  //blendfunction(BF_ONE, BF_ZERO);
  */
}

void DoMode(num)
     int num;
{
  //cmov2(aspect*top-12, bot +9 + (top - bot)*(.1 + .05*num));
  //fmprstr(A_modes[num]);
}

void A_AllModes()
{
  /*int i;
  
  mycolor(myGREY);
  for (i = 0; i < A_nmodes; i++) DoMode(i);
  */
}

void A_SwapBuffers()
{
  /*short dev, val;

  if (A_modeq) A_AllModes();*/
 /* if (A_ack)
    {
    do {
      sginap(5);
    } while (!getbutton(SPACEKEY));
    }*/

/*  swapbuffers();*/ /**/
  //frontbuffer(TRUE);
 /* while (qtest())
    {
    switch (dev = qread(&val)) 
      {
      case REDRAW:
	reshapeviewport();
	break;

      case ESCKEY:
      case WINQUIT:
	A_Quit();
	exit(0);
      }

    if (val != 0)
      switch (dev)
	{
	
	case UPARROWKEY:
	  A_delay += 1 + A_delay / 6;
	  break;

	case DOWNARROWKEY:
	  A_delay -= 1 + A_delay / 6;
	  if (A_delay < 0) A_delay = 0;
	  break;

	case AKEY:
	  A_ack = !A_ack;
	  break;
	}
  }*/
}


void A_Update()
{
  A_ClearBack();
  A_DrawChains();
  A_SwapBuffers();
}
     
void A_Setup(nmodes)
     int nmodes;
{				/*  tl-t-tr  */
  double b, t;			/*  l     r  */
  int i;			/*  bl-b-br  */
  
  A_nmodes = nmodes;
  b = t = 0.0;
  for (i = 0; i < n; i++)
    {
      if (t < V[i][YY]) t = V[i][YY];
      if (t < V[i][XX]) t = V[i][XX];
      if (b > V[i][YY]) b = V[i][YY];
      if (b > V[i][XX]) b = V[i][XX];
    }
  top = t + 3.5 + t*0.05; bot = b - 10.5 - b*0.05;
  //ortho2(aspect * bot, aspect*top, top, bot);
  n_split = 2;
  splits[0] = V;
  splits[1] = V+n - 1;
  if (A_ack)
    {
      A_ack = 0;
      A_Update();
      A_ack = 1;
    };
  A_Update();
}


void A_AddSplit(split)
     POINT *split;
{
  int i;

 // linewidth(6);
  //mycolor(myMAGENTA);
  circle(*split, 3 * POINTSIZE);
  //linewidth(4);
  for (i = n_split; splits[i-1] > split; i--)
    splits[i] = splits[i-1];
  splits[i] = split;
  n_split++;
  
}


void A_DrawSeg(p, q)
     POINT p, q;
{
  //bgnline();
  //v2d(p);
  //v2d(q);
 // endline();
 // gflush();
}


void A_DrawLine(l)
     HOMOG l;
{
 /* POINT p, q;
  
  if (fabs(l[XX]) > fabs(l[YY]))
    {
      p[XX] = (-l[WW]+2*n*l[YY]) / l[XX];
      p[YY] = -2*n;
      q[XX] = (-l[WW]-2*n*l[YY]) / l[XX];
      q[YY] = 2*n;
    }
  else
    {
      p[XX] = -2*n;
      p[YY] = (-l[WW]+2*n*l[XX]) / l[YY];
      q[XX] = 2*n;
      q[YY] = (-l[WW]-2*n*l[XX]) / l[YY];
    }
  mycolor(myYELLOW);
  A_DrawSeg(p, q);
  */
}


void A_DrawPLdist(p, l)
     POINT p;
     HOMOG l;
{
  /*int i;
  double dsq, dot;
  POINT q, tmp;

  dsq = DOTPROD_2C(l,l);
  dot = p[XX] * l[YY] - p[YY] * l[XX];
  q[XX] = (-l[WW] * l[XX] + l[YY] * dot) / dsq;
  q[YY] = (-l[WW] * l[YY] - l[XX] * dot) / dsq;
  
  mycolor(myBLUE);
  for (i = 0; i < A_delay; i++)
    {
      dot = (i+1.0) / (A_delay+2.0);
      LINCOMB_2C(dot, p, (1.0 - dot), q, tmp);
      A_DrawSeg(q, tmp);
      gsync();
    }
  A_DrawSeg(q, p);
  mycolor(myGREEN);
  bgnpoint();
  v2d(p);
  endpoint();
  */
}


void A_Mode(num)
     int num;
{
  /*static int lastnum = -1;

  if (A_modeq)
    {
      if (lastnum >= 0)
	{
	  mycolor(myGREY);
	  DoMode(lastnum);
	}
      mycolor(myWHITE);
      DoMode(num);
      lastnum = num;      
      if (A_delay > 10)
	{
	  gflush();
	  sginap((long) A_delay );
	}
    }
  */
}
    
void A_Quit()
{
  //sginap(100L);
  //gexit();
}
