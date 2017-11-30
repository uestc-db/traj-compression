#include "illinois_reader.h"

#include <string>
#include <ctime>
#include <limits>
using namespace std;

GPSPoint IllinoisReader::read_point()
{
  double lat, lon;
  *is >> lat;
  is->ignore(numeric_limits<streamsize>::max(), '|');
  is->ignore(numeric_limits<streamsize>::max(), ' ');
  *is >> lon;
  is->ignore(numeric_limits<streamsize>::max(), '|');
  is->ignore(numeric_limits<streamsize>::max(), ' ');
  lat = lat / 100.0;
  lon = -(lon / 100);

  int hour, min, sec;
  *is >> hour;
  is->ignore(numeric_limits<streamsize>::max(), ':');
  if(hour < 0) hour += 24;
  *is >> min;
  is->ignore(numeric_limits<streamsize>::max(), ':');
  *is >> sec;
  is->ignore(numeric_limits<streamsize>::max(), '\n');

  tm timeinfo;
  timeinfo.tm_sec = sec;
  timeinfo.tm_min = min;
  timeinfo.tm_hour = hour;
  timeinfo.tm_year = 0;
  timeinfo.tm_mon = 0;
  timeinfo.tm_mday = 1;
  int time = mktime(&timeinfo);

  is->ignore(numeric_limits<streamsize>::max(), '|');
  is->ignore(numeric_limits<streamsize>::max(), ' ');

  double d;
  *is >> d;
  is->ignore(numeric_limits<streamsize>::max(), '|');
  is->ignore(numeric_limits<streamsize>::max(), ' ');
  *is >> d;
  is->ignore(numeric_limits<streamsize>::max(), '\n');

  GPSPoint point(time, lat, lon);

  return point;
}


