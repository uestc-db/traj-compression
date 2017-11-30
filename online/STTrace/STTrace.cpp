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
	int index;
	double lat;
	double lon;
	double time;
}Point;
typedef struct {
	Point point;
	double sed;
} GPSPointWithSED;

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

vector<GPSPointWithSED> STTrace(double cmp_ratio){
	int max_buffer_size = (int)(cmp_ratio * points.size());
	vector<GPSPointWithSED> buffer;
	buffer.reserve(max_buffer_size + 1);
	buffer.push_back({ points[0],0 });
	if (max_buffer_size > 2) {
		buffer.push_back({ points[1], 0 });
		for (int i = 2; i < points.size(); ++i){
			buffer.push_back({ points[i], 0});
			// Compute SED for previous point
			Point segment_start = buffer[buffer.size() - 3].point;
			Point segment_end = buffer[buffer.size() - 1].point;
			buffer[buffer.size() - 2].sed = cacl_SED(segment_start, buffer[buffer.size() - 2].point, segment_end);
			// Buffer full, remove a point
			if (buffer.size() > max_buffer_size){
				/*
				double min_sed = buffer[1].sed;
				int min_index = 1;
				int k = 2;
				while (k < buffer.size){
					if (buffer[k].sed < min_sed){
						min_sed = buffer[i].sed;
						min_index = k;
					}
					k++;
				}
				if (min_index -1 > 0) 
					buffer[min_index - 1].sed = cacl_SED(buffer[min_index - 2].point, buffer[min_index - 1].point, buffer[min_index + 1].point);
				if (min_index + 1 < buffer.size()-1)
					buffer[min_index +1].sed = cacl_SED(buffer[min_index - 1].point, buffer[min_index + 1].point, buffer[min_index + 2].point);
				*/
				auto start = buffer.begin();
				++start;
				auto to_remove = buffer.end();
				int min_index = 1,k = 1;
				for (auto it = buffer.begin() + 1; it != buffer.end() - 1; ++it) {
					if (to_remove == buffer.end() || it->sed < to_remove->sed) {
						to_remove = it;
						min_index = k;
					}
					k = k + 1;
				}
				if (min_index - 1 > 0)
					buffer[min_index - 1].sed = cacl_SED(buffer[min_index - 2].point, buffer[min_index - 1].point, buffer[min_index + 1].point);
				if (min_index + 1 < buffer.size() - 1)
					buffer[min_index + 1].sed = cacl_SED(buffer[min_index - 1].point, buffer[min_index + 1].point, buffer[min_index + 2].point);
				buffer.erase(to_remove);

			}
		}
		
	}
	else
		buffer.push_back({ points[points.size() - 1], 0 });
	return buffer;
}

int main(int argc, char** args){
	/*gpsreader("C://Users//Ding//Desktop//20081023025304-0.plt");
	vector<GPSPointWithSED> cmp_buffer = STTrace(0.2);
	for (int i = 0; i < cmp_buffer.size(); i++)
		//printf("%lf %lf %lf\n", cmp_buffer[i].point.lat, cmp_buffer[i].point.lon, cmp_buffer[i].point.time);
		cout << cmp_buffer[i].point.index << " ";
	cout << endl;
	cout << points.size() / cmp_buffer.size() << endl;*/
	string filename = args[1];
	double cmp_ratio = atof(args[2]);
	char* save_filename = args[3];
	gpsreader(filename);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	vector<GPSPointWithSED> cmp_buffer = STTrace(cmp_ratio);
	gettimeofday(&end, NULL);
	long timeuse =1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
	FILE* s_fp = fopen(save_filename, "w+");
	for (int i = 0; i < cmp_buffer.size(); i++)
		fprintf(s_fp, "%lf %lf %lf\n", cmp_buffer[i].point.lat, cmp_buffer[i].point.lon, cmp_buffer[i].point.time);
	fprintf(s_fp, "%lf\n", timeuse / 1000000.0);
	fclose(s_fp);
}
