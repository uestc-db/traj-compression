#ifndef _GPS_POINT_H_
#define _GPS_POINT_H_

#define EARTH_RADIUS 6371

#include "util.h"

#include <iostream>
using namespace std;

class GPSPoint
{
public:
  GPSPoint(double time, double lat, double lon)
    : time(time), lat(lat), lon(lon) {}
  GPSPoint(bits64* triple)
    : GPSPoint(triple[0].dbl, triple[1].dbl, triple[2].dbl) {}
  GPSPoint()
    : GPSPoint(0, 0, 0) {}
  double get_time() const { return time; }
  double get_longitude() const { return lon; }
  double get_latitude() const { return lat; }
  /// Calculates the Euclidian distance between this point and another
  double distance(GPSPoint& other) const;
  /// Calculates the distance between this point and another in kilometres
  double distance_kms(GPSPoint& other) const;
  /// Writes a point to an ostream in a human-readable format
  friend ostream& operator<<(ostream& os, const GPSPoint& p);
private:
  double time, lat, lon;
};

#endif

