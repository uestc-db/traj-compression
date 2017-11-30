#ifndef _SQUISH_COMPRESSOR_H_
#define _SQUISH_COMPRESSOR_H_

#include "compressor.h"

/// A Compressor using the SQUISH algorithm proposed by Muckell et al
class SquishCompressor : public Compressor
{
public:
  SquishCompressor(double compression_ratio)
    : compression_ratio(compression_ratio) {}
  virtual void compress(obstream& obs, vector<GPSPoint> points) override;
  virtual vector<GPSPoint> decompress(ibstream& ibs) override;

private:
  double compression_ratio;
};

#endif

