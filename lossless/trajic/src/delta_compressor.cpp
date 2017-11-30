#include "delta_compressor.h"

#include <cmath>

void DeltaCompressor::compress(obstream& obs, vector<GPSPoint> points)
{
  bits64 tuples[points.size()][3];
  for(size_t i = 0; i < points.size(); ++i)
  {
    tuples[i][0].dbl = points[i].get_time();
    tuples[i][1].dbl = points[i].get_latitude();
    tuples[i][2].dbl = points[i].get_longitude();
  }

  obs.write_int(points.size(), 32);

  obs.write_double(points[0].get_time());
  obs.write_double(points[0].get_latitude());
  obs.write_double(points[0].get_longitude());

  for(size_t i = 1; i < points.size(); ++i)
  {
    for(int j = 0; j < 3; ++j)
    {
      uint64_t delta = tuples[i][j].lng ^ tuples[i - 1][j].lng;

      int guts = 1;
      if(delta > 0) guts = (int)log2(delta) + 1;
      obs.write_int(guts - 1, 6);
      obs.write_int(delta, guts);
    }
  }
}

vector<GPSPoint> DeltaCompressor::decompress(ibstream& ibs)
{
  int size = ibs.read_int(32);
  vector<GPSPoint> points;

  bits64 prev[3];

  prev[0].lng = ibs.read_int(64);
  prev[1].lng = ibs.read_int(64);
  prev[2].lng = ibs.read_int(64);

  for(int i = 0; i < size - 1; ++i)
  {
    points.push_back(GPSPoint(prev[0].dbl, prev[1].dbl, prev[2].dbl));

    for(int j = 0; j < 3; ++j)
    {
      int guts = ibs.read_int(6) + 1;
      uint64_t delta = ibs.read_int(guts);
      prev[j].lng ^= delta;
    }
  }

  points.push_back(GPSPoint(prev[0].dbl, prev[1].dbl, prev[2].dbl));

  return points;
}

