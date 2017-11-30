#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include "sys/time.h"
#include<algorithm>
using namespace std;
typedef struct Point{
	double lat;
	double lon;
	double time;
}Point;
vector<Point> points;
void gpsreader(string filename){
	ifstream fin(filename.c_str());
	if (!fin){
		cout << "open file error !" << endl;
		exit(0);
	}
	while (fin){
		string line;
		getline(fin, line);
		stringstream sin(line);
		double lat, lon, time;
		struct Point point;
		sin >> point.lat;
		sin >> point.lon;
		sin >> point.time;
		points.push_back(point);

	}
	points.pop_back();
	return;
}
double cacl_PED(Point s, Point m, Point e){


	double A = e.lon - s.lon;
	double B = s.lat - e.lat;
	double C = e.lat * s.lon - s.lat * e.lon;
	if (A == 0 && B == 0)
		return 0;
	double shortDist = fabs((A * m.lat + B * m.lon + C) / sqrt(A * A + B * B));
	return shortDist;
}
vector<int> DouglasPeucker(int start, int last, double epsilon){

	double dmax = 0;
	int index = start;
	vector<int> recResults;
	for (int i = start + 1; i < last; i++){
		double d = cacl_PED(points[start], points[i], points[last]);
		if (d > dmax){
			index = i;
			dmax = d;
		}
	}
	if (dmax > epsilon){
			vector<int> recResults1 = DouglasPeucker(start, index, epsilon);
			vector<int> recResults2 = DouglasPeucker(index, last, epsilon);
			recResults.insert(recResults.end(), recResults1.begin(), recResults1.end());
			recResults.insert(recResults.end(), recResults2.begin(), recResults2.end());
	}
	else{
			recResults.push_back(start);
			recResults.push_back(last);
		}
	

	return recResults;

}

int main(int argc, char** args){
	string filename = args[1];
	double epsilon = atof(args[2]);
	char* save_filename = args[3];
	gpsreader(filename);
  struct timeval start,end;
	/*
	for (int i = 0; i < points.size(); i++)
		printf("%lf %lf %lf\n",points[i].lat,points[i].lon,points[i].time);
	*/
  gettimeofday(&start, NULL );
	vector<int> cmp_index = DouglasPeucker(0, points.size() - 1, epsilon);
  gettimeofday(&end, NULL );
  long timeuse =1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
	sort(cmp_index.begin(), cmp_index.end());
	cmp_index.erase(unique(cmp_index.begin(), cmp_index.end()), cmp_index.end());
  // save to file
	FILE* s_fp= fopen(save_filename,"w+");
	for (int i = 0; i < cmp_index.size(); i++)
		fprintf(s_fp,"%lf %lf %lf\n",points[cmp_index[i]].lat,points[cmp_index[i]].lon,points[cmp_index[i]].time);
  fprintf(s_fp,"%lf\n",timeuse /1000000.0);
  fclose(s_fp);
}
