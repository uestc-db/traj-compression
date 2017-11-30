#ifndef _GPS_READER_H_
#define _GPS_READER_H_

#include <iostream>
#include <vector>
using namespace std;

#include "gps_point.h"

class GPSReader
{
public:
  GPSReader(istream *is) : is(is) {};
  virtual GPSPoint read_point() = 0;
  virtual bool has_more() { return is->peek() >= 0; }
  virtual vector<GPSPoint> read_points()
  {
    vector<GPSPoint> points;
    while(has_more()) points.push_back(read_point());
    return points;
  }
  virtual ~GPSReader() {};

protected:
  istream *is;
};

#endif

