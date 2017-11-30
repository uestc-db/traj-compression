#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include<cmath>
#include<algorithm>
#include "sys/time.h"
#include <float.h>
using namespace std;
typedef struct Point{
	int index;
	double lat;
	double lon;
	double time;
}Point;
typedef struct {
	double priority;
	double pi;
	Point point;
} GPSPointWithSED;
int capacity = 4;
vector<Point> points;
vector<GPSPointWithSED> Q;
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
		point.index = index;
		index++;
		points.push_back(point);

	}
	points.pop_back();
	return;
}
double cacl_SED(Point s, Point m, Point e){
	double numerator = m.time - s.time;
	double denominator = e.time - s.time;
	double time_ratio = denominator == 0 ? 1 : numerator / denominator;
	double lat = s.lat + (e.lat - s.lat)*time_ratio;
	double lon = s.lon + (e.lon - s.lon)*time_ratio;
	double lat_diff = lat - m.lat;
	double lon_diff = lon - m.lon;
	return sqrt(lat_diff*lat_diff + lon_diff*lon_diff);
}

void adjust_priority(int pre_index, int Q_index, int succ_index){
	if (Q_index == Q.size() - 1 || Q_index == 0)
		return;
	double p = Q[Q_index].pi + cacl_SED(Q[pre_index].point, Q[Q_index].point, Q[succ_index].point);
	Q[Q_index].priority = p;
}

void reduce(int min_index, double min_p){
	Q[min_index - 1].pi = max(min_p, Q[min_index - 1].pi);
	Q[min_index + 1].pi = max(min_p, Q[min_index + 1].pi);
	adjust_priority(min_index - 2, min_index - 1, min_index + 1);
	adjust_priority(min_index - 1, min_index + 1, min_index + 2);
	std::vector<GPSPointWithSED>::iterator it = Q.begin() + min_index;
	Q.erase(it);

}

int find_mini_priority(){
	auto start = Q.begin();
	++start;
	auto to_remove = Q.end();
	int min_index = 1, k = 1;
	for (auto it = Q.begin() + 1; it != Q.end() - 1; ++it) {
		if (to_remove == Q.end() || it->priority < to_remove->priority) {
			to_remove = it;
			min_index = k;
		}
		k = k + 1;
	}
	return min_index;
}
void SQUISH_E(int cmp_ratio, double sed_error){
	int i = 0,min_index ,k;
	double min_p;
	while (i < points.size()){
		if (i / cmp_ratio >= capacity){
			capacity = capacity + 1;
		}
		Q.push_back({DBL_MAX, 0, points[i] });
		if (i > 0){
			adjust_priority(Q.size() - 3, Q.size() - 2, Q.size() - 1);
		}
		if (Q.size() == capacity){
			min_index = find_mini_priority();
			min_p = Q[min_index].priority;
			reduce(min_index, min_p);
		}
		i = i + 1;
	}
	min_index = find_mini_priority();
	min_p = Q[min_index].priority;
	while (min_p <= sed_error){
		reduce(min_index, min_p);
		min_index = find_mini_priority();
		min_p = Q[min_index].priority;
	}
}

int main(int argc, char** args){
	/*gpsreader("C://Users//Ding//Desktop//20081023025304-0.plt");
	SQUISH_E(1, 0.0001);
	for (int i = 0; i < Q.size(); i++)
		cout << Q[i].point.index << " ";
	cout << endl;
	cout << points.size() / Q.size() << endl;*/
	string filename = args[1];
	double ratio = atof(args[2]);
	double sed = atof(args[3]);
	char* save_filename = args[4];
	gpsreader(filename);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	SQUISH_E(ratio, sed);
	gettimeofday(&end, NULL);
	long timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	FILE* s_fp = fopen(save_filename, "w+");
	for (int i = 0; i < Q.size(); i++)
		fprintf(s_fp, "%lf %lf %lf\n", Q[i].point.lat, Q[i].point.lon, Q[i].point.time);
	fprintf(s_fp, "%lf\n", timeuse / 1000000.0);
	fclose(s_fp);
}
