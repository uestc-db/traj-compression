#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include<algorithm>
#include "sys/time.h"
using namespace std;
#define EARTH_RADIUS 6371229
typedef struct Point{
	double lat;
	double lon;
	double time;
}Point;
vector<Point> points;
double ori_threshold;
double speed_threshold;
int dataset;
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
double cacl_haversine(Point a, Point b){
	double lat1 = a.lat * M_PI / 180;
	double lat2 = b.lat * M_PI / 180;
	double lon1 = a.lon * M_PI / 180;
	double lon2 = b.lon * M_PI / 180;
	double dlat = lat2 - lat1;
	double dlon = lon2 - lon1;
	double a_a = sin(dlat / 2) * sin(dlat / 2) + sin(dlon / 2) * sin(dlon / 2)*cos(lat1) *cos(lat2);
	double c = 2 * atan2(sqrt(a_a), sqrt(1 - a_a));

	return EARTH_RADIUS * c;
}
double cacl_angle(Point a ,Point b){
	double lat_diff = b.lat - a.lat;
	double lon_diff = b.lon - a.lon;
	return atan2(lon_diff, lat_diff);
	


}
bool safe_oritentation(Point sample_b, Point sample_c, Point point_c, Point point_d, Point  point_e){
	double angle_sample_bc = cacl_angle(sample_b, sample_c);
	double angle_de = cacl_angle(point_d, point_e);
	double angle_sample_bc_de = angle_de - angle_sample_bc;
	double angle_trajectory_cd = cacl_angle(point_c, point_d);
	double angle_trajectory_cd_de = angle_de - angle_trajectory_cd;
	if (fabs(angle_sample_bc_de) > ori_threshold || fabs(angle_trajectory_cd_de) > ori_threshold)
		return false;
	else
		return true;
}

double cacl_distance(Point a, Point b){

	//return sqrt((pow(a.lat - b.lat, 2) + pow(a.lon - b.lon, 2)));
	if (dataset == 0)
		return sqrt((pow(a.lat - b.lat, 2) + pow(a.lon - b.lon, 2)));
	else{
		if (dataset == 1)
			return sqrt((pow(a.lat / 1000 - b.lat / 1000, 2) + pow(a.lon / 1000 - b.lon / 1000, 2)));
		else
			return cacl_haversine(a, b);
	}
}
double cacl_speed(Point a,Point b){

        //return sqrt((pow(a.lat - b.lat, 2) + pow(a.lon - b.lon, 2))) / (b.time - a.time);
        return cacl_distance(a,b) / (b.time - a.time);
}

bool safe_speed(Point sample_b, Point sample_c, Point point_c, Point point_d, Point  point_e){
	double sample_speed = cacl_speed(sample_b, sample_c);
	double trajectory_speed = cacl_speed(point_c, point_d);
	double de_speed = cacl_speed(point_d,point_e);
	if(fabs(sample_speed - de_speed) > speed_threshold || fabs(trajectory_speed - de_speed) > speed_threshold)
		return false;
	else
		return true;
	//double max_sample_distance = (point_e.time - point_d.time)*(sample_speed + speed_threshold);
	//double min_sample_distance = (point_e.time - point_d.time)*(sample_speed - speed_threshold);
	//double max_trajectory_distance = (point_e.time - point_d.time)*(trajectory_speed + speed_threshold);
	//double min_trajectory_distance = (point_e.time - point_d.time)*(trajectory_speed - speed_threshold);
	//double distance_de = cacl_distance(point_d, point_e);
	//if (distance_de <= max_sample_distance && distance_de >= min_sample_distance && distance_de <= max_trajectory_distance && distance_de >= min_trajectory_distance)
	//	return true;
	//else

	//	return false;
}

vector<Point> Threshold(){
	vector<Point> sample;
	vector<int> simplifyindex;
	sample.push_back(points[0]);
	sample.push_back(points[1]);
	simplifyindex.push_back(0);
	simplifyindex.push_back(1);
	int i = 2;
	for (int i = 2; i < points.size()-1; i++){
		//cout << i << endl;
		//cout << i << ":" << cacl_oritentation(sample[sample.size() - 2], sample[sample.size() - 1], points[i - 2], points[i - 1], points[i]) << " " <<cacl_distance(sample[sample.size() - 2], sample[sample.size() - 1], points[i - 2], points[i - 1], points[i]) << endl;
		if (safe_speed(sample[sample.size() - 2], sample[sample.size() - 1], points[i - 2], points[i - 1], points[i]) && safe_oritentation(sample[sample.size() - 2], sample[sample.size() - 1], points[i - 2], points[i - 1], points[i]))
			continue;
		else
		{
			sample.push_back(points[i]);
			//simplifyindex.push_back(i);
		}
	}
	
	//simplifyindex.push_back(points.size()-1);
	sample.push_back(points[points.size() - 1]);

	return sample; 
		
}
int main(int argc, char** args){
	/*
	gpsreader("C://Users//Ding//Desktop//20081023025304-0.plt");
	ori_threshold = 0.8;
	speed_threshold = 25;
	vector<int> cmp_index = Threshold();
	for (int i = 0; i < cmp_index.size(); i++)
		cout << cmp_index[i] << " ";
	cout << endl;*/
	string filename = args[1];
	speed_threshold = atof(args[2]);
	ori_threshold = atof(args[3]);
	dataset = atoi(args[4]);
	char* save_filename = args[5];
	gpsreader(filename);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	vector<Point> cmp_points = Threshold();
	gettimeofday(&end, NULL);
	long timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	FILE* s_fp = fopen(save_filename, "w+");
	for (int i = 0; i < cmp_points.size(); i++)
		fprintf(s_fp, "%lf %lf %lf\n", cmp_points[i].lat, cmp_points[i].lon, cmp_points[i].time);
	fprintf(s_fp, "%lf\n", timeuse / 1000000.0);
	fclose(s_fp);
}
