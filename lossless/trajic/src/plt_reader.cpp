#include "plt_reader.h"

#include <string>
#include <ctime>
#include <limits>
using namespace std;

PLTReader::PLTReader(istream* is) : GPSReader(is)
{
  for(int i = 0; i < 6; ++i)
    is->ignore(numeric_limits<streamsize>::max(), '\n');
}

GPSPoint PLTReader::read_point()
{
  double lat, lon;
  *is >> lat;
  is->ignore(numeric_limits<streamsize>::max(), ',');
  *is >> lon;
  is->ignore(numeric_limits<streamsize>::max(), ',');
  is->ignore(numeric_limits<streamsize>::max(), ',');
  is->ignore(numeric_limits<streamsize>::max(), ',');
  is->ignore(numeric_limits<streamsize>::max(), ',');
  
  int year, month, day;
  *is >> year;
  is->ignore(numeric_limits<streamsize>::max(), '-');
  *is >> month;
  is->ignore(numeric_limits<streamsize>::max(), '-');
  *is >> day;
  is->ignore(numeric_limits<streamsize>::max(), ',');
  
  int hour, min, sec;
  *is >> hour;
  is->ignore(numeric_limits<streamsize>::max(), ':');
  *is >> min;
  is->ignore(numeric_limits<streamsize>::max(), ':');
  *is >> sec;
  is->ignore(numeric_limits<streamsize>::max(), '\n');
  
  tm timeinfo;
  timeinfo.tm_sec = sec;
  timeinfo.tm_min = min;
  timeinfo.tm_hour = hour;
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_mday = day;
  int time = mktime(&timeinfo);
  
  GPSPoint point(time, lat, lon);
  
  return point;
}


