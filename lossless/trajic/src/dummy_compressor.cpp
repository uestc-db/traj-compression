#include "dummy_compressor.h"

#include <cmath>

void DummyCompressor::compress(obstream& obs, vector<GPSPoint> points)
{
  obs.write_int(points.size(), 32);

  for(size_t i = 0; i < points.size(); ++i)
  {
    obs.write_double(points[i].get_time());
    obs.write_double(points[i].get_latitude());
    obs.write_double(points[i].get_longitude());
  }
}

vector<GPSPoint> DummyCompressor::decompress(ibstream& ibs)
{
  int size = ibs.read_int(32);
  vector<GPSPoint> points;

  bits64 tuple[3];

  for(int i = 0; i < size; ++i)
  {
    tuple[0].lng = ibs.read_int(64);
    tuple[1].lng = ibs.read_int(64);
    tuple[2].lng = ibs.read_int(64);
    points.push_back(GPSPoint(tuple[0].dbl, tuple[1].dbl, tuple[2].dbl));
  }

  return points;
}

