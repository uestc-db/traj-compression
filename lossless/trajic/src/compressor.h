#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_

#include "gps_point.h"
#include "obstream.h"
#include "ibstream.h"

#include <vector>
using namespace std;

/// An abstract base class for compressing GPS trajectories
class Compressor
{
public:
  /// Compresses and writes out a trajectory
  virtual void compress(obstream& obs, vector<GPSPoint> points) = 0;
  /// Reads in and decompresses a trajectory
  virtual vector<GPSPoint> decompress(ibstream& ibs) = 0;
};

#endif

