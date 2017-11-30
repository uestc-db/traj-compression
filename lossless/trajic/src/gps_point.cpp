#include "gps_point.h"
#include <cmath>

double GPSPoint::distance(GPSPoint& other) const
{
  return sqrt(pow(other.lat - lat, 2) +
         pow(other.lon - lon, 2));
}

/// The calculation takes the Earth's curvature into consideration using
/// the haversine function.
double GPSPoint::distance_kms(GPSPoint& other) const
{
  double lat1 = get_latitude() * M_PI / 180;
  double lat2 = other.get_latitude() * M_PI / 180;
  double lon1 = get_longitude() * M_PI / 180;
  double lon2 = other.get_longitude() * M_PI / 180;

  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;

  double a = sin(dlat / 2) * sin(dlat / 2) +
             sin(dlon / 2) * sin(dlon / 2) * cos(lat1) * cos(lat2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  
  return EARTH_RADIUS * c;
}

ostream& operator<<(ostream& os, const GPSPoint& p)
{
  os << "[time: " << p.time;
  os << ", lat: " << p.lat;
  os << ", lon: " << p.lon << "]";
  return os;
}

