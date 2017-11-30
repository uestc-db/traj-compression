/*
 * 7-6-91      Jack Snoeyink procedures for testing/timing line simplification
 */

#include "DP.h"
//#include "animate.h"
#include "stdlib.h"
#include "stdio.h"
#include <sys/types.h>
#include <sys/times.h>

#define LAST 9

POINT *V, **R;

float  p[MAX_POINTS][4];

int n, num_result;
int outFlag, looping = 0;

//double EPSILON = 0.00001,		/* error tolerance */
//  EPSILON_SQ = 0.000000001;
double EPSILON ,EPSILON_SQ;

static int  cycler, CycleFlag = FALSE;

static int file_active = 0;

FILE *infp;
#define REC_LENGTH 144
char buffer[145];

POINT *Alloc_Points(int n)
{ 	
 // void *calloc();

  return ((POINT *) calloc(n, sizeof(POINT)));
}

void Print_Points(POINT **P, int n, int flag)
{
  int i;

  printf("%d points\n", n);
  if (flag)
    for (i = 0; i < n; i++)
      printf("%d: %.4f %.4f \n", i, (*(P[i]))[XX], (*(P[i]))[YY]);	/**/
  fflush(stdout);
}

void Print_Result(int flag)
{
    Print_Points(R, num_result, flag);
}


#define TWO_PI 6.28318531


void fatalError(char *msg, char *var)
{
  fprintf(stderr, msg, *var);
  exit(1);
}

void ReadLine(char* filename)
{
	
	
	int i;
	//infp = fopen("C:\\Users\\Ding\\Desktop\\¿Î³Ì\\1\\DPhull\\3.txt", "r");
        //printf("%s\n",filename);
	infp = fopen(filename, "r");
	if (infp == NULL)
	{
		printf("file open error\n");
		exit(0);
	}
	//fread(p[0], sizeof(float), 2*n, infp);
	double *c = (double *)malloc(3*n * sizeof(double));
	for (i = 0; i<3* n; i++)
	{
		fscanf(infp, "%lf", &c[i]);
	}
	int j = 0;
	for (i = 0; i < 3 * n; j++) {
		     V[j][XX] = c[i];
		     V[j][YY] = c[i+1];
		     V[j][TT] = c[i+2];
		     i = i + 3;
	}
        
	//for (i = 0; i < n;i++) {
	//	printf("%lf %lf %lf\n", V[i][XX], V[i][YY],V[i][TT]);
	//}
        int index = 0;
	int k = 1;
	for (k = 1 ; k < n ; k++)
	{
                if (V[index][XX] != V[k][XX] || V[index][YY] != V[k][YY])
			{
				index = index + 1;
				V[index][XX] = V[k][XX];
				V[index][YY] = V[k][YY];
				
			}
        
        }
	n = index;
	//printf("n = %d\n",n);
        while ((V[0][XX] == V[n-1][XX]) && (V[0][YY] == V[n-1][YY])) n--;
	fclose(infp);
	//infp = NULL;
}

void Parse(int argc, char **argv)
{
  double LEFT, RIGHT, BOTTOM, TOP;
  --argc; argv++;

  n = 50;
  cycler = 0;
  file_active = 0;

  while ((argc > 0) && (argv[0][0] == '-'))
      {
	if (!strcmp(argv[0], "-n")) {
	    if (sscanf(argv[1], "%d", &n) != 1) 
	      fatalError("ERROR -- \"%d\": bad n\n", (char *)(argv[1]));
	    argv += 2;
	    argc -= 2;
	}
	else if (!strcmp(argv[0], "-k")) {
	    if (sscanf(argv[1], "%d", &cycler) != 1) 
	      fatalError("ERROR -- \"%d\": bad kind of test case\n-k Test Case:    0/all\n   1/circle, 2/prtb circ, 3/mon random, 4/non-mon random,\n   5/ prtb zig-zag, 6/cnvx zig-zag, 7/cncv zig-zag, 8/spiral\n", (char *)(argv[1]));
	    argv += 2;
	    argc -= 2;
	  }
	else if (!strcmp(argv[0], "-e")) {
	    if (sscanf(argv[1], "%lf", &EPSILON) != 1)
	      fatalError("ERROR -- \"%lf\": bad epsilon\n", (char *)(argv[1]));
	    EPSILON_SQ = EPSILON * EPSILON;
	    argv += 2;
	    argc -= 2;
	  }
	else if (!strcmp(argv[0], "-f")) {
	  infp = fopen(argv[1], "r");
	  if (infp == NULL) 
	    fatalError("Could not open input file %s\n", (char *)(argv[1]));
	  fgets(buffer, REC_LENGTH, infp);
	  if (strncmp("my format", buffer, 9) != 0)
	    fatalError("File not in my format: %s\n", buffer);
	  sscanf(buffer, "my format %lg%lg%lg%lg",&LEFT, &RIGHT, &BOTTOM, &TOP);
	  file_active = 1;
	  argv += 2;
	  argc -= 2;
	}
	else if (!strcmp(argv[0], "-l")) {
	    if (sscanf(argv[1], "%d", &looping) != 1) 
	      fatalError("ERROR -- \"%d\": bad l\n", (char *)(argv[1]));
	    argv += 2;
	    argc -= 2;
	  }
	else if (!strcmp(argv[0], "-h")) {
	    argv++;
	    --argc;
	    fprintf(stderr, 
"usage: basic -n # no. pts  -k # test case  -l loop# -f file of test cases\n -e epsilon/n"
);
	    exit(1);
	}
      }
}

#define SIGN(x) (x < 0 ? -1 : (x > 0 ? 1 : 0))

#define DET2(a, b, c, d)	/* 2x2 determinant */\
  ((a)*(d) - (c)*(b)) 
#define DETPts(p, q)		/* 2x2 determinant for points */\
  DET2(p[XX], p[YY], q[XX], q[YY])

int intersect(POINT a, POINT b, POINT c, POINT d)	/* check if (ab) \cap (cd) */
{
  register double ab = DETPts(a, b);
  register double ac = DETPts(a, c);
  register double ad = DETPts(a, d);
  register double bc = DETPts(b, c);
  register double bd = DETPts(b, d);
  register double cd = DETPts(c, d);
  register double abc = bc - ac + ab;
  register double abd = bd - ad + ab;
  register double acd = cd - ad + ac;
  register double bcd = cd - bd + bc;

  return ((SIGN(abc)*SIGN(abd) == -1) && (SIGN(acd)*SIGN(bcd) == -1));
}


void swapV(int i, int j)
{
  double tmp;

  tmp = V[i][XX]; V[i][XX] = V[j][XX]; V[j][XX] = tmp;
  tmp = V[i][YY]; V[i][YY] = V[j][YY]; V[j][YY] = tmp;
}


void untangle()
{
  /*int i, j, 
  oi, oj, 
  ti, tj, 
  k, l, flag, tmpflag;
  
  do
    {
      oi = rand(n);
      flag = 0;
      for (i = 0; i < n; i++)
	{
	  ti = (oi + i) % n;
	  oj = rand(n);
	  for (j = 0; j < n; j++)
	    {
	      tj = (oj + j) % n;
	      if ((ti + n + 1 - tj) % n > 2)
		if (intersect(V[ti], V[(ti+1)%n], V[tj], V[(tj+1)%n]))
		  {
		    flag = TRUE;
		    if (tj < ti + 1) tj = tj + n;
		    for (k = ti + 1, l = tj; k < l; k++, l--)
		      swapV(k % n, l % n);
		    break;
		  }
	    }
	}
    }
  while (flag);
*/
}				/* working */



void Get_Points(char* filename)
{
    ReadLine(filename);
/*  Print_Points(V, n, TRUE); /**/
}




struct tms      starttime, endtime;

void Start_Timing()
{
  times(&starttime);
}

void  End_Timing()
{
  times(&endtime);
  printf("compression ratio: %lf\n", n*1.0 / n_split);
  printf("compression time : %lf sec\n", (double) (endtime.tms_utime - starttime.tms_utime) / 100.0);
}

void Output(int i, int j)
{
  if (outFlag)
    {
      outFlag = FALSE;
      OutputVertex(V+i);
    }
      OutputVertex(V+j);
}


void Init(char *name)
{
  //srand48((long) 123);

  V = Alloc_Points(MAX_POINTS);
  R = (POINT **) calloc(MAX_POINTS, sizeof(POINT *));

}  


