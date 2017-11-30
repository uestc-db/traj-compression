/*  				7-6-91      Jack Snoeyink
Recursive implementation of the Douglas Peucker line simplification
algorithm based on path hulls
*/
#include "DP.h"
#include "PH.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/time.h"
PATH_HULL *left, *right;	/* Path Hull: \va{left} and \va{right} portions and tag vertex \va{PHtag}. */
POINT *PHtag;
char *filename; 
double top,bot; 
void AddSplit(POINT *split)
{
  int i;
  for (i = n_split; splits[i-1] > split; i--)
         splits[i] = splits[i-1];
  splits[i] = split;
  n_split++;
  //splits[n_split++] = split;
}
void Build(POINT *i, POINT *j)		/* Build Path Hull for the chain from vertex $i$ to vertex $j$.   */
{
  register POINT *k;

 // A_Mode(0);

  PHtag = i + (j - i) / 2;

  Hull_Init(left, PHtag, PHtag - 1);
  for (k = PHtag - 2; k >= i; k--)
    Hull_Add(left, k);
  //printf("LEFT "); Hull_Print(left);

  Hull_Init(right, PHtag, PHtag + 1);
  for (k = PHtag + 2; k <= j; k++)
    Hull_Add(right, k);
/*  printf("RIGHT "); Hull_Print(right);/**/
}
  
void DP(POINT *i, POINT *j)
{
  static double ld, rd, len_sq;
  static HOMOG l;
  POINT *le, *re;
  if (j - i > 1)
    {
      CROSSPROD_2CCH(*i, *j, l);
      //len_sq = DOTPROD_2C(l,l);
      Find_Extreme(left, l, &le, &ld,i,j);
      Find_Extreme(right, l, &re, &rd,i,j);
     if (ld < rd)
	{
	  if (rd  > EPSILON)
	    {
	      if(PHtag == re)
		   Build(i,re);
	      else
	      	   Split(right, re);
	      /*	    printf("RIGHT Backup "); Hull_Print(right);/**/
	      AddSplit(re);
	      DP(i, re);
	      Build(re, j);
	      DP(re, j);
	    }
	}
      else
	if (ld  > EPSILON)
	  {
	    //AddSplit(le);
	    Split(left, le);
	    AddSplit(le);
/*	    printf("LEFT Backup "); Hull_Print(left);/**/
	    DP(le, j);
	    Build(i, le);
	    DP(i, le);
	  }
    }
}




int main(int argc, char** args)
{
	if (argc < 3)
	{
		printf("<infile> <lines><EPSILON><result_filename>\n");
	}
	else
	{
		sscanf(args[2], "%d",&n);
                sscanf(args[3],"%lf",&EPSILON);
                EPSILON_SQ = EPSILON * EPSILON;
		//EPSILON_SQ = EPSILON;
		filename= args[1];
		char* result_filename = args[4];
                FILE* s_fp= fopen(result_filename,"w+");
                if(s_fp == NULL)
                     {
                        printf("open result_file error!\n");
                        exit(0);
                     }
		Init("DPhull");
		double b, t;                  /*  l     r  */
		int i;                        /*  bl-b-br  */
		b = t = 0.0;
 		for (i = 0; i < n; i++)
    		{
      			if (t < V[i][YY]) t = V[i][YY];
      			if (t < V[i][XX]) t = V[i][XX];
      			if (b > V[i][YY]) b = V[i][YY];
      			if (b > V[i][XX]) b = V[i][XX];
    		}
  		top = t + 3.5 + t*0.05; bot = b - 10.5 - b*0.05;
                n_split = 2;
  		splits[0] = V;
  		splits[1] = V+n - 1;
		left = (PATH_HULL *)malloc(sizeof(PATH_HULL));
		right = (PATH_HULL *)malloc(sizeof(PATH_HULL));
		if (left == NULL || right == NULL)
		{
			printf("malloc error\n");
			exit(0);
		}
		Get_Points(filename);
                struct timeval start,end;  
                gettimeofday(&start, NULL ); 
                //Start_Timing();
		Build(V, V + n - 1);
		DP(V, V + n - 1);
                gettimeofday(&end, NULL );  
                long timeuse =1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;  
                //printf("Alogrithm : DPhull\n");
                //printf("Compression time: %f sec \n",timeuse /1000000.0);
                //printf("Compression ratio: %f \n",n*1.0/n_split);
                for (i = 0; i < n_split; i++)
                   {
                          fprintf(s_fp,"%lf %lf %lf\n",(*splits[i])[0],(*splits[i])[1],(*splits[i])[2]);   
                          //printf("%lf,%lf\n",(*splits[i])[0],(*splits[i])[1]);
                    }
                 fprintf(s_fp,"%lf\n",timeuse /1000000.0);
                 fclose(s_fp);
                //End_Timing();
                
	}
  
  return 0;
}


