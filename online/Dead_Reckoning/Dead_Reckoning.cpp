#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include<algorithm>
#include "sys/time.h"
#include <cmath>
using namespace std;
#define PI 3.1415926
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
	int index = 0;
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
vector<double> cacl_distance(){
	vector<double> distance;
	for (int i = 1; i < points.size(); i++){
		distance.push_back(sqrt(pow(points[i].lat - points[i - 1].lat, 2) + pow(points[i].lon - points[i - 1].lon, 2)));
	}
	return distance;
}

vector<double> cacl_angle(){
	vector<double> angles;
	for (int i = 1; i < points.size(); i++){
		double lat_diff = points[i].lat - points[i - 1].lat;
		double lon_diff = points[i].lon - points[i - 1].lon;
		angles.push_back(atan2(lon_diff, lat_diff));
	}
	return angles;
}
vector<int> dead_reckoning(double eps){
	int n = points.size();
	double max_d = 0;
	int start_idx = 0;
	vector<double> d = cacl_distance();
	vector<double> angles = cacl_angle();
	vector<int> simplifindex;
	simplifindex.push_back(0);
	for (int i = 2; i < n; i++){
		max_d += fabs(d[i - 1] * sin(angles[i - 1] - angles[start_idx]));
		if (abs(max_d) > eps){
			max_d = 0;
			simplifindex.push_back(i - 1);
			start_idx = i - 1;
		}
	}
	if (simplifindex[simplifindex.size() - 1] != n - 1)
		simplifindex.push_back(n - 1);
	
	return simplifindex;

}

int main(int argc, char** args){
	/*gpsreader("C://Users//Ding//Desktop//20081023025304-0.plt");
	vector<int> cmp_index = dead_reckoning(0.0006);
	for (int i = 0; i < cmp_index.size(); i++)
		cout << cmp_index[i] << " ";
	cout << endl;*/
	string filename = args[1];
	double epsilon = atof(args[2]);
	char* save_filename = args[3];
	gpsreader(filename);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	vector<int> cmp_index = dead_reckoning(epsilon);
	gettimeofday(&end, NULL);
	long timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	FILE* s_fp = fopen(save_filename, "w+");
	for (int i = 0; i < cmp_index.size(); i++)
		fprintf(s_fp, "%lf %lf %lf\n", points[cmp_index[i]].lat, points[cmp_index[i]].lon, points[cmp_index[i]].time);
	fprintf(s_fp, "%lf\n", timeuse / 1000000.0);
	fclose(s_fp);
}
