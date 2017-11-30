#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include<algorithm>
#include "sys/time.h"
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
vector<int> OPW(double eplision){
	int e = 0;
	int originalIndex = 0;
	vector<int> simplified;
	simplified.push_back(originalIndex);

	e = originalIndex + 2;
	while (e < points.size()){
		int i = originalIndex + 1;
		bool condOPW = true;
		while (i < e && condOPW){
			if (cacl_PED(points[originalIndex], points[i], points[e]) > eplision)
				condOPW = false;
			else
				i++;
		}
		if (!condOPW){
			originalIndex = i;
			simplified.push_back(originalIndex);
			e = originalIndex + 2;
		}
		else
			e++;
	}
	simplified.push_back(points.size() - 1);
	
	return simplified;
}

int main(int argc, char** args){
	string filename = args[1];
	double epsilon = atof(args[2]);
	char* save_filename = args[3];
	gpsreader(filename);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	vector<int> cmp_index = OPW(epsilon);
	gettimeofday(&end, NULL);
	long timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	FILE* s_fp = fopen(save_filename, "w+");
	for (int i = 0; i < cmp_index.size(); i++)
		fprintf(s_fp, "%lf %lf %lf\n", points[cmp_index[i]].lat, points[cmp_index[i]].lon, points[cmp_index[i]].time);
	fprintf(s_fp, "%lf\n", timeuse / 1000000.0);
	fclose(s_fp);
	/*gpsreader("C://Users//Ding//Desktop//20081023025304-0.plt");
	vector<int> cmp_index = OPW(0.0001);
	for (int i = 0; i < cmp_index.size(); i++)
		cout << cmp_index[i] << " ";
	cout << endl;*/
}
