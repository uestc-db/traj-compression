#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include "sys/time.h"
#include <iostream>
using namespace std;

#ifdef __unix__
#include <unistd.h>
#define __my_inline inline
#define mySnprintf snprintf
#define myHypot hypot
#elif __WIN32__ || _MSC_VER
#include <windows.h>
#define __my_inline __forceinline
#define mySnprintf sprintf_s
#define myHypot _hypot
#endif



#define MAX_ORIGINAL_SIZE  1000
#define MAX_GRAPH_NEIGHBORS_OTS 1000
#define MAX_GRAPH_NEIGHBORS_RTS 1000
#define REPETITIONS 1
// Resulting maximum memory allocation in function runLS is m(o,n):=2*3*8*o+3*4*o+2*o*n, where o is MAX_ORIGINAL_SIZE, and n is MAX_GRAPH_NEIGHBORS_OTS.
// Resulting maximum memory allocation in function runGRTSmcOpt is m(o,n,r):=2*3*8*o+8*o*r+(4+3+3)*8*o+3*4*o+(2+8)*o*n, where o is MAX_ORIGINAL_SIZE, n is MAX_GRAPH_NEIGHBORS_RTS, and r is REPETITIONS.
// The above formulas for the memory allocation can be directly pasted into Maxima.

#define REPETITION_SLEEP_MILLIS 0

#define BYTES_HEADER 0
#define BYTES_STPOINT 24
#define BYTES_INT 4
#define BYTES_BOOL 1
#define BYTES_DOUBLE 8
#define BYTES_VECTOR 16

#define MAX_STRING_LENGTH 250

#define bool int
#define TRUE 1
#define FALSE 0

#define PHYS_MAX_SPEED_CONSTRAINT 1
#define PHYS_MAX_ACCEL_CONSTRAINT 2



void runLSOpt();

void runLSDP();
void runLSDP_recursive(int l, int r);

void runOPWTR();

void runLDRHalf(bool optCond);
__my_inline void runLDRHalf_processUpdate(double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY);
__my_inline void runLDRHalf_processFinalUpdate(double currentT, double currentX, double currentY);

void runCDR(bool useHeap);
void runCDRm(int m);
__my_inline void runCDR_processUpdate(double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double xPY);
__my_inline void runCDR_processFinalUpdate(double currentT, double currentX, double currentY);
__my_inline void runCDR_historyPop(double* historyT, double* historyX, double* historyY, int* historySize, double currentT, double currentX, double currentY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY);
__my_inline void runCDR_historyPush(double* historyT, double* historyX, double* historyY, int* historySize, double currentT, double currentX, double currentY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY);
__my_inline double runCDR_phi(double siT, double siX, double siY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY);

void runGRTSmSec(bool optimizedHistory, int m);
void runGRTSmcSec(bool optimizedHistory, int m, int c);
void runGRTSkOpt(int k, bool optimizedUpdates);
void runGRTSmOpt(int m);
void runGRTSmcOpt(int m, int c);
__my_inline void runGRTS_processUpdate(int r, double* updateT, double* updateX, double* updateY, int updateSize, double predSpeedX, double predSpeedY);

__my_inline double interpolate(double t, double t0, double x0, double t1, double x1);
__my_inline double distance(double x0, double y0, double x1, double y1);
__my_inline double distanceToLineSection(double t, double x, double y, double t0, double x0, double y0, double t1, double x1, double y1);
__my_inline double distanceToVector(double t, double x, double y, double t0, double x0, double y0, double xV, double yV);
__my_inline double getMaxDeviationAlongLineSection();
__my_inline bool evalLDRCond(double currentT, double currentX, double currentY, double currentSpeedX, double currentSpeedY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY);
__my_inline bool evalLDRCondHalf(bool optCond, double currentT, double currentX, double currentY, double currentSpeedX, double currentSpeedY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY);

void clearSimplified();
void checkSimplified(char* policy);
void printTrajectory(double* t, double* x, double* y, int size);

void writeResultsHeader();
void writeResultsDataOTS(char* algorithm, char* params);
void writeResultsDataRTS(char* algorithm, char* params, int messageCount, int payloadSum, int memoryConsumption, unsigned long long processorTicks);
void writeResultsBytes(char* buffer);

unsigned long long getProcessorTicksPerSec();
__my_inline unsigned long long getProcessorTicks();

__my_inline double myDoubleMax(double a, double b);
__my_inline double myDoubleMin(double a, double b);
__my_inline int myIntMax(int a, int b);
__my_inline int myIntMin(int a, int b);
__my_inline unsigned long long myUnsignedLongLongMax(unsigned long long a, unsigned long long b);

__my_inline void myMSleep(int millis);

__my_inline void myHandleError(char* message);



// Code convention: Upper-case names either refer to pre-processor constants, global constants, or global variables that should be modified in the main-function only!

double ORIGINAL_T[MAX_ORIGINAL_SIZE];
double ORIGINAL_X[MAX_ORIGINAL_SIZE];
double ORIGINAL_Y[MAX_ORIGINAL_SIZE];
int ORIGINAL_SIZE;

double simplifiedT[MAX_ORIGINAL_SIZE];
double simplifiedX[MAX_ORIGINAL_SIZE];
double simplifiedY[MAX_ORIGINAL_SIZE];
int simplifiedSize;

const double SENS_TIME = 1.0; // 1000ms between two consecutive sensing operations.
const double SENS_INACC_SYS = 0.9 * 7.8; // The U.S.DoD maintains that their system will provide standard civilian applications with a horizontal accuracy of 13m [Ubl2006b].
const double SENS_INACC_NOI = 0.1 * 7.8; // Performance Standard [http://www.navcen.uscg.gov/gps/geninfo/2008SPSPerformanceStandardFINAL.pdf] states 7.8m horizontal accuracy in 95%.
// Noise accounts for about 10% of the error, while the systematic error accounts for 90% [Ubl2006b,Ran1994].
const double PROC_COM_TIME = 0.2; // 200ms for local processing, transmission of the update, and processing of the update at the MOD.
const double MAX_SPEED = 20.0; // Maximum speed of 20m/s - rather low for cars.
const double MAX_ACCEL = 10.0; // Maximum acceleration/deceleration of 10m/s?- normal for cars.
int PHYS_CONSTRAINT = -1; // This parameter is set and varied below.
double ACCURACY_BOUND = -999999999.0; // This parameter is set and varied below.


FILE* FP_RESULTS = NULL;
char* TRAJECTORY_FILE = NULL;
char* RESULT_FILE = NULL;
int accuracyBoundIndex;
string alg;

int main(int argc, char** argv) {
	int distances[MAX_ORIGINAL_SIZE];
	int previous[MAX_ORIGINAL_SIZE];
	bool queued[MAX_ORIGINAL_SIZE];
	int queuedCount;
	short neighbors[MAX_ORIGINAL_SIZE][MAX_GRAPH_NEIGHBORS_OTS];
	int neighborsSizes[MAX_ORIGINAL_SIZE];



	FILE* fpTrajectory = NULL;
	double originalLength = 0.0;
	double originalTime = 0.0;
	double originalMinTimeSpan = 999999999.0;
	double originalMaxTimeSpan = -999999999.0;
	double originalMinX = 999999999.0;
	double originalMaxX = -999999999.0;
	double originalMinY = 999999999.0;
	double originalMaxY = -999999999.0;
	int i = -1;

	if (argc != 6) {
		printf("Call %s  algortithm trajectory-file  original_size result-file accurancyBoundIndex.\n\n", argv[0]);
	exit(EXIT_SUCCESS);
	}
	alg = argv[1];
	TRAJECTORY_FILE = argv[2];
	ORIGINAL_SIZE = atoi(argv[3]);
	RESULT_FILE = argv[4];
	ACCURACY_BOUND = atof(argv[5]);

	//TRAJECTORY_FILE = "E:\\Trajectory compression\\sourecode\\Efficient real-time trajectory tracking\\Dead_Recoking\\Car_Traces\\4.txt";
	//RESULT_FILE = "E:\\Trajectory compression\\sourecode\\Efficient real-time trajectory tracking\\Dead_Recoking\\Car_Traces\\result.txt ";
	//printf("Processor speed is %llu ticks per second.\n", getProcessorTicksPerSec());

	fpTrajectory = fopen(TRAJECTORY_FILE, "rb");
	if (fpTrajectory == NULL) {
		myHandleError("Could not open trajectory file.");
	}

	//fseek(fpTrajectory, 0L, SEEK_END);
	//if (ftell(fpTrajectory) % 24 != 0) {
		//myHandleError("Size of trajectory file is not a multiple of 24.");
	//}
	//ORIGINAL_SIZE = ftell(fpTrajectory) / 24;
	//fseek(fpTrajectory, 0L, SEEK_SET);
	//ORIGINAL_SIZE =29 ;
	double *c = (double *)malloc(3 * ORIGINAL_SIZE * sizeof(double));
	for (i = 0; i < 3*ORIGINAL_SIZE; i++) {
		fscanf(fpTrajectory, "%lf", &c[i]);			    
		
		/*
		int bytesRead = -1;
		char* target;
		int j = -1;
		int kind = -1;
		char buffer[sizeof(double)];

		for (kind = 0; kind <= 2; kind++) {
			bytesRead = fread(buffer, 1, sizeof(double), fpTrajectory);
			if (bytesRead != sizeof(double)) {
				myHandleError("Could not read double from trajectory file.");
			}
			if (kind == 0) {
				target = (char*)&ORIGINAL_T[i];
			}
			else if (kind == 1) {
				target = (char*)&ORIGINAL_X[i];
			}
			else {
				target = (char*)&ORIGINAL_Y[i];
			}
			for (j = 0; j < sizeof(double); j++) {
				target[j] = buffer[sizeof(double)-1 - j];
			}
		}*/
	
	}
	int j = 0;
	for (int i = 0; i < 3 * ORIGINAL_SIZE; j++)
	{
		ORIGINAL_X[j] = c[i];
		ORIGINAL_Y[j] = c[i + 1];
		ORIGINAL_T[j] = c[i + 2];
		i = i + 3;
	}
	fclose(fpTrajectory);
	//printTrajectory(ORIGINAL_T, ORIGINAL_X, ORIGINAL_Y, ORIGINAL_SIZE);
    //exit(0);
	originalMinX = ORIGINAL_X[0];
	originalMaxX = ORIGINAL_X[0];
	originalMinY = ORIGINAL_Y[0];
	originalMaxY = ORIGINAL_Y[0];

	for (i = 1; i < ORIGINAL_SIZE; i++) {
		originalMinTimeSpan = myDoubleMin(originalMinTimeSpan, ORIGINAL_T[i] - ORIGINAL_T[i - 1]);
		//assert(ORIGINAL_T[i] - ORIGINAL_T[i - 1] > 0.0);
		originalMaxTimeSpan = myDoubleMax(originalMaxTimeSpan, ORIGINAL_T[i] - ORIGINAL_T[i - 1]);
		originalLength += distance(ORIGINAL_X[i], ORIGINAL_Y[i], ORIGINAL_X[i - 1], ORIGINAL_Y[i - 1]);
		originalMinX = myDoubleMin(originalMinX, ORIGINAL_X[i]);
		originalMaxX = myDoubleMax(originalMaxX, ORIGINAL_X[i]);
		originalMinY = myDoubleMin(originalMinY, ORIGINAL_Y[i]);
		originalMaxY = myDoubleMax(originalMaxY, ORIGINAL_Y[i]);
	}
	originalTime = ORIGINAL_T[ORIGINAL_SIZE - 1] - ORIGINAL_T[0];
	//printf("Loaded trajectory %s with the following characteristics:\n", TRAJECTORY_FILE);
	//printf("  Number of position records: %d\n", ORIGINAL_SIZE);
	//printf("  Time: %2.1f from %2.1f to %2.1f with min time-span %5.4f and max time-span %5.4f\n", originalTime, ORIGINAL_T[0], ORIGINAL_T[ORIGINAL_SIZE - 1], originalMinTimeSpan, originalMaxTimeSpan);
	//printf("  Length: %2.1f\n", originalLength);
	//printf("  Average speed: %2.1f\n", (originalLength / originalTime));
	//printf("  Minimum bounding rectangle: [%2.1f,%2.1f]-[%2.1f,%2.1f]\n", originalMinX, originalMinY, originalMaxX, originalMaxY);
    assert(ORIGINAL_SIZE >= 0);
	//assert(originalMinTimeSpan >= 0.9999);
	//assert(originalMaxTimeSpan <= 1.0001);
	//assert((originalLength / originalTime) >= 0.1);
	//assert((originalLength / originalTime) <= 50.0);
	assert(originalMaxX - originalMinX <= 10000000.0);
	assert(originalMaxY - originalMinY <= 10000000.0);

    //FP_RESULTS = fopen(RESULT_FILE, "w");
	//if (FP_RESULTS == NULL) {
		//myHandleError("Could not open result file.");
	//}

	//writeResultsHeader();

	{ // Definition of the run configuration.

		const int accuracyBoundsSize = 15;
		const double accuracyBoundMinExp = 1.397940008672038; // Default 1.397940008672038 - yields to 25m.
		const double accuracyBoundMaxExp = 2.0; // Default 2.0 - yields to 100m.
		struct timeval start, end;
		//for (accuracyBoundIndex = 0; accuracyBoundIndex < accuracyBoundsSize; accuracyBoundIndex++) {
			//ACCURACY_BOUND = pow(10.0, accuracyBoundMinExp + (accuracyBoundMaxExp - accuracyBoundMinExp) * accuracyBoundIndex / (accuracyBoundsSize - 1.0));
			//printf("Running policies with accuracyBound=%1.1f, i.e. iteration %i of %i:\n", ACCURACY_BOUND, (accuracyBoundIndex + 1), accuracyBoundsSize);
                        //printf("%d\n",ACCURACY_BOUND);
			//ACCURACY_BOUND = ACCURACY_BOUND / 2.0;
			//printf("%lf\n",ACCURACY_BOUND);
			PHYS_CONSTRAINT = PHYS_MAX_SPEED_CONSTRAINT; // To show effectiveness of acceleration constraint.
			//runLSOpt();
			//runLDRHalf(FALSE);
			//runLDRHalf(TRUE);
			//runCDR(FALSE); // To show effectiveness of heap regarding memory consumption and processor ticks.
			if (alg == "CDR")
			{
				//struct timeval start, end;
			        gettimeofday(&start, NULL);
				runCDR(TRUE);
				gettimeofday(&end, NULL);
				//FP_RESULTS = fopen(RESULT_FILE, "w");
				//if (FP_RESULTS == NULL) {
				//	myHandleError("Could not open result file.");
				//}
				//int j = 0;
				//for (j = 0; j < simplifiedSize; j++)
				//	fprintf(FP_RESULTS, "%lf %lf %lf\n",simplifiedX[j], simplifiedY[j],simplifiedT[j]);
			}
			
			if (alg == "CDRm_10")
				{
					gettimeofday(&start, NULL);
					runCDRm(10); // Vary m to show influence on reduction.
					gettimeofday(&end, NULL);
				}
			if (alg == "CDRm_100")
			{
				gettimeofday(&start, NULL);
				runCDRm(100);
				gettimeofday(&end, NULL);
			}
			if (alg == "CDRm_500")
			{
				gettimeofday(&start, NULL);
				runCDRm(500);
				gettimeofday(&end, NULL);
			}
			if (alg == "CDRm_1000")
			{
				gettimeofday(&start, NULL);
				runCDRm(1000);
				gettimeofday(&end, NULL);
			}
			if (alg == "GRTSkOpt_0")
			{
			    gettimeofday(&start, NULL);
			    runGRTSkOpt(0, TRUE); // Vary k to show influence on reduction.
			    gettimeofday(&end, NULL);
						//runGRTSkOpt(1, TRUE);
						//runGRTSkOpt(3, TRUE);
						//runGRTSkOpt(3, FALSE); // To show effectiveness of optimized updates regarding payload size.
			}
			if (alg == "GRTSkOpt_1")
			{
			gettimeofday(&start, NULL);
           		runGRTSkOpt(1, TRUE);
			gettimeofday(&end, NULL);
     			}		
      			if (alg == "GRTSkOpt_3")
     			{	
			gettimeofday(&start, NULL);
           		runGRTSkOpt(3, TRUE);
			gettimeofday(&end, NULL);                          
    			 }	
			if (alg == "GRTSmSec_100")
			
			{
				gettimeofday(&start, NULL);
				runGRTSmSec(TRUE, 100); // Vary m to show influence on reduction.
				//
				//runGRTSmSec(TRUE, 1000);
				//runGRTSmSec(TRUE, 999999999);
				//runGRTSmSec(FALSE, 100); // To show effectiveness of optimized history and for comparability with GRTSmOpt.
				//runGRTSmSec(FALSE, 500);
				//runGRTSmSec(FALSE, 1000);
				//runGRTSmSec(FALSE, 999999999);

				//runGRTSmcSec(TRUE, 100, 0); // Try c = 0 to verify consistency with GRTSmSec.
				//runGRTSmcSec(TRUE, 100, 1); // Vary m to show influence on reduction.
				//runGRTSmcSec(TRUE, 500, 1);
				//runGRTSmcSec(TRUE, 1000, 1);
				gettimeofday(&end, NULL);

			}
     				 if (alg == "GRTSmSec_500")
      				{	
      				gettimeofday(&start, NULL);
      				runGRTSmSec(TRUE, 500);
				gettimeofday(&end, NULL);
      				}	
      				if (alg == "GRTSmSec_1000")
     				 {
      				gettimeofday(&start, NULL);
      				runGRTSmSec(TRUE, 10000);
				gettimeofday(&end, NULL);
     				 }
      				if (alg == "GRTSmSec_999999999")
     				 {	
      				gettimeofday(&start, NULL);
      				runGRTSmSec(TRUE, 999999999);
				gettimeofday(&end, NULL);
      				}
			if (alg == "GRTSmOpt_100")
			{
				gettimeofday(&start, NULL);
				runGRTSmOpt(100);
				 gettimeofday(&end, NULL);
				//runGRTSmOpt(100); // Vary m to show influence on reduction.
				//runGRTSmOpt(500);
				//runGRTSmOpt(1000);
				//runGRTSmcOpt(100, 0); // Try c=0 to verify consistency with GRTSmSec.
				//runGRTSmcOpt(100, 1); // Show effectiveness of compression.
				//runGRTSmcOpt(500, 1);
				//runGRTSmcOpt(1000, 1);
				//runGRTSmcOpt(100, 2); // Show that c = 2 gives very similar results, i.e. c > 2 is unnecessary.
				//runGRTSmcOpt(500, 2);
				//runGRTSmcOpt(1000, 2);
			}
			if (alg == "GRTSmOpt_500")
                        {
                                gettimeofday(&start, NULL);
                                runGRTSmOpt(500);
                                gettimeofday(&end, NULL);
                        }
			if (alg == "GRTSmOpt_1000")
                        {
                                gettimeofday(&start, NULL);
                                runGRTSmOpt(1000);
                                gettimeofday(&end, NULL);
                        }      

	  FP_RESULTS = fopen(RESULT_FILE, "w");
          if (FP_RESULTS == NULL) {
             myHandleError("Could not open result file.");
           }
          int j = 0;
           for (j = 0; j < simplifiedSize; j++)
                   fprintf(FP_RESULTS, "%lf %lf %lf\n",simplifiedX[j], simplifiedY[j],simplifiedT[j]);
           fprintf(FP_RESULTS,"%lf",(1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000000.0);

	}

	//printf("Completely finished.\n");
	/*
	FP_RESULTS = fopen(RESULT_FILE, "w");
        if (FP_RESULTS == NULL) {
           myHandleError("Could not open result file.");
         }
         int j = 0;
         for (j = 0; j < simplifiedSize; j++)
                  fprintf(FP_RESULTS, "%lf %lf %lf\n",simplifiedX[j], simplifiedY[j],simplifiedT[j]);
	 fprintf(FP_RESULTS,"%lf",(1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec)/1000000.0);
	*/ 
	 fclose(FP_RESULTS);
	 return EXIT_SUCCESS;
}


void runLSOpt() {
	int distances[MAX_ORIGINAL_SIZE];
	int previous[MAX_ORIGINAL_SIZE];
	bool queued[MAX_ORIGINAL_SIZE];
	int queuedCount;
	short neighbors[MAX_ORIGINAL_SIZE][MAX_GRAPH_NEIGHBORS_OTS];
	int neighborsSizes[MAX_ORIGINAL_SIZE];
	int i;

//	printf("  Running LSOpt:\n");

	clearSimplified();

	queuedCount = ORIGINAL_SIZE;
//	printf("    Neighbor lists: ");
	for (i = 0; i < ORIGINAL_SIZE; i++) {
		distances[i] = INT_MAX;
		previous[i] = -1;
		queued[i] = TRUE;
		neighborsSizes[i] = 0;
	}
	for (i = ORIGINAL_SIZE - 1; i >= 0; i--) {
		int j;
		for (j = 0; j < i; j++) {
			int k;
			bool add = TRUE;
			// Do not test linearly from j+1 to i-1 but start in the middle, i.e. at (j + i) / 2. Also stop the loop once add becomes false.
			for (k = (j + i) / 2; k > j && add; k--) {
				add = (distanceToLineSection(ORIGINAL_T[k], ORIGINAL_X[k], ORIGINAL_Y[k], ORIGINAL_T[j], ORIGINAL_X[j], ORIGINAL_Y[j], ORIGINAL_T[i], ORIGINAL_X[i], ORIGINAL_Y[i]) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
			}
			for (k = (j + i) / 2 + 1; k < i && add; k++) {
				add = (distanceToLineSection(ORIGINAL_T[k], ORIGINAL_X[k], ORIGINAL_Y[k], ORIGINAL_T[j], ORIGINAL_X[j], ORIGINAL_Y[j], ORIGINAL_T[i], ORIGINAL_X[i], ORIGINAL_Y[i]) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
			}
			if (add) {
				neighbors[j][neighborsSizes[j]] = i;
				neighborsSizes[j]++;
				assert(neighborsSizes[j] < MAX_GRAPH_NEIGHBORS_OTS);
			}
		}
	//	if (i % (ORIGINAL_SIZE / 50) == 0) {
	//		printf(".");
	//	}
	}
	//printf("\n");

//	printf("    Dijkstra's algorithm: ");
	distances[0] = 0;
	while (queuedCount > 0) {
		int k;
		int distanceMin = INT_MAX;
		for (k = 0; k < ORIGINAL_SIZE; k++) {
			if (queued[k] && distances[k] < distanceMin) {
				distanceMin = distances[k];
				i = k;
			}
		}
		queued[i] = FALSE;
		queuedCount--;
		for (k = 0; k < neighborsSizes[i]; k++) {
			int j = neighbors[i][k];
			if (queued[j]) {
				if (distances[i] + 1 < distances[j]) {
					distances[j] = distances[i] + 1;
					previous[j] = i;
				}
			}
		}
		//if (queuedCount % (ORIGINAL_SIZE / 50) == 0) {
		//	printf(".");
		//}
	}
	//printf("\n");

	// Store simplified trajectory, where previous-sequence is traversed in reverse order.
	simplifiedSize = 0;
	i = ORIGINAL_SIZE - 1;
	while (i >= 0) {
		simplifiedT[distances[ORIGINAL_SIZE - 1] - simplifiedSize] = ORIGINAL_T[i];
		simplifiedX[distances[ORIGINAL_SIZE - 1] - simplifiedSize] = ORIGINAL_X[i];
		simplifiedY[distances[ORIGINAL_SIZE - 1] - simplifiedSize] = ORIGINAL_Y[i];
		simplifiedSize++;
		i = previous[i];
	}
	assert(i == -1 && distances[ORIGINAL_SIZE - 1] + 1 == simplifiedSize);

	//checkSimplified("LSOpt");

	//writeResultsDataOTS("LSOpt", "n/a");

//	printf("    done.\n");
}


void runLSDP() {
	int i = 0;

	printf("  Running LSDP:\n");

	clearSimplified();

	for (i = 0; i < ORIGINAL_SIZE; i++) {
		simplifiedT[i] = ORIGINAL_T[i];
		simplifiedX[i] = ORIGINAL_X[i];
		simplifiedY[i] = ORIGINAL_Y[i];
	}
	simplifiedSize = ORIGINAL_SIZE;

	runLSDP_recursive(0, simplifiedSize - 1);

	//checkSimplified("LSDP");

	//writeResultsDataOTS("LSDP", "n/a");

//	printf("    done.\n");
}


void runLSDP_recursive(int l, int r) {
	double maxDist = 0.0;
	int maxK = l;
	int k = 0;

	for (k = l; k <= r; k++) {
		double dist = distanceToLineSection(simplifiedT[k], simplifiedX[k], simplifiedY[k], simplifiedT[l], simplifiedX[l], simplifiedY[l], simplifiedT[r], simplifiedX[r], simplifiedY[r]);
		if (dist > maxDist) {
			maxDist = dist;
			maxK = k;
		}
	}
	if (maxDist + getMaxDeviationAlongLineSection() >= ACCURACY_BOUND) {
		int prevSize = simplifiedSize;
		int shift = -1;
		runLSDP_recursive(l, maxK);
		shift = prevSize - simplifiedSize;
		runLSDP_recursive(maxK - shift, r - shift);
	}
	else {
		int shift = r - l - 1;
		int i = 0;
		simplifiedSize = simplifiedSize - shift;
		for (i = l + 1; i < simplifiedSize; i++) {
			simplifiedT[i] = simplifiedT[i + shift];
			simplifiedX[i] = simplifiedX[i + shift];
			simplifiedY[i] = simplifiedY[i + shift];
		}
	}
}


void runOPWTR() {
	int e = 0;
	int originalIndex = 0;

	//printf("  Running OPWTR:\n");

	clearSimplified();

	simplifiedT[simplifiedSize] = ORIGINAL_T[originalIndex];
	simplifiedX[simplifiedSize] = ORIGINAL_X[originalIndex];
	simplifiedY[simplifiedSize] = ORIGINAL_Y[originalIndex];
	simplifiedSize++;

	e = originalIndex + 2;

	while (e < ORIGINAL_SIZE) {
		int i = originalIndex + 1;
		bool condOPWTR = TRUE;
		while (i < e && condOPWTR) {
			if (distanceToLineSection(ORIGINAL_T[i], ORIGINAL_X[i], ORIGINAL_Y[i], ORIGINAL_T[originalIndex], ORIGINAL_X[originalIndex], ORIGINAL_Y[originalIndex], ORIGINAL_T[e], ORIGINAL_X[e], ORIGINAL_Y[e]) + getMaxDeviationAlongLineSection() > ACCURACY_BOUND) {
				condOPWTR = FALSE;
			}
			else {
				i++;
			}
		}
		if (!condOPWTR) {
			originalIndex = i;
			simplifiedT[simplifiedSize] = ORIGINAL_T[originalIndex];
			simplifiedX[simplifiedSize] = ORIGINAL_X[originalIndex];
			simplifiedY[simplifiedSize] = ORIGINAL_Y[originalIndex];
			simplifiedSize++;
			e = originalIndex + 2;
		}
		else {
			e++;
		}
	}

	simplifiedT[simplifiedSize] = ORIGINAL_T[ORIGINAL_SIZE - 1];
	simplifiedX[simplifiedSize] = ORIGINAL_X[ORIGINAL_SIZE - 1];
	simplifiedY[simplifiedSize] = ORIGINAL_Y[ORIGINAL_SIZE - 1];
	simplifiedSize++;

	//checkSimplified("OPWTR");

	//writeResultsDataOTS("OPWTR", "n/a");

	//printf("    done.\n");
}


void runLDRHalf(bool optCond) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;

	//printf("  Running LDRHalf with optCond=%s:\n", ((optCond) ? "true" : "false"));

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY, lastT, lastX, lastY;
		int originalIndex;

		originalIndex = 0;

		clearSimplified();

		currentT = ORIGINAL_T[0];
		currentX = ORIGINAL_X[0];
		currentY = ORIGINAL_Y[0];
		predOriginT = currentT;
		predOriginX = currentX;
		predOriginY = currentY;
		predSpeedX = 0.0;
		predSpeedY = 0.0;
		runLDRHalf_processUpdate(predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
		messageCount++;
		payloadSum += BYTES_HEADER + BYTES_STPOINT + BYTES_VECTOR;
		lastT = currentT;
		lastX = currentX;
		lastY = currentY;

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) {
			double currentSpeedX, currentSpeedY;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			currentT = ORIGINAL_T[originalIndex];
			currentX = ORIGINAL_X[originalIndex];
			currentY = ORIGINAL_Y[originalIndex];
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			if (!evalLDRCondHalf(optCond, currentT, currentX, currentY, currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)) {
				predOriginT = currentT;
				predOriginX = currentX;
				predOriginY = currentY;
				predSpeedX = (currentX - lastX) / (currentT - lastT);
				predSpeedY = (currentY - lastY) / (currentT - lastT);
				runLDRHalf_processUpdate(predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);

				messageCount++;
				payloadSum += BYTES_HEADER + BYTES_STPOINT + BYTES_VECTOR;
			}
			lastT = currentT;
			lastX = currentX;
			lastY = currentY;

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}

		runLDRHalf_processFinalUpdate(currentT, currentX, currentY);
		messageCount++;
		payloadSum += BYTES_HEADER + BYTES_STPOINT;

		myMSleep(REPETITION_SLEEP_MILLIS);
		printf(" %i", (repetition + 1));
	}
	printf("\n");

	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		// checkSimplified("LDRHalf"); // Do not check simplified trajectory since there may be deviations greater than epsilon because of jumps in the original trajectory!

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		assert(messageCount == simplifiedSize || messageCount == simplifiedSize + 1);
		assert(payloadSum == simplifiedSize * BYTES_HEADER + (simplifiedSize - 1) * (BYTES_STPOINT + BYTES_VECTOR) + 1 * BYTES_STPOINT || payloadSum == (simplifiedSize + 1) * BYTES_HEADER + (simplifiedSize - 1 + 1) * (BYTES_STPOINT + BYTES_VECTOR) + 1 * BYTES_STPOINT);

	//mySnprintf(paramString, MAX_STRING_LENGTH, "optCond=%s", ((optCond) ? "true" : "false"));
		//writeResultsDataRTS("LDRHalf", paramString, messageCount, payloadSum, 3 * BYTES_STPOINT, maxProcessorTicks);
	}

//	printf("    done.\n");
}


__my_inline void runLDRHalf_processUpdate(double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	assert(simplifiedSize == 0 || predOriginT > simplifiedT[simplifiedSize - 1]);
	simplifiedT[simplifiedSize] = predOriginT;
	simplifiedX[simplifiedSize] = predOriginX;
	simplifiedY[simplifiedSize] = predOriginY;
	simplifiedSize++;
}


__my_inline void runLDRHalf_processFinalUpdate(double currentT, double currentX, double currentY) {
	assert(simplifiedSize == 0 || currentT >= simplifiedT[simplifiedSize - 1]);
	if (simplifiedSize > 0 && currentT == simplifiedT[simplifiedSize - 1]) {
		// Do nothing. Message just indicates end of trajectory.
	}
	else {
		simplifiedT[simplifiedSize] = currentT;
		simplifiedX[simplifiedSize] = currentX;
		simplifiedY[simplifiedSize] = currentY;
		simplifiedSize++;
	}
}


void runCDR(bool useHeap) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;
	//fprintf(FP_RESULTS, "%d\n", ACCURACY_BOUND);
	//printf("  Running CDR with useHeap=%s:\n", ((useHeap) ? "true" : "false"));

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY, lastT, lastX, lastY;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		int historySize = 0;
		int originalIndex;

		originalIndex = 0;

		clearSimplified();

		currentT = ORIGINAL_T[0];
		currentX = ORIGINAL_X[0];
		currentY = ORIGINAL_Y[0];
		predOriginT = currentT;
		predOriginX = currentX;
		predOriginY = currentY;
		predSpeedX = 0.0;
		predSpeedY = 0.0;
		runCDR_processUpdate(predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
		messageCount++;
		payloadSum += BYTES_HEADER + BYTES_STPOINT + BYTES_VECTOR;
		lastT = currentT;
		lastX = currentX;
		lastY = currentY;

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) {
			double currentSpeedX, currentSpeedY;
			bool sectionCond;
			int k;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			currentT = ORIGINAL_T[originalIndex];
			currentX = ORIGINAL_X[originalIndex];
			currentY = ORIGINAL_Y[originalIndex];
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			if (useHeap) {
				while (historySize > 0 && runCDR_phi(historyT[0], historyX[0], historyY[0], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) * (currentT - predOriginT) > ACCURACY_BOUND) {
					runCDR_historyPop(historyT, historyX, historyY, &historySize, currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
				}
			}

			sectionCond = TRUE;
			for (k = 0; k < historySize && sectionCond; k++) {
				sectionCond = (distanceToLineSection(historyT[k], historyX[k], historyY[k], predOriginT, predOriginX, predOriginY, currentT, currentX, currentY) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
			}

			if (!(evalLDRCond(currentT, currentX, currentY, currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) && sectionCond)) {
				predOriginT = lastT;
				predOriginX = lastX;
				predOriginY = lastY;
				predSpeedX = (currentX - lastX) / (currentT - lastT);
				predSpeedY = (currentY - lastY) / (currentT - lastT);

				maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + 3) * BYTES_STPOINT + BYTES_VECTOR);

				runCDR_processUpdate(predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
				messageCount++;
				payloadSum += BYTES_HEADER + BYTES_STPOINT + BYTES_VECTOR;
				historySize = 0;
			}

			if (useHeap) {
				runCDR_historyPush(historyT, historyX, historyY, &historySize, currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
			}
			else {
				historyT[historySize] = currentT;
				historyX[historySize] = currentX;
				historyY[historySize] = currentY;
				historySize++;
			}

			lastT = currentT;
			lastX = currentX;
			lastY = currentY;

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}
		runCDR_processFinalUpdate(currentT, currentX, currentY);
		messageCount++;
		payloadSum += BYTES_HEADER + BYTES_STPOINT;

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
	/*printf("\n");

	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("CDR");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		assert(messageCount == simplifiedSize || messageCount == simplifiedSize + 1);
		assert(payloadSum == simplifiedSize * BYTES_HEADER + (simplifiedSize - 1) * (BYTES_STPOINT + BYTES_VECTOR) + 1 * BYTES_STPOINT ||
			payloadSum == (simplifiedSize + 1) * BYTES_HEADER + (simplifiedSize - 1 + 1) * (BYTES_STPOINT + BYTES_VECTOR) + 1 * BYTES_STPOINT);

		//mySnprintf(paramString, MAX_STRING_LENGTH, "useHeap=%s", ((useHeap) ? "true" : "false"));
		//writeResultsDataRTS("CDR", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}

	printf("    done.\n");
	*/
}


void runCDRm(int m) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;

	//printf("  Running CDRm with m=%d:\n", m);
  //      cout << 111111111111111111 <<endl;
	//printf("    %i repetitions:", REPETITIONS);
	//cout << 2222222222222222222 << endl;
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY, lastT, lastX, lastY;
		double estimate;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		int originalIndex;
		int historySize = 0;

		originalIndex = 0;

		clearSimplified();

		currentT = ORIGINAL_T[0];
		currentX = ORIGINAL_X[0];
		currentY = ORIGINAL_Y[0];
		predOriginT = currentT;
		predOriginX = currentX;
		predOriginY = currentY;
		predSpeedX = 0.0;
		predSpeedY = 0.0;
		runCDR_processUpdate(predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
		messageCount++;
		payloadSum += BYTES_HEADER + BYTES_STPOINT + BYTES_VECTOR;
		estimate = HUGE_VAL;
		lastT = currentT;
		lastX = currentX;
		lastY = currentY;

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) {
			double currentSpeedX, currentSpeedY;
			bool sectionCondA, sectionCondB;
			int k;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			currentT = ORIGINAL_T[originalIndex];
			currentX = ORIGINAL_X[originalIndex];
			currentY = ORIGINAL_Y[originalIndex];
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			while (historySize > 0 && runCDR_phi(historyT[0], historyX[0], historyY[0], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) * (currentT - predOriginT) > ACCURACY_BOUND) {
				runCDR_historyPop(historyT, historyX, historyY, &historySize, currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
			}

			sectionCondA = TRUE;
			for (k = 0; k < historySize && sectionCondA; k++) {
				sectionCondA = (distanceToLineSection(historyT[k], historyX[k], historyY[k], predOriginT, predOriginX, predOriginY, currentT, currentX, currentY) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
			}

			sectionCondB = (estimate >= distanceToVector(currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) / (currentT - predOriginT));

			if (!(evalLDRCond(currentT, currentX, currentY, currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) && sectionCondA && sectionCondB)) {
				predOriginT = lastT;
				predOriginX = lastX;
				predOriginY = lastY;
				predSpeedX = (currentX - lastX) / (currentT - lastT);
				predSpeedY = (currentY - lastY) / (currentT - lastT);

				maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + 3) * BYTES_STPOINT + BYTES_VECTOR + BYTES_DOUBLE);

				runCDR_processUpdate(predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
				messageCount++;
				payloadSum += BYTES_HEADER + BYTES_STPOINT + BYTES_VECTOR;
				historySize = 0;
				estimate = HUGE_VAL;
			}
			if (historySize == m && runCDR_phi(currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) < estimate) {
				double phiHead = runCDR_phi(historyT[0], historyX[0], historyY[0], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
				estimate = phiHead;
				runCDR_historyPop(historyT, historyX, historyY, &historySize, currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
			}
			if (runCDR_phi(currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) < estimate) {
				runCDR_historyPush(historyT, historyX, historyY, &historySize, currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
			}
			lastT = currentT;
			lastX = currentX;
			lastY = currentY;

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}
		runCDR_processFinalUpdate(currentT, currentX, currentY);
		messageCount++;
		payloadSum += BYTES_HEADER + BYTES_STPOINT;

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
	/*
	printf("\n");

	{ // Check and print results;
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("CDRm");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		assert(messageCount == simplifiedSize || messageCount == simplifiedSize + 1);
		assert(payloadSum == simplifiedSize * BYTES_HEADER + (simplifiedSize - 1) * (BYTES_STPOINT + BYTES_VECTOR) + 1 * BYTES_STPOINT ||
			payloadSum == (simplifiedSize + 1) * BYTES_HEADER + (simplifiedSize - 1 + 1) * (BYTES_STPOINT + BYTES_VECTOR) + 1 * BYTES_STPOINT);

		mySnprintf(paramString, MAX_STRING_LENGTH, "m=%d", m);
		//writeResultsDataRTS("CDRm", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}

	printf("    done.\n");
	*/
}


__my_inline void runCDR_processUpdate(double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	assert(simplifiedSize == 0 || predOriginT >= simplifiedT[simplifiedSize - 1]);
	if (simplifiedSize > 0 && predOriginT == simplifiedT[simplifiedSize - 1]) {
		//assert(simplifiedSize == 1);
		simplifiedX[simplifiedSize - 1] = predOriginX;
		simplifiedY[simplifiedSize - 1] = predOriginY;
	}
	else {
		simplifiedT[simplifiedSize] = predOriginT;
		simplifiedX[simplifiedSize] = predOriginX;
		simplifiedY[simplifiedSize] = predOriginY;
		simplifiedSize++;
	}
}


__my_inline void runCDR_processFinalUpdate(double currentT, double currentX, double currentY) {
	assert(simplifiedSize == 0 || currentT >= simplifiedT[simplifiedSize - 1]);
	if (simplifiedSize > 0 && currentT == simplifiedT[simplifiedSize - 1]) {
		//assert(simplifiedSize == 1);
		simplifiedX[simplifiedSize - 1] = currentX;
		simplifiedY[simplifiedSize - 1] = currentY;
	}
	else {
		simplifiedT[simplifiedSize] = currentT;
		simplifiedX[simplifiedSize] = currentX;
		simplifiedY[simplifiedSize] = currentY;
		simplifiedSize++;
	}
}


__my_inline void runCDR_historyPop(double* historyT, double* historyX, double* historyY, int* historySize, double currentT, double currentX, double currentY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	*historySize = *historySize - 1;
	if (*historySize > 0) {
		int k = 0;
		int half = *historySize / 2;
		while (k < half) {
			int child, right;
			double phiChild, phiLast;
			child = k * 2 + 1;
			right = child + 1;
			phiChild = runCDR_phi(historyT[child], historyX[child], historyY[child], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
			if (right < *historySize) {
				double phiRight = runCDR_phi(historyT[right], historyX[right], historyY[right], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
				if (phiRight > phiChild) {
					child = right;
					phiChild = phiRight;
				}
			}
			phiLast = runCDR_phi(historyT[*historySize], historyX[*historySize], historyY[*historySize], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
			if (phiLast >= phiChild) {
				break;
			}
			historyT[k] = historyT[child];
			historyX[k] = historyX[child];
			historyY[k] = historyY[child];
			k = child;
		}
		historyT[k] = historyT[*historySize];
		historyX[k] = historyX[*historySize];
		historyY[k] = historyY[*historySize];
	}
}


__my_inline void runCDR_historyPush(double* historyT, double* historyX, double* historyY, int* historySize, double currentT, double currentX, double currentY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	int k = *historySize;
	*historySize = *historySize + 1;
	while (k > 0) {
		double phiParent, phiC;
		int parent = (k - 1) / 2;
		phiParent = runCDR_phi(historyT[parent], historyX[parent], historyY[parent], predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
		phiC = runCDR_phi(currentT, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY);
		if (phiParent >= phiC) {
			break;
		}
		historyT[k] = historyT[parent];
		historyX[k] = historyX[parent];
		historyY[k] = historyY[parent];
		k = parent;
	}
	historyT[k] = currentT;
	historyX[k] = currentX;
	historyY[k] = currentY;
}


__my_inline double runCDR_phi(double siT, double siX, double siY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	return (ACCURACY_BOUND - getMaxDeviationAlongLineSection() - distanceToVector(siT, siX, siY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)) / (siT - predOriginT);
}


void runGRTSmSec(bool optimizedHistory, int m) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;

	//printf("  Running GRTSmSec with optimizedHistory=%s and m=%d:\n", ((optimizedHistory) ? "true" : "false"), m);

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		int historySize;

		double updateT[MAX_ORIGINAL_SIZE];
		double updateX[MAX_ORIGINAL_SIZE];
		double updateY[MAX_ORIGINAL_SIZE];
		int updateSize;

		int originalIndex;
		int variableSize; // Only 0 or 1 here.

		originalIndex = 0;
		historySize = 0;
		updateSize = 0;
		variableSize = 0;

		clearSimplified();

		{ // Compute and send initial update.
			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historySize++;

			updateT[updateSize] = historyT[historySize - 1];
			updateX[updateSize] = historyX[historySize - 1];
			updateY[updateSize] = historyY[historySize - 1];
			updateSize++;
			predOriginT = historyT[0];
			predOriginX = historyX[0];
			predOriginY = historyY[0];
			predSpeedX = 0.0;
			predSpeedY = 0.0;

			variableSize = 0;
			runGRTS_processUpdate(0, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY); // Transmission of variableSize is not measured as it is always 0 for the first two updates.
			messageCount++;
			payloadSum += BYTES_HEADER + updateSize * BYTES_STPOINT + BYTES_VECTOR;

			updateSize = 0;
		}

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) {
			double currentSpeedX, currentSpeedY;
			bool sectionCond;
			int k;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historySize++;
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + 1) * BYTES_STPOINT + BYTES_VECTOR);

			sectionCond = TRUE;
			for (k = 1; k < historySize - 1; k++) { // It is possible to include "&& add" in the loop-condition. But, regarding computing time, this does not help in the worst-case.
				sectionCond = sectionCond && (distanceToLineSection(historyT[k], historyX[k], historyY[k], historyT[0], historyX[0], historyY[0], historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1]) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
			}
			if (!sectionCond || historySize == m) {
				assert(historySize >= 2);
				updateT[updateSize] = historyT[historySize - 2]; // Add last sensed position before current one as new vertex to update.
				updateX[updateSize] = historyX[historySize - 2];
				updateY[updateSize] = historyY[historySize - 2];
				updateSize++;
				historyT[0] = historyT[historySize - 2];
				historyX[0] = historyX[historySize - 2];
				historyY[0] = historyY[historySize - 2];
				historyT[1] = historyT[historySize - 1];
				historyX[1] = historyX[historySize - 1];
				historyY[1] = historyY[historySize - 1];
				historySize = 2;
			}

			if (optimizedHistory) {
				for (k = 1; k < historySize - 2; k++) { // Do not optimize last two positions of history such that last sensed position before the current one always is at history[historySize - 2].
					double siX = interpolate(historyT[k], historyT[0], historyX[0], historyT[historySize - 1], historyX[historySize - 1]);
					double siY = interpolate(historyT[k], historyT[0], historyY[0], historyT[historySize - 1], historyY[historySize - 1]);
					double ei = (ACCURACY_BOUND - getMaxDeviationAlongLineSection()) * (historyT[k] - historyT[0]) / (historyT[historySize - 1] - historyT[0]);
					if (distance(siX, siY, historyX[k], historyY[k]) + ei < ACCURACY_BOUND - getMaxDeviationAlongLineSection()) { // Better use "<" instead of "<=" here - due to rounding errors.
						int l;
						for (l = k + 1; l < historySize; l++) { // Shifting could be performed more efficient...
							historyT[l - 1] = historyT[l];
							historyX[l - 1] = historyX[l];
							historyY[l - 1] = historyY[l];
						}
						historySize--;
						k--;
					}
				}
			}

			assert(historySize < m);

			if (originalIndex == ORIGINAL_SIZE - 1 || !evalLDRCond(historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1], currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)) {
				//assert(historyT[historySize - 2] + 0.9 * SENS_TIME < historyT[historySize - 1] && historyT[historySize - 1] < historyT[historySize - 2] + 1.1 * SENS_TIME);
				predOriginT = historyT[historySize - 1];
				predOriginX = historyX[historySize - 1];
				predOriginY = historyY[historySize - 1];
				predSpeedX = (historyX[historySize - 1] - historyX[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				predSpeedY = (historyY[historySize - 1] - historyY[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				updateT[updateSize] = historyT[historySize - 1]; // Add current position as prediction origin to update.
				updateX[updateSize] = historyX[historySize - 1];
				updateY[updateSize] = historyY[historySize - 1];
				updateSize++;

				maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + 1) * BYTES_STPOINT + BYTES_VECTOR);

				runGRTS_processUpdate(variableSize, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
				messageCount++;
				payloadSum += BYTES_HEADER + updateSize * BYTES_STPOINT + BYTES_VECTOR; // Transmission of variableSize is not measured as it is always 0 for the first two updates and then 1.
				updateSize = 0;
				variableSize = 1;
			}

			assert(historySize < m);

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
	/*printf("\n");

	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("GRTSmSec");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		mySnprintf(paramString, MAX_STRING_LENGTH, "optHistory=%s,m=%d", ((optimizedHistory) ? "true" : "false"), m);
		//writeResultsDataRTS("GRTSmSec", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}

	printf("    done.\n");
	*/
}


void runGRTSmcSec(bool optimizedHistory, int m, int c) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;

	assert(c >= 0 && c <= 1 && m >= c + 3);

	//printf("  Running GRTSmcSec with optimizedHistory=%s and m=%d and c=%d:\n", ((optimizedHistory) ? "true" : "false"), m, c);

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		double historyD[MAX_ORIGINAL_SIZE];
		int historySize;
		int compressedSize = 0; // Note, that history[compressedSize] is the latest compressed position as whether history[0] is compressed or not is irrelevant.

		double updateT[MAX_ORIGINAL_SIZE];
		double updateX[MAX_ORIGINAL_SIZE];
		double updateY[MAX_ORIGINAL_SIZE];
		int updateSize;

		int originalIndex;
		int variableSize; // Only 0 or 1 here.

		originalIndex = 0;
		historySize = 0;
		updateSize = 0;
		variableSize = 0;

		clearSimplified();

		{ // Compute and send initial update.
			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historyD[0] = 999999999.0; // historyD[0] must be never used!
			historySize++;

			updateT[updateSize] = historyT[historySize - 1];
			updateX[updateSize] = historyX[historySize - 1];
			updateY[updateSize] = historyY[historySize - 1];
			updateSize++;
			predOriginT = historyT[0];
			predOriginX = historyX[0];
			predOriginY = historyY[0];
			predSpeedX = 0.0;
			predSpeedY = 0.0;

			variableSize = 0;
			runGRTS_processUpdate(0, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY); // Transmission of variableSize is not measured as it is always 0 for the first two updates.
			messageCount++;
			payloadSum += BYTES_HEADER + updateSize * BYTES_STPOINT + BYTES_VECTOR;

			updateSize = 0;
		}

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) {
			double currentSpeedX, currentSpeedY;
			bool sectionCond;
			int k;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historyD[historySize] = getMaxDeviationAlongLineSection(); // Maximum deviation along line section between two consecutive sensed positions given by sensing inaccuracy and movement between fixes.
			historySize++;
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + 1) * BYTES_STPOINT + BYTES_VECTOR + (historySize - 1) * BYTES_DOUBLE); // Count (historySize - 1) * BYTES_DOUBLE for historyD[] only, since historyD[0] could be removed.

			sectionCond = TRUE;
			for (k = 1; k < historySize - 1; k++) { // It is possible to include "&& add" in the loop-condition. But, regarding computing time, this does not help in the worst-case.
				sectionCond = sectionCond && (myDoubleMax(historyD[k], historyD[k + 1]) + distanceToLineSection(historyT[k], historyX[k], historyY[k], historyT[0], historyX[0], historyY[0], historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1]) <= ACCURACY_BOUND); // Condition is equal to checking all line sections, i.e. the pairs of positions [k-1,k].
			}
			if (!sectionCond) {
				assert(historySize >= 2);
				updateT[updateSize] = historyT[historySize - 2]; // Add last sensed position before current one as new vertex to update.
				updateX[updateSize] = historyX[historySize - 2];
				updateY[updateSize] = historyY[historySize - 2];
				updateSize++;
				historyT[0] = historyT[historySize - 2];
				historyX[0] = historyX[historySize - 2];
				historyY[0] = historyY[historySize - 2];
				historyD[0] = 999999999.0; // historyD[0] must be never used!
				historyT[1] = historyT[historySize - 1];
				historyX[1] = historyX[historySize - 1];
				historyY[1] = historyY[historySize - 1];
				historyD[1] = historyD[historySize - 1];
				historySize = 2;
				compressedSize = 0;
			}

			assert(0 <= compressedSize && compressedSize <= 1);

			if (optimizedHistory) { // Remove positions from history that cannot anymore influence sectionCond in subsequenct iterations.
				for (k = 1; k < historySize - 2; k++) { // Do not optimize last two positions of history such that last sensed position before the current one always is at history[historySize - 2].
					double siX = interpolate(historyT[k], historyT[0], historyX[0], historyT[historySize - 1], historyX[historySize - 1]);
					double siY = interpolate(historyT[k], historyT[0], historyY[0], historyT[historySize - 1], historyY[historySize - 1]);
					double ei = (ACCURACY_BOUND - historyD[historySize - 1]) * (historyT[k] - historyT[0]) / (historyT[historySize - 1] - historyT[0]);
					if (distance(siX, siY, historyX[k], historyY[k]) + ei < ACCURACY_BOUND - myDoubleMax(historyD[k], historyD[k + 1])) { // Better use "<" instead of "<=" here - due to rounding errors.
						int l;
						for (l = k + 1; l < historySize; l++) { // Shifting could be performed more efficient...
							historyT[l - 1] = historyT[l];
							historyX[l - 1] = historyX[l];
							historyY[l - 1] = historyY[l];
							historyD[l - 1] = historyD[l];
						}
						if (k == 1 && compressedSize == 1) {
							compressedSize = 0;
						}
						historySize--;
						k--;
					}
				}
			}

			assert(compressedSize <= c && historySize <= m);
			assert(historySize - compressedSize <= m - c);

			// If neccessary, compress a part of the history.
			if (historySize - compressedSize == m - c) {
				double compressedDeviation;

				compressedDeviation = -1.0;
				for (k = 1; k < historySize - 2; k++) {
					compressedDeviation = myDoubleMax(compressedDeviation, myDoubleMax(historyD[k], historyD[k + 1]) + distanceToLineSection(historyT[k], historyX[k], historyY[k], historyT[0], historyX[0], historyY[0], historyT[historySize - 2], historyX[historySize - 2], historyY[historySize - 2]));
				}
				assert(compressedDeviation >= getMaxDeviationAlongLineSection() && compressedDeviation <= ACCURACY_BOUND);
				assert(historySize >= 2);
				historyT[1] = historyT[historySize - 2];
				historyX[1] = historyX[historySize - 2];
				historyY[1] = historyY[historySize - 2];
				historyD[1] = compressedDeviation;
				historyT[2] = historyT[historySize - 1];
				historyX[2] = historyX[historySize - 1];
				historyY[2] = historyY[historySize - 1];
				historyD[2] = historyD[historySize - 1];
				historySize = 3;
				compressedSize = 1;

				assert(compressedSize <= c + 1 && historySize <= m); // Compressed size may exceed c by one temporarily.
				assert(historySize - compressedSize < m - c);

				if (compressedSize  > c) {
					assert(historySize == 3);
					updateT[updateSize] = historyT[1]; // Add last sensed position before current one as new vertex to update.
					updateX[updateSize] = historyX[1];
					updateY[updateSize] = historyY[1];
					updateSize++;
					historyT[0] = historyT[1];
					historyX[0] = historyX[1];
					historyY[0] = historyY[1];
					historyD[0] = 999999999.0; // historyD[0] must be never used!
					historyT[1] = historyT[2];
					historyX[1] = historyX[2];
					historyY[1] = historyY[2];
					historyD[1] = historyD[2];
					historySize = 2;
					compressedSize = 0;
				}
			}

			if (originalIndex == ORIGINAL_SIZE - 1 || !evalLDRCond(historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1], currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)) {
				assert(historyT[historySize - 2] + 0.9 * SENS_TIME < historyT[historySize - 1] && historyT[historySize - 1] < historyT[historySize - 2] + 1.1 * SENS_TIME);
				predOriginT = historyT[historySize - 1];
				predOriginX = historyX[historySize - 1];
				predOriginY = historyY[historySize - 1];
				predSpeedX = (historyX[historySize - 1] - historyX[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				predSpeedY = (historyY[historySize - 1] - historyY[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				updateT[updateSize] = historyT[historySize - 1]; // Add current position as prediction origin to update.
				updateX[updateSize] = historyX[historySize - 1];
				updateY[updateSize] = historyY[historySize - 1];
				updateSize++;

				maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + 1) * BYTES_STPOINT + BYTES_VECTOR + (historySize - 1) * BYTES_DOUBLE); // Count (historySize - 1) * BYTES_DOUBLE for historyD[] only, since historyD[0] could be removed.

				runGRTS_processUpdate(variableSize, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
				messageCount++;
				payloadSum += BYTES_HEADER + updateSize * BYTES_STPOINT + BYTES_VECTOR; // Transmission of variableSize is not measured as it is always 0 for the first two updates and then 1.
				updateSize = 0;
				variableSize = 1;
			}

			assert(0 <= compressedSize && compressedSize <= 1);
			assert(historySize < m);

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
	/*printf("\n");

	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("GRTSmcSec");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		mySnprintf(paramString, MAX_STRING_LENGTH, "optHistory=%s,m=%d,c=%d", ((optimizedHistory) ? "true" : "false"), m, c);
		//writeResultsDataRTS("GRTSmcSec", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}

	printf("    done.\n");
	*/
}


void runGRTSkOpt(int k, bool optimizedUpdates) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;
	//printf("  Running GRTSkOpt with k=%d and optimizedUpdates=%s:\n", k, ((optimizedUpdates) ? "true" : "false"));

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double predSpeedX, predSpeedY;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		int historySize = 0;
		int predOriginIndex = 0;
		double updateT[MAX_ORIGINAL_SIZE];
		double updateX[MAX_ORIGINAL_SIZE];
		double updateY[MAX_ORIGINAL_SIZE];
		int updateSize = 0;

		// Only used for optimized updates.
		double variableT[MAX_ORIGINAL_SIZE];
		double variableX[MAX_ORIGINAL_SIZE];
		double variableY[MAX_ORIGINAL_SIZE];
		int variableSize = 0;
		int variableMatch = 0;

		int originalIndex = 0;

		short neighbors[MAX_ORIGINAL_SIZE][MAX_GRAPH_NEIGHBORS_RTS];
		int neighborsSizes[MAX_ORIGINAL_SIZE];
		int neighborsSizesSum = 0;

		clearSimplified();
		

		{ // Compute and send initial update.
			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historySize++;

			neighborsSizes[historySize - 1] = 0;

			updateT[updateSize] = historyT[historySize - 1];
			updateX[updateSize] = historyX[historySize - 1];
			updateY[updateSize] = historyY[historySize - 1];
			updateSize++;
			predSpeedX = 0.0;
			predSpeedY = 0.0;

			variableMatch = 0;
			runGRTS_processUpdate(0, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
			messageCount++;
			if (optimizedUpdates) {
				payloadSum += BYTES_HEADER + BYTES_INT + (updateSize - variableMatch) * BYTES_STPOINT + BYTES_VECTOR; // Here, it is variableMatch = 0.
			}
			else {
				// Without the variable-arrays, the shared prefix has be sent as well (here empty) - but integer to indicate the positions to remove.
				payloadSum += BYTES_HEADER + updateSize * BYTES_STPOINT + BYTES_VECTOR;
			}

			updateSize = 0;
		}
		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) { // Compute and send normal updates as well as the final update.
			double currentSpeedX, currentSpeedY;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			{ // Add sensed position to graph and compute edges accordingly.
				int j;
				neighborsSizes[historySize] = 0;

				for (j = 0; j < historySize; j++) {
					int l;
					bool add = TRUE;
					for (l = j + 1; l < historySize; l++) { // It is possible to include "&& add" in the loop-condition. But, regarding computing time, this does not help in the worst-case.
						add = add && (distanceToLineSection(historyT[l], historyX[l], historyY[l], historyT[j], historyX[j], historyY[j], historyT[historySize], historyX[historySize], historyY[historySize]) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
					}
					if (add) {
						neighbors[j][neighborsSizes[j]] = historySize;
						neighborsSizes[j]++;
						neighborsSizesSum++;
						assert(neighborsSizes[j] < MAX_GRAPH_NEIGHBORS_RTS);
					}
				}
			}

			historySize++;

			if (originalIndex == ORIGINAL_SIZE - 1 || !evalLDRCond(historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1], currentSpeedX, currentSpeedY, historyT[predOriginIndex], historyX[predOriginIndex], historyY[predOriginIndex], predSpeedX, predSpeedY)) {
				int previous[MAX_ORIGINAL_SIZE];
				int previousSteps;
				int i;

				for (i = 0; i < historySize; i++) {
					previous[i] = -1;
				}

				{ // Run Dijkstra's algorithm.
					int distances[MAX_ORIGINAL_SIZE];
					bool queued[MAX_ORIGINAL_SIZE];

					for (i = 0; i < historySize; i++) {
						distances[i] = INT_MAX;
						queued[i] = TRUE;
					}

					i = -1;
					distances[0] = 0;
					while (queued[historySize - 1]) {
						int l;
						int distanceMin = INT_MAX;
						for (l = 0; l < historySize; l++) {
							if (queued[l] && distances[l] < distanceMin) {
								distanceMin = distances[l];
								i = l;
							}
						}
						queued[i] = FALSE;
						for (l = 0; l < neighborsSizes[i]; l++) {
							int j = neighbors[i][l];
							if (queued[j]) {
								if (distances[i] + 1 < distances[j]) {
									distances[j] = distances[i] + 1;
									previous[j] = i;
								}
								else if (distances[i] + 1 == distances[j] && previous[j] < i) { // Selects shortest path with largest vertex number before last one.
									previous[j] = i;
								}
							}
						}
					}
					previousSteps = distances[historySize - 1];
				}

				// Store simplified trajectory, where previous-sequence is traversed in reverse order.
				updateSize = 0;
				i = historySize - 1;
				while (previous[i] >= 0) {
					updateSize++;
					updateT[previousSteps - updateSize] = historyT[i];
					updateX[previousSteps - updateSize] = historyX[i];
					updateY[previousSteps - updateSize] = historyY[i];
					i = previous[i];
				}
				assert(i == 0 && updateSize == previousSteps);

				predSpeedX = (historyX[historySize - 1] - historyX[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				predSpeedY = (historyY[historySize - 1] - historyY[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);

				// Determine shared prefix of current variable part and update.
				variableMatch = 0;
				while (variableMatch < variableSize && variableMatch < updateSize && variableT[variableMatch] == updateT[variableMatch]) {
					assert(variableX[variableMatch] == updateX[variableMatch] && variableY[variableMatch] == updateY[variableMatch]);
					variableMatch++;
				}

				// Remove shared prefix from update.
				updateSize = updateSize - variableMatch;
				for (i = 0; i < updateSize; i++) {
					updateT[i] = updateT[variableMatch + i];
					updateX[i] = updateX[variableMatch + i];
					updateY[i] = updateY[variableMatch + i];
				}

				if (optimizedUpdates) {
					maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + variableSize) * BYTES_STPOINT + BYTES_VECTOR + historySize * (3 * BYTES_INT + BYTES_BOOL) + neighborsSizesSum * BYTES_INT);
				}
				else {
					// Without optimized updates, the variable part would not have to be stored explicitly.
					maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize) * BYTES_STPOINT + BYTES_VECTOR + historySize * (3 * BYTES_INT + BYTES_BOOL) + neighborsSizesSum * BYTES_INT);
				}

				runGRTS_processUpdate(variableSize - variableMatch, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
				messageCount++;

				if (optimizedUpdates) {
					payloadSum += BYTES_HEADER + BYTES_INT + updateSize * BYTES_STPOINT + BYTES_VECTOR;
				}
				else {
					// Without the variable-arrays, the shared prefix has be sent as well - but no integer to indicate the positions to remove.
					payloadSum += BYTES_HEADER + (updateSize + variableMatch) * BYTES_STPOINT + BYTES_VECTOR;
				}

				// Store new variable part.
				variableSize = variableMatch + updateSize;
				for (i = 0; i < updateSize; i++) {
					variableT[variableMatch + i] = updateT[i];
					variableX[variableMatch + i] = updateX[i];
					variableY[variableMatch + i] = updateY[i];
				}

				// Shift history and update graph accordingly.
				if (variableSize > k) {
					int shift = historySize - 1;
					for (i = 0; i < k; i++) {
						shift = previous[shift];
					}
					assert(shift > 0);
					historySize = historySize - shift;
					for (i = 0; i < shift; i++) {
						neighborsSizesSum -= neighborsSizes[i];
					}
					for (i = 0; i < historySize; i++) {
						int j;
						historyT[i] = historyT[i + shift];
						historyX[i] = historyX[i + shift];
						historyY[i] = historyY[i + shift];
						neighborsSizes[i] = neighborsSizes[i + shift];
						for (j = 0; j < neighborsSizes[i]; j++) {
							neighbors[i][j] = neighbors[i + shift][j] - shift;
						}
					}

					{
						int removeSize;

						removeSize = 0;
						while (shift > 0) {
							shift = previous[shift];
							removeSize++;
						}
						variableSize = variableSize - removeSize;
						for (i = 0; i < variableSize; i++) {
							variableT[i] = variableT[i + removeSize];
							variableX[i] = variableX[i + removeSize];
							variableY[i] = variableY[i + removeSize];
						}
					}
				}

				updateSize = 0;
				predOriginIndex = historySize - 1;
			}

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
//	printf("\n");
/*

	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("GRTSkOpt");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		mySnprintf(paramString, MAX_STRING_LENGTH, "k=%d,optUpdates=%s", k, ((optimizedUpdates) ? "true" : "false"));
		//writeResultsDataRTS("GRTSkOpt", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}

	printf("    done.\n");
 */
}


void runGRTSmOpt(int m) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;

	//printf("  Running GRTSmOpt with m=%d:\n", m);

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		int historySize = 0;

		double updateT[MAX_ORIGINAL_SIZE];
		double updateX[MAX_ORIGINAL_SIZE];
		double updateY[MAX_ORIGINAL_SIZE];
		int updateSize = 0;

		double variableT[MAX_ORIGINAL_SIZE];
		double variableX[MAX_ORIGINAL_SIZE];
		double variableY[MAX_ORIGINAL_SIZE];
		int variableSize = 0;

		int originalIndex = 0;

		short neighbors[MAX_ORIGINAL_SIZE][MAX_GRAPH_NEIGHBORS_RTS];
		int neighborsSizes[MAX_ORIGINAL_SIZE];
		int neighborsSizesSum = 0;

		clearSimplified();

		{ // Compute and send initial update.
			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historySize++;

			neighborsSizes[historySize - 1] = 0;

			updateT[updateSize] = historyT[historySize - 1];
			updateX[updateSize] = historyX[historySize - 1];
			updateY[updateSize] = historyY[historySize - 1];
			updateSize++;
			predOriginT = historyT[0];
			predOriginX = historyX[0];
			predOriginY = historyY[0];
			predSpeedX = 0.0;
			predSpeedY = 0.0;

			variableSize = 0;
			runGRTS_processUpdate(0, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
			messageCount++;
			payloadSum += BYTES_HEADER + BYTES_INT + updateSize * BYTES_STPOINT + BYTES_VECTOR;

			updateSize = 0;
		}

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) {  // Compute and send normal updates as well as the final update.
			double currentSpeedX, currentSpeedY;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			{ // Add sensed position to graph and compute edges accordingly.
				int j;
				neighborsSizes[historySize] = 0;

				for (j = 0; j < historySize; j++) {
					int l;
					bool add = TRUE;
					for (l = j + 1; l < historySize; l++) { // It is possible to include "&& add" in the loop-condition. But, regarding computing time, this does not help in the worst-case.
						add = add && (distanceToLineSection(historyT[l], historyX[l], historyY[l], historyT[j], historyX[j], historyY[j], historyT[historySize], historyX[historySize], historyY[historySize]) + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
					}
					if (add) {
						neighbors[j][neighborsSizes[j]] = historySize;
						neighborsSizes[j]++;
						neighborsSizesSum++;
						assert(neighborsSizes[j] < MAX_GRAPH_NEIGHBORS_RTS);
					}
				}
				historySize++;
			}

			maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + variableSize) * BYTES_STPOINT + BYTES_VECTOR + historySize * (3 * BYTES_INT + BYTES_BOOL) + neighborsSizesSum * BYTES_INT);

			// Reduce (shift) history if required and update graph accordingly. Add reduced vertex of simplification to update taking into account the variable part known to the MOD.
			if (historySize == m) {
				int previous[MAX_ORIGINAL_SIZE];
				int i;

				for (i = 0; i < historySize; i++) {
					previous[i] = -1;
				}

				{ // Run Dijkstra's algorithm.
					int distances[MAX_ORIGINAL_SIZE];
					bool queued[MAX_ORIGINAL_SIZE];

					for (i = 0; i < historySize; i++) {
						distances[i] = INT_MAX;
						queued[i] = TRUE;
					}

					i = -1;
					distances[0] = 0;
					while (queued[historySize - 1]) {
						int l;
						int distanceMin = INT_MAX;
						for (l = 0; l < historySize; l++) {
							if (queued[l] && distances[l] < distanceMin) {
								distanceMin = distances[l];
								i = l;
							}
						}
						queued[i] = FALSE;
						for (l = 0; l < neighborsSizes[i]; l++) {
							int j = neighbors[i][l];
							if (queued[j]) {
								if (distances[i] + 1 < distances[j]) {
									distances[j] = distances[i] + 1;
									previous[j] = i;
								}
								else if (distances[i] + 1 == distances[j] && previous[j] < i) { // Selects shortest path with largest vertex number before last one.
									previous[j] = i;
								}
							}
						}
					}
				}

				{ // Add second vertex of simplification to update and reduce history by one edge/vertex of simplification.
					int historyShift = historySize - 1;
					while (previous[historyShift] > 0) {
						historyShift = previous[historyShift];
					}
					assert(historyShift > 0 && previous[historyShift] == 0);

					updateT[updateSize] = historyT[historyShift];
					updateX[updateSize] = historyX[historyShift];
					updateY[updateSize] = historyY[historyShift];
					updateSize++;

					for (i = 0; i < historyShift; i++) {
						neighborsSizesSum -= neighborsSizes[i];
					}
					historySize = historySize - historyShift;
					for (i = 0; i < historySize; i++) {
						int j;
						historyT[i] = historyT[i + historyShift];
						historyX[i] = historyX[i + historyShift];
						historyY[i] = historyY[i + historyShift];
						neighborsSizes[i] = neighborsSizes[i + historyShift];
						for (j = 0; j < neighborsSizes[i]; j++) {
							neighbors[i][j] = neighbors[i + historyShift][j] - historyShift;
						}
					}
				}
			}

			assert(historySize < m);

			if (originalIndex == ORIGINAL_SIZE - 1 || !evalLDRCond(historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1], currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)) {
				int variableMatch = 0;
				int previous[MAX_ORIGINAL_SIZE];
				int i;
				int previousSteps;

				for (i = 0; i < historySize; i++) {
					previous[i] = -1;
				}

				{ // Run Dijkstra's algorithm.
					int distances[MAX_ORIGINAL_SIZE];
					bool queued[MAX_ORIGINAL_SIZE];

					for (i = 0; i < historySize; i++) {
						distances[i] = INT_MAX;
						queued[i] = TRUE;
					}

					i = -1;
					distances[0] = 0;
					while (queued[historySize - 1]) {
						int l;
						int distanceMin = INT_MAX;
						for (l = 0; l < historySize; l++) {
							if (queued[l] && distances[l] < distanceMin) {
								distanceMin = distances[l];
								i = l;
							}
						}
						queued[i] = FALSE;
						for (l = 0; l < neighborsSizes[i]; l++) {
							int j = neighbors[i][l];
							if (queued[j]) {
								if (distances[i] + 1 < distances[j]) {
									distances[j] = distances[i] + 1;
									previous[j] = i;
								}
								else if (distances[i] + 1 == distances[j] && previous[j] < i) { // Selects shortest path with largest vertex number before last one.
									previous[j] = i;
								}
							}
						}
					}
					previousSteps = distances[historySize - 1];
				}

				{ // Store simplified trajectory, where previous-sequence is traversed in reverse order.
					int j = 0;
					i = historySize - 1;
					while (previous[i] >= 0) {
						j++;
						updateT[updateSize + previousSteps - j] = historyT[i];
						updateX[updateSize + previousSteps - j] = historyX[i];
						updateY[updateSize + previousSteps - j] = historyY[i];
						i = previous[i];
					}
					assert(i == 0 && j == previousSteps);
					updateSize += j;
				}

				predOriginT = historyT[historySize - 1];
				predOriginX = historyX[historySize - 1];
				predOriginY = historyY[historySize - 1];
				predSpeedX = (historyX[historySize - 1] - historyX[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				predSpeedY = (historyY[historySize - 1] - historyY[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);

				assert(updateSize > 0 && updateT[updateSize - 1] == predOriginT);
				// Determine shared prefix of current variable part and update.
				variableMatch = 0;
				while (variableMatch < variableSize && variableMatch < updateSize && variableT[variableMatch] == updateT[variableMatch]) {
					assert(variableX[variableMatch] == updateX[variableMatch] && variableY[variableMatch] == updateY[variableMatch]);
					variableMatch++;
				}

				maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + variableSize) * BYTES_STPOINT + BYTES_VECTOR + historySize * (3 * BYTES_INT + BYTES_BOOL) + neighborsSizesSum * BYTES_INT);

				// Remove shared prefix from update.
				updateSize = updateSize - variableMatch;
				for (i = 0; i < updateSize; i++) {
					updateT[i] = updateT[variableMatch + i];
					updateX[i] = updateX[variableMatch + i];
					updateY[i] = updateY[variableMatch + i];
				}

				runGRTS_processUpdate(variableSize - variableMatch, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
				messageCount++;
				payloadSum += BYTES_HEADER + BYTES_INT + updateSize * BYTES_STPOINT + BYTES_VECTOR;

				// Store new variable part.
				variableSize = variableMatch + updateSize;
				for (i = 0; i < updateSize; i++) {
					variableT[variableMatch + i] = updateT[i];
					variableX[variableMatch + i] = updateX[i];
					variableY[variableMatch + i] = updateY[i];
				}

				{ // Reduce variable part with respect to current history reduced previously.
					int reduce = 0;
					while (reduce < variableSize && variableT[reduce] <= historyT[0]) {
						reduce++;
					}
					variableSize -= reduce;
					for (i = 0; i < variableSize; i++) {
						variableT[i] = variableT[i + reduce];
						variableX[i] = variableX[i + reduce];
						variableY[i] = variableY[i + reduce];
					}
					assert(variableSize == 0 || (variableSize > 0 && variableT[variableSize - 1] > historyT[0]));
				}

				updateSize = 0;
			}

			assert(historySize < m);

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
 /*
	printf("\n");

	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("GRTSmOpt");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		//mySnprintf(paramString, MAX_STRING_LENGTH, "m=%d", m);
		//writeResultsDataRTS("GRTSmOpt", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}

	//printf("    done.\n");
 */
}


void runGRTSmcOpt(int m, int c) {
	unsigned long long processorTicks[MAX_ORIGINAL_SIZE - 1][REPETITIONS];
	unsigned long long maxProcessorTicks = 0UL;
	int maxMemoryConsumption = 0;
	int repetition;
	int messageCount = 0;
	int payloadSum = 0;

	assert(c >= 0 && m >= c + 2);

	//printf("  Running GRTSmcOpt with m=%d and c=%d:\n", m, c);

	//printf("    %i repetitions:", REPETITIONS);
	for (repetition = 0; repetition < REPETITIONS; repetition++) {
		double predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY;
		double historyT[MAX_ORIGINAL_SIZE];
		double historyX[MAX_ORIGINAL_SIZE];
		double historyY[MAX_ORIGINAL_SIZE];
		double historyD[MAX_ORIGINAL_SIZE];
		int historySize = 0;
		int compressedSize = 0; // Note, that history[compressedSize] is the latest compressed position as whether history[0] is compressed or not is irrelevant.

		double updateT[MAX_ORIGINAL_SIZE];
		double updateX[MAX_ORIGINAL_SIZE];
		double updateY[MAX_ORIGINAL_SIZE];
		int updateSize = 0;

		double variableT[MAX_ORIGINAL_SIZE];
		double variableX[MAX_ORIGINAL_SIZE];
		double variableY[MAX_ORIGINAL_SIZE];
		int variableSize = 0;

		int originalIndex = 0;

		short neighbors[MAX_ORIGINAL_SIZE][MAX_GRAPH_NEIGHBORS_RTS];
		double neighborsD[MAX_ORIGINAL_SIZE][MAX_GRAPH_NEIGHBORS_RTS];
		int neighborsSizes[MAX_ORIGINAL_SIZE];
		int neighborsSizesSum = 0;

		clearSimplified();

		{ // Compute and send initial update.
			historyT[0] = ORIGINAL_T[originalIndex];
			historyX[0] = ORIGINAL_X[originalIndex];
			historyY[0] = ORIGINAL_Y[originalIndex];
			historyD[0] = 999999999.0; // historyD[0] must be never used!
			historySize = 1;

			neighborsSizes[0] = 0;

			updateT[updateSize] = historyT[0];
			updateX[updateSize] = historyX[0];
			updateY[updateSize] = historyY[0];
			updateSize++;
			predOriginT = historyT[0];
			predOriginX = historyX[0];
			predOriginY = historyY[0];
			predSpeedX = 0.0;
			predSpeedY = 0.0;

			variableSize = 0;
			runGRTS_processUpdate(0, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
			messageCount++;
			payloadSum += BYTES_HEADER + BYTES_INT + updateSize * BYTES_STPOINT + BYTES_VECTOR;

			updateSize = 0;
		}

		for (originalIndex = 1; originalIndex < ORIGINAL_SIZE; originalIndex++) { // Compute and send normal updates as well as the final update.
			double currentSpeedX, currentSpeedY;
			unsigned long long processorTicksStart, processorTicksEnd;
			processorTicksStart = getProcessorTicks();

			historyT[historySize] = ORIGINAL_T[originalIndex];
			historyX[historySize] = ORIGINAL_X[originalIndex];
			historyY[historySize] = ORIGINAL_Y[originalIndex];
			historyD[historySize] = getMaxDeviationAlongLineSection(); // Maximum deviation along line section between two consecutive sensed positions given by sensing inaccuracy and movement between fixes.
			if (originalIndex == 0) {
				currentSpeedX = 0.0;
				currentSpeedY = 0.0;
			}
			else {
				currentSpeedX = (ORIGINAL_X[originalIndex] - ORIGINAL_X[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
				currentSpeedY = (ORIGINAL_Y[originalIndex] - ORIGINAL_Y[originalIndex - 1]) / (ORIGINAL_T[originalIndex] - ORIGINAL_T[originalIndex - 1]);
			}

			{ // Add sensed position to graph and compute edges accordingly.
				int j;
				neighborsSizes[historySize] = 0;

				for (j = 0; j < historySize; j++) {
					int l;
					double deviation;

					deviation = historyD[historySize]; // "Check" deviation at l = historySize.
					for (l = j + 1; l < historySize; l++) { // Check deviations at l in [j + 1, historySize - 1] which implicitly also checks the deviation at l = j.
						deviation = myDoubleMax(deviation, myDoubleMax(historyD[l], historyD[l + 1]) + distanceToLineSection(historyT[l], historyX[l], historyY[l], historyT[j], historyX[j], historyY[j], historyT[historySize], historyX[historySize], historyY[historySize]));
					}
					assert(deviation >= getMaxDeviationAlongLineSection());
					if (deviation <= ACCURACY_BOUND) {
						neighbors[j][neighborsSizes[j]] = historySize;
						neighborsD[j][neighborsSizes[j]] = deviation;
						neighborsSizes[j]++;
						neighborsSizesSum++;
						assert(neighborsSizes[j] < MAX_GRAPH_NEIGHBORS_RTS);
					}
				}

				historySize++;
			}

			maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + variableSize) * BYTES_STPOINT + BYTES_VECTOR + historySize * (3 * BYTES_INT + BYTES_BOOL) + neighborsSizesSum * (BYTES_INT + BYTES_DOUBLE) + (historySize - 1) * BYTES_DOUBLE); // Count (historySize - 1) * BYTES_DOUBLE for historyD[] only, since historyD[0] could be removed.


			assert(compressedSize <= c && historySize <= m);
			assert(historySize - compressedSize <= m - c);

			// If neccessary, compress a part of the history.
			if (historySize - compressedSize == m - c) {
				int i;
				int previous[MAX_ORIGINAL_SIZE];

				for (i = 0; i < historySize; i++) {
					previous[i] = -1;
				}

				{ // Run Dijkstra's algorithm.
					int distances[MAX_ORIGINAL_SIZE];
					bool queued[MAX_ORIGINAL_SIZE];

					for (i = 0; i < historySize; i++) {
						distances[i] = INT_MAX;
						queued[i] = TRUE;
					}

					distances[0] = 0;
					while (queued[historySize - 1]) {
						int l;
						int distanceMin = INT_MAX;
						for (l = 0; l < historySize; l++) {
							if (queued[l] && distances[l] < distanceMin) {
								distanceMin = distances[l];
								i = l;
							}
						}
						queued[i] = FALSE;
						for (l = 0; l < neighborsSizes[i]; l++) {
							int j = neighbors[i][l];
							if (queued[j]) {
								if (distances[i] + 1 < distances[j]) {
									distances[j] = distances[i] + 1;
									previous[j] = i;
								}
								else if (distances[i] + 1 == distances[j] && previous[j] < i) { // Selects shortest path with largest vertex number before last one.
									previous[j] = i;
								}
							}
						}
					}
				}

				assert(historySize > compressedSize); // There must be at least one not-compressed position, namely the one just added.

				{ // Determine the one edge of the optimal solution going from a compressed position to a normal position and remove history in-between.
					int shift;
					int removeSize;
					double compressedDeviation;

					shift = historySize - 1;
					while (previous[shift] > compressedSize) {
						shift = previous[shift];
					}
					assert(shift > compressedSize && previous[shift] <= compressedSize);
					{ // Verify the invariant that all compressed positions before previous[shift] belong to the shortest path.
						int j = previous[shift];
						while (j > 0) {
							assert(previous[j] == j - 1);
							j--;
						}
						assert(j == 0);
					}

					compressedDeviation = -1.0;
					{ // Search deviation and store it.
						int j;
						for (j = 0; j < neighborsSizes[previous[shift]]; j++) {
							if (neighbors[previous[shift]][j] == shift) {
								assert(compressedDeviation == -1.0); // Compressed deviation must be set once only.
								compressedDeviation = neighborsD[previous[shift]][j];
								historyD[shift] = compressedDeviation; // Shift is > 0, see above. Thus, historyD[0] remains unused.
							}
						}
					}
					assert(compressedDeviation >= getMaxDeviationAlongLineSection() && compressedDeviation <= ACCURACY_BOUND);

					removeSize = shift - (previous[shift] + 1);
					assert(removeSize >= 0); // In rare cases, the history size is not reduced here but then the compressed size is increased below.

					historySize -= removeSize;
					for (i = 0; i <= previous[shift]; i++) {
						int j;
						for (j = 0; j < neighborsSizes[i]; j++) {
							if (neighbors[i][j] > previous[shift] && neighbors[i][j] < shift) { // This edge becomes invalid. Remove it.
								int l;
								neighborsSizes[i]--;
								neighborsSizesSum--;
								for (l = j; l < neighborsSizes[i]; l++) {
									neighbors[i][l] = neighbors[i][l + 1];
									neighborsD[i][l] = neighborsD[i][l + 1];
								}
								j--;
							}
							else if (neighbors[i][j] >= shift) {
								neighbors[i][j] = neighbors[i][j] - removeSize;
								// neighborsD[i][j] does not change.
							}
						}
					}
					for (i = 0; i < removeSize; i++) {
						neighborsSizesSum -= neighborsSizes[previous[shift] + 1 + i];
					}
					for (i = previous[shift] + 1; i < historySize; i++) {
						int j;
						historyT[i] = historyT[i + removeSize];
						historyX[i] = historyX[i + removeSize];
						historyY[i] = historyY[i + removeSize];
						historyD[i] = historyD[i + removeSize];
						neighborsSizes[i] = neighborsSizes[i + removeSize];
						for (j = 0; j < neighborsSizes[i]; j++) {
							neighbors[i][j] = neighbors[i + removeSize][j] - removeSize;
							neighborsD[i][j] = neighborsD[i + removeSize][j];
						}
					}

					assert(historyD[previous[shift] + 1] == compressedDeviation);
					assert(previous[shift] + 1 <= compressedSize + 1); // Compressed size must not increase by more than one. It may even decrease.
					compressedSize = previous[shift] + 1;
				}

				assert(compressedSize <= c + 1 && historySize <= m); // Compressed size may exceed c by one temporarily.
				assert(historySize - compressedSize < m - c);

				if (compressedSize  > c) { // Add second vertex of simplification to update and reduce history by one compressed position.
					assert(historySize >= 2); // It holds c >= 0, thus compressedSize >= 1, and thus history size >= 2 as history[compressedSize] is the latest compressed position.

					updateT[updateSize] = historyT[1];
					updateX[updateSize] = historyX[1];
					updateY[updateSize] = historyY[1];
					updateSize++;

					neighborsSizesSum -= neighborsSizes[0];
					historySize--;
					for (i = 0; i < historySize; i++) {
						int j;
						historyT[i] = historyT[i + 1];
						historyX[i] = historyX[i + 1];
						historyY[i] = historyY[i + 1];
						historyD[i] = historyD[i + 1];
						neighborsSizes[i] = neighborsSizes[i + 1];
						for (j = 0; j < neighborsSizes[i]; j++) {
							neighbors[i][j] = neighbors[i + 1][j] - 1;
							neighborsD[i][j] = neighborsD[i + 1][j];
						}
					}
					historyD[0] = 999999999.0; // historyD[0] must be never used!
					compressedSize--;
				}
			}

			assert(compressedSize <= c && historySize < m);

			if (originalIndex == ORIGINAL_SIZE - 1 || !evalLDRCond(historyT[historySize - 1], historyX[historySize - 1], historyY[historySize - 1], currentSpeedX, currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)) {
				int previous[MAX_ORIGINAL_SIZE];
				int variableMatch = 0;
				int i;
				int previousSteps;

				for (i = 0; i < historySize; i++) {
					previous[i] = -1;
				}

				{ // Run Dijkstra's algorithm.
					int distances[MAX_ORIGINAL_SIZE];
					bool queued[MAX_ORIGINAL_SIZE];

					for (i = 0; i < historySize; i++) {
						distances[i] = INT_MAX;
						queued[i] = TRUE;
					}

					distances[0] = 0;
					while (queued[historySize - 1]) {
						int l;
						int distanceMin = INT_MAX;
						for (l = 0; l < historySize; l++) {
							if (queued[l] && distances[l] < distanceMin) {
								distanceMin = distances[l];
								i = l;
							}
						}
						queued[i] = FALSE;
						for (l = 0; l < neighborsSizes[i]; l++) {
							int j = neighbors[i][l];
							if (queued[j]) {
								if (distances[i] + 1 < distances[j]) {
									distances[j] = distances[i] + 1;
									previous[j] = i;
								}
								else if (distances[i] + 1 == distances[j] && previous[j] < i) { // Selects shortest path with largest vertex number before last one.
									previous[j] = i;
								}
							}
						}
					}
					previousSteps = distances[historySize - 1];
				}

				{ // Store simplified trajectory, where previous-sequence is traversed in reverse order.
					int j = 0;
					i = historySize - 1;
					while (previous[i] >= 0) {
						j++;
						updateT[updateSize + previousSteps - j] = historyT[i];
						updateX[updateSize + previousSteps - j] = historyX[i];
						updateY[updateSize + previousSteps - j] = historyY[i];
						i = previous[i];
					}
					assert(i == 0 && j == previousSteps);
					updateSize += j;
				}


				predOriginT = historyT[historySize - 1];
				predOriginX = historyX[historySize - 1];
				predOriginY = historyY[historySize - 1];
				predSpeedX = (historyX[historySize - 1] - historyX[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);
				predSpeedY = (historyY[historySize - 1] - historyY[historySize - 2]) / (historyT[historySize - 1] - historyT[historySize - 2]);

				assert(updateSize > 0 && updateT[updateSize - 1] == predOriginT);
				// Determine shared prefix of current variable part and update.
				variableMatch = 0;
				while (variableMatch < variableSize && variableMatch < updateSize && variableT[variableMatch] == updateT[variableMatch]) {
					assert(variableX[variableMatch] == updateX[variableMatch] && variableY[variableMatch] == updateY[variableMatch]);
					variableMatch++;
				}

				maxMemoryConsumption = myIntMax(maxMemoryConsumption, (historySize + updateSize + variableSize) * BYTES_STPOINT + BYTES_VECTOR + historySize * (3 * BYTES_INT + BYTES_BOOL) + neighborsSizesSum * (BYTES_INT + BYTES_DOUBLE) + (historySize - 1) * BYTES_DOUBLE); // Count (historySize - 1) * BYTES_DOUBLE for historyD[] only, since historyD[0] could be removed.

				// Remove shared prefix from update.
				updateSize = updateSize - variableMatch;
				for (i = 0; i < updateSize; i++) {
					updateT[i] = updateT[variableMatch + i];
					updateX[i] = updateX[variableMatch + i];
					updateY[i] = updateY[variableMatch + i];
				}

				runGRTS_processUpdate(variableSize - variableMatch, updateT, updateX, updateY, updateSize, predSpeedX, predSpeedY);
				messageCount++;
				payloadSum += BYTES_HEADER + BYTES_INT + updateSize * BYTES_STPOINT + BYTES_VECTOR;

				// Store new variable part.
				variableSize = variableMatch + updateSize;
				for (i = 0; i < updateSize; i++) {
					variableT[variableMatch + i] = updateT[i];
					variableX[variableMatch + i] = updateX[i];
					variableY[variableMatch + i] = updateY[i];
				}

				{ // Reduce variable part with respect to current history reduced previously.
					int reduce = 0;
					while (reduce < variableSize && variableT[reduce] <= historyT[0]) {
						reduce++;
					}
					variableSize -= reduce;
					for (i = 0; i < variableSize; i++) {
						variableT[i] = variableT[i + reduce];
						variableX[i] = variableX[i + reduce];
						variableY[i] = variableY[i + reduce];
					}
					assert(variableSize == 0 || (variableSize > 0 && variableT[variableSize - 1] > historyT[0]));
				}

				updateSize = 0;
			}

			assert(compressedSize <= c && historySize < m);
			assert(historySize - compressedSize < m - c);

			processorTicksEnd = getProcessorTicks();
			processorTicks[originalIndex - 1][repetition] = processorTicksEnd - processorTicksStart;
		}

		//myMSleep(REPETITION_SLEEP_MILLIS);
		//printf(" %i", (repetition + 1));
	}
	//printf("\n");
        /*
	{ // Check and print results.
		int originalIndex;
		char paramString[MAX_STRING_LENGTH];

		checkSimplified("GRTSmcOpt");

		for (originalIndex = 0; originalIndex < ORIGINAL_SIZE - 1; originalIndex++) {
			int i, j;
			for (i = 0; i < REPETITIONS; i++) {
				for (j = 1; j < REPETITIONS; j++) {
					if (processorTicks[originalIndex][j - 1] > processorTicks[originalIndex][j]) {
						unsigned long long t = processorTicks[originalIndex][j - 1];
						processorTicks[originalIndex][j - 1] = processorTicks[originalIndex][j];
						processorTicks[originalIndex][j] = t;
					}
				}
			}
			maxProcessorTicks = myUnsignedLongLongMax(maxProcessorTicks, processorTicks[originalIndex][0]);
		}

		assert(messageCount % REPETITIONS == 0);
		messageCount = messageCount / REPETITIONS;
		assert(payloadSum % REPETITIONS == 0);
		payloadSum = payloadSum / REPETITIONS;

		mySnprintf(paramString, MAX_STRING_LENGTH, "m=%d,c=%d", m, c);
		//writeResultsDataRTS("GRTSmcOpt", paramString, messageCount, payloadSum, maxMemoryConsumption, maxProcessorTicks);
	}
         */
	//printf("    done.\n");
}


__my_inline void runGRTS_processUpdate(int r, double* updateT, double* updateX, double* updateY, int updateSize, double predSpeedX, double predSpeedY) {
	if (simplifiedSize == 0) { // Inital update.
		assert(updateSize == 1);
		simplifiedT[simplifiedSize] = updateT[0];
		simplifiedX[simplifiedSize] = updateX[0];
		simplifiedY[simplifiedSize] = updateY[0];
		simplifiedSize = 1;
	}
	else { // Normal update.
		int i;
		assert(r < simplifiedSize);
		simplifiedSize = simplifiedSize - r;
		for (i = 0; i < updateSize; i++) {
			assert(updateT[i] > simplifiedT[simplifiedSize - 1]);
			simplifiedT[simplifiedSize] = updateT[i];
			simplifiedX[simplifiedSize] = updateX[i];
			simplifiedY[simplifiedSize] = updateY[i];
			simplifiedSize++;
		}
	}
}


__my_inline double interpolate(double t, double t0, double x0, double t1, double x1) {
	//printf("%lf %lf\n",t0,t1);
	assert(t0 < t1);
	assert(t0 <= t && t <= t1);
	return ((t1 - t) * x0 + (t - t0) * x1) / (t1 - t0);
}


__my_inline double distance(double x0, double y0, double x1, double y1) {
	return myHypot(x1 - x0, y1 - y0);
}


__my_inline double distanceToLineSection(double t, double x, double y, double t0, double x0, double y0, double t1, double x1, double y1) {
	return distance(x, y, interpolate(t, t0, x0, t1, x1), interpolate(t, t0, y0, t1, y1));
}


__my_inline double distanceToVector(double t, double x, double y, double t0, double x0, double y0, double xV, double yV) {
	//assert(t >= t0);
	return distance(x, y, x0 + (t - t0) * xV, y0 + (t - t0) * yV);
}


__my_inline double getMaxDeviationAlongLineSection() {
	if (PHYS_CONSTRAINT == PHYS_MAX_SPEED_CONSTRAINT) {
		return MAX_SPEED * (0.5 * SENS_TIME) + SENS_INACC_SYS + SENS_INACC_NOI;
	}
	else if (PHYS_CONSTRAINT == PHYS_MAX_ACCEL_CONSTRAINT) {
		return 0.5 * MAX_ACCEL * (0.5 * SENS_TIME) * (0.5 * SENS_TIME) + SENS_INACC_SYS + SENS_INACC_NOI;
	}
	else {
		myHandleError("Unknown physical constraint.");
		return -1.0; // Never reached.
	}
}


__my_inline bool evalLDRCond(double currentT, double currentX, double currentY, double currentSpeedX, double currentSpeedY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	if (PHYS_CONSTRAINT == PHYS_MAX_SPEED_CONSTRAINT) {
		return (distanceToVector(currentT + SENS_TIME + PROC_COM_TIME, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) // Deviation between predicted position after sensing time and current one.
			+ MAX_SPEED * (SENS_TIME + PROC_COM_TIME) // Possible movement within sensing time and until update is received.
			+ SENS_INACC_SYS + SENS_INACC_NOI // Possible inaccuracy of positioning sensor.
			<= ACCURACY_BOUND);
	}
	else if (PHYS_CONSTRAINT == PHYS_MAX_ACCEL_CONSTRAINT) {
		return (distanceToVector(currentT + SENS_TIME + PROC_COM_TIME, currentX + (SENS_TIME + PROC_COM_TIME) * currentSpeedX, currentY + (SENS_TIME + PROC_COM_TIME) * currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)
			+ 0.5 * MAX_ACCEL * (SENS_TIME + PROC_COM_TIME) * (SENS_TIME + PROC_COM_TIME) // Possible movement away from current velocity vector within sensing time and until update is received.
			+ MAX_ACCEL * (0.5 * SENS_TIME) * (SENS_TIME + PROC_COM_TIME) // Possible deviation from inaccuracy in velocity vector due to interpolation over sensing time.
			+ (2.0 * SENS_INACC_NOI / SENS_TIME) * (SENS_TIME + PROC_COM_TIME) // Possible deviation from inaccuracy in velocity vector due to non-systematic sensing inaccuracy.
			+ SENS_INACC_SYS + SENS_INACC_NOI // Possible deviation from sensing inaccuracy.
			<= ACCURACY_BOUND);
	}
	else {
		myHandleError("Unknown physical constraint.");
		return FALSE; // Never reached.
	}
}

// This function must be only used by LDRHalf.
__my_inline bool evalLDRCondHalf(bool optCond, double currentT, double currentX, double currentY, double currentSpeedX, double currentSpeedY, double predOriginT, double predOriginX, double predOriginY, double predSpeedX, double predSpeedY) {
	double halfAccuracyBound = -999999999.0;
	if (PHYS_CONSTRAINT == PHYS_MAX_SPEED_CONSTRAINT) {
		halfAccuracyBound = 0.5 * ACCURACY_BOUND;
		if (optCond) {
			halfAccuracyBound = 0.5 * ACCURACY_BOUND + ((SENS_INACC_SYS + SENS_INACC_NOI) + MAX_SPEED * (SENS_TIME + PROC_COM_TIME) - 0.5 * getMaxDeviationAlongLineSection());
			halfAccuracyBound = myDoubleMin(halfAccuracyBound, ACCURACY_BOUND);
		}
		// The actual condition simply is copied from evalLDRCond, except for the right-hand side of the inequation.
		return (distanceToVector(currentT + SENS_TIME + PROC_COM_TIME, currentX, currentY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY) // Deviation between predicted position after sensing time and current one.
			+ MAX_SPEED * (SENS_TIME + PROC_COM_TIME) // Possible movement within sensing time and until update is received.
			+ SENS_INACC_SYS + SENS_INACC_NOI // Possible inaccuracy of positioning sensor.
			<= halfAccuracyBound);
	}
	else if (PHYS_CONSTRAINT == PHYS_MAX_ACCEL_CONSTRAINT) {
		halfAccuracyBound = 0.5 * ACCURACY_BOUND;
		if (optCond) {
			halfAccuracyBound = 0.5 * ACCURACY_BOUND + ((SENS_INACC_SYS + SENS_INACC_NOI) + (0.5 * MAX_ACCEL * (SENS_TIME + PROC_COM_TIME) * (SENS_TIME + PROC_COM_TIME) + MAX_ACCEL * (0.5 * SENS_TIME) * (SENS_TIME + PROC_COM_TIME) + (2.0 * SENS_INACC_NOI / SENS_TIME) * (SENS_TIME + PROC_COM_TIME)) - 0.5 * getMaxDeviationAlongLineSection());
			halfAccuracyBound = myDoubleMin(halfAccuracyBound, ACCURACY_BOUND);
		}
		// The actual condition simply is copied from evalLDRCond, except for the right-hand side of the inequation.
		return (distanceToVector(currentT + SENS_TIME + PROC_COM_TIME, currentX + (SENS_TIME + PROC_COM_TIME) * currentSpeedX, currentY + (SENS_TIME + PROC_COM_TIME) * currentSpeedY, predOriginT, predOriginX, predOriginY, predSpeedX, predSpeedY)
			+ 0.5 * MAX_ACCEL * (SENS_TIME + PROC_COM_TIME) * (SENS_TIME + PROC_COM_TIME) // Possible movement away from current velocity vector within sensing time and until update is received.
			+ MAX_ACCEL * (0.5 * SENS_TIME) * (SENS_TIME + PROC_COM_TIME) // Possible deviation from inaccuracy in velocity vector due to interpolation over sensing time.
			+ (2.0 * SENS_INACC_NOI / SENS_TIME) * (SENS_TIME + PROC_COM_TIME) // Possible deviation from inaccuracy in velocity vector due to non-systematic sensing inaccuracy.
			+ SENS_INACC_SYS + SENS_INACC_NOI // Possible deviation from sensing inaccuracy.
			<= halfAccuracyBound);
	}
	else {
		myHandleError("Unknown physical constraint.");
		return FALSE; // Never reached.
	}
}


void clearSimplified() {
	int i;
	for (i = 0; i < MAX_ORIGINAL_SIZE; i++) {
		simplifiedT[i] = -999999999.0;
		simplifiedX[i] = -999999999.0;
		simplifiedY[i] = -999999999.0;
	}
	simplifiedSize = 0;
}


void checkSimplified(char* policy) {
	int j;
	int i;
	double maxDeviationFound;
	bool okay;

	assert(simplifiedSize >= 2);

	okay = TRUE;

	okay = okay && (simplifiedSize >= 2);
	okay = okay && (simplifiedT[0] == ORIGINAL_T[0]);
	okay = okay && (simplifiedX[0] == ORIGINAL_X[0]);
	okay = okay && (simplifiedY[0] == ORIGINAL_Y[0]);
	okay = okay && (simplifiedT[simplifiedSize - 1] == ORIGINAL_T[ORIGINAL_SIZE - 1]);
	okay = okay && (simplifiedX[simplifiedSize - 1] == ORIGINAL_X[ORIGINAL_SIZE - 1]);
	okay = okay && (simplifiedY[simplifiedSize - 1] == ORIGINAL_Y[ORIGINAL_SIZE - 1]);

	j = 0;
	maxDeviationFound = 0.0;
	for (i = 0; i < ORIGINAL_SIZE; i++) {
		if (ORIGINAL_T[i] == simplifiedT[j]) {
			okay = okay && (ORIGINAL_X[i] == simplifiedX[j]);
			okay = okay && (ORIGINAL_Y[i] == simplifiedY[j]);
			j++;
			if (j < simplifiedSize) {
				okay = okay && (simplifiedT[j - 1] < simplifiedT[j]);
			}
		}
		else {
			double deviationFound;

			deviationFound = distanceToLineSection(ORIGINAL_T[i], ORIGINAL_X[i], ORIGINAL_Y[i], simplifiedT[j - 1], simplifiedX[j - 1], simplifiedY[j - 1], simplifiedT[j], simplifiedX[j], simplifiedY[j]);
			okay = okay && (deviationFound + getMaxDeviationAlongLineSection() <= ACCURACY_BOUND);
			maxDeviationFound = myDoubleMax(maxDeviationFound, deviationFound);
		}
	}
	okay = okay && (j == simplifiedSize);
	if (!okay) {
		char message[MAX_STRING_LENGTH];
		mySnprintf(message, MAX_STRING_LENGTH, "Simplified trajectory of policy %s exceeds original one by up to %f.", policy, maxDeviationFound);
		myHandleError(message);
	}
}


void printTrajectory(double* t, double* x, double* y, int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("t[%d]=%f x[%d]=%f y[%d]=%f\n", i, t[i], i, x[i], i, y[i]);
	}
}


void writeResultsHeader() {
	char buffer[MAX_STRING_LENGTH];
	mySnprintf(buffer, MAX_STRING_LENGTH, "#%30s %7s %11s %11s %9s %5s %6s %9s %8s %9s %33s %6s %7s %9s %10s %11s\n", "Trajectory", "Sens.", "Sens.Inacc.", "Sens.Inacc.", "Proc.Com.", "Max.", "Max.", "Phys.", "Accuracy", "Algorithm", "Parameters", "Simpl.", "Message", "Payload", "Memory", "Processor");
	writeResultsBytes(buffer);
	mySnprintf(buffer, MAX_STRING_LENGTH, "#%30s %7s %11s %11s %9s %5s %6s %9s %8s %9s %33s %6s %7s %9s %10s %11s\n", "File", "Time", "Systematic", "Noise", "Time", "Speed", "Accel.", "Constr.", "Bound", "", "", "Size", "Count", "Sum", "Consumpt.", "Ticks");
	writeResultsBytes(buffer);
}


void writeResultsDataOTS(char* algorithm, char* params) {
	char buffer[MAX_STRING_LENGTH];
	assert(strnlen(TRAJECTORY_FILE, MAX_STRING_LENGTH) <= 30);
	assert(strnlen(algorithm, MAX_STRING_LENGTH) <= 11);
	assert(strnlen(params, MAX_STRING_LENGTH) <= 33);
	if (PHYS_CONSTRAINT == PHYS_MAX_SPEED_CONSTRAINT) {
		mySnprintf(buffer, MAX_STRING_LENGTH, " %30s %7.2f %11.1f %11.1f %9.3f %5.1f %6s %9s %8.2f %9s %33s %6d %7s %9s %10s %11s\n", TRAJECTORY_FILE, SENS_TIME, SENS_INACC_SYS, SENS_INACC_NOI, PROC_COM_TIME, MAX_SPEED, "n/a", "max_speed", ACCURACY_BOUND, algorithm, params, simplifiedSize, "n/a", "n/a", "n/a", "n/a");
	}
	else if (PHYS_CONSTRAINT == PHYS_MAX_ACCEL_CONSTRAINT) {
		mySnprintf(buffer, MAX_STRING_LENGTH, " %30s %7.2f %11.1f %11.1f %9.3f %5s %6.1f %9s %8.2f %9s %33s %6d %7s %9s %10s %11s\n", TRAJECTORY_FILE, SENS_TIME, SENS_INACC_SYS, SENS_INACC_NOI, PROC_COM_TIME, "n/a", MAX_ACCEL, "max_accel", ACCURACY_BOUND, algorithm, params, simplifiedSize, "n/a", "n/a", "n/a", "n/a");
	}
	else {
		myHandleError("Unknown physical constraint.");
	}
	writeResultsBytes(buffer);
}


void writeResultsDataRTS(char* algorithm, char* params, int messageCount, int payloadSum, int memoryConsumption, unsigned long long processorTicks) {
	char buffer[MAX_STRING_LENGTH];
	assert(strnlen(TRAJECTORY_FILE, MAX_STRING_LENGTH) <= 30);
	assert(strnlen(algorithm, MAX_STRING_LENGTH) <= 11);
	assert(strnlen(params, MAX_STRING_LENGTH) <= 33);
	if (PHYS_CONSTRAINT == PHYS_MAX_SPEED_CONSTRAINT) {
		mySnprintf(buffer, MAX_STRING_LENGTH, " %30s %7.2f %11.1f %11.1f %9.3f %5.1f %6s %9s %8.2f %9s %33s %6d %7d %9d %10d %11llu\n", TRAJECTORY_FILE, SENS_TIME, SENS_INACC_SYS, SENS_INACC_NOI, PROC_COM_TIME, MAX_SPEED, "n/a", "max_speed", ACCURACY_BOUND, algorithm, params, simplifiedSize, messageCount, payloadSum, memoryConsumption, processorTicks);
	}
	else if (PHYS_CONSTRAINT == PHYS_MAX_ACCEL_CONSTRAINT) {
		mySnprintf(buffer, MAX_STRING_LENGTH, " %30s %7.2f %11.1f %11.1f %9.3f %5s %6.1f %9s %8.2f %9s %33s %6d %7d %9d %10d %11llu\n", TRAJECTORY_FILE, SENS_TIME, SENS_INACC_SYS, SENS_INACC_NOI, PROC_COM_TIME, "n/a", MAX_ACCEL, "max_accel", ACCURACY_BOUND, algorithm, params, simplifiedSize, messageCount, payloadSum, memoryConsumption, processorTicks);
	}
	else {
		myHandleError("Unknown physical constraint.");
	}
	writeResultsBytes(buffer);
}


void writeResultsBytes(char* buffer) {
	int bytesWritten = -1;
	bytesWritten = fprintf(FP_RESULTS, "%s", buffer);
	if (bytesWritten < 0) {
		char message[MAX_STRING_LENGTH];
		mySnprintf(message, MAX_STRING_LENGTH, "Could not write to file due to error %d.", bytesWritten);
		myHandleError(message);
	}
	assert(bytesWritten == strnlen(buffer, MAX_STRING_LENGTH));
}


unsigned long long getProcessorTicksPerSec() {
	unsigned long long processorTicksStart, processorTicksEnd;

	processorTicksStart = getProcessorTicks();
	myMSleep(1000);
	processorTicksEnd = getProcessorTicks();

	return processorTicksEnd - processorTicksStart;
}


__my_inline unsigned long long getProcessorTicks() {
#ifdef __unix__
	unsigned int a, d;
	asm volatile("rdtsc" : "=a" (a), "=d" (d));
	return (((unsigned long long)a) | (((unsigned long long)d) << 32));
#elif __WIN32__ || _MSC_VER
	// See http://www.geisswerks.com/ryan/FAQS/timing.html for details.
	__try
	{
		unsigned long long dest;
		unsigned long long* destP = &dest;
		__asm
		{
			_emit 0xf        // These two bytes form the 'rdtsc' asm instruction,
				_emit 0x31       //  available on Pentium I and later.
				mov esi, destP
				mov[esi], eax    // Lower 32 bits of tsc.
				mov[esi + 4], edx    // Upper 32 bits of tsc.
		}
		return dest;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		myHandleError("Could not read time stamp counter.");
	}
	return -1L;
#endif
}

__my_inline double myDoubleMax(double a, double b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}


__my_inline double myDoubleMin(double a, double b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}


__my_inline int myIntMax(int a, int b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}


__my_inline int myIntMin(int a, int b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}


__my_inline unsigned long long myUnsignedLongLongMax(unsigned long long a, unsigned long long b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}


__my_inline void myMSleep(int millis) {
#ifdef __unix__
	usleep(millis * 1000);
#elif __WIN32__ || _MSC_VER
	Sleep(millis);
#endif
}


__my_inline void myHandleError(char* message) {
	fprintf(stderr, "ERROR: %s\n\n", message);
	exit(EXIT_FAILURE);
}
