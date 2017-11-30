#include "dp_compressor.h"
#include "delta_compressor.h"

void DPCompressor::compress(obstream& obs, vector<GPSPoint> points)
{
  max_error_kms = 0;
  aux_compr->compress(obs, downsample(points));
}

vector<GPSPoint> DPCompressor::decompress(ibstream& ibs)
{
  return aux_compr->decompress(ibs);
}

vector<GPSPoint> DPCompressor::downsample(vector<GPSPoint> points)
{
  if(points.size() < 3) return points;

  GPSPoint first = points[0];
  GPSPoint last = points[points.size() - 1];
  double dlat = last.get_latitude() - first.get_latitude();
  double dlon = last.get_longitude() - first.get_longitude();
  double dt = last.get_time() - first.get_time();

  double max_kms = -1;
  int furthest;
  double max_dist = -1;
  for(size_t i = 1; i < points.size() - 1; ++i)
  {
    double tr = (points[i].get_time() - first.get_time()) / dt;
    GPSPoint approx(points[i].get_time(),
                 first.get_latitude() + dlat * tr,
                 first.get_longitude() + dlon * tr);
    double dist = points[i].distance(approx);
    max_kms = max(max_kms, points[i].distance_kms(approx));
    if(dist > max_dist)
    {
      max_dist = dist;
      furthest = i;
    }
  }

  if(max_dist > max_error)
  {
    vector<GPSPoint> a(points.begin(), points.begin() + furthest);
    vector<GPSPoint> b(points.begin() + furthest, points.end());
    a = downsample(a);
    b = downsample(b);
    vector<GPSPoint> ab;
    ab.reserve(a.size() + b.size());
    ab.insert(ab.end(), a.begin(), a.end());
    ab.insert(ab.end(), b.begin(), b.end());
    return ab;
  }
  else
  {
    max_error_kms = max(max_error_kms, max_kms);
  }

  vector<GPSPoint> ab;
  ab.push_back(first);
  ab.push_back(last);

  return ab;
}

