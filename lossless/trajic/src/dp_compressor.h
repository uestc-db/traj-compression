#ifndef _DP_COMPRESSOR_H_
#define _DP_COMPRESSOR_H_

#include "compressor.h"

/// A Compressor which uses Douglas-Peucker sampling
class DPCompressor : public Compressor
{
public:
  DPCompressor(Compressor *aux_compr, double max_error=0)
    : aux_compr(aux_compr), max_error(max_error) {}
  virtual void compress(obstream& obs, vector<GPSPoint> points) override;
  virtual vector<GPSPoint> decompress(ibstream& ibs) override;
  
  double max_error_kms;
private:
  Compressor *aux_compr;
  double max_error;
  
  vector<GPSPoint> downsample(vector<GPSPoint> points);
};

#endif

