#ifndef _DELTA_COMPRESSOR_H_
#define _DELTA_COMPRESSOR_H_

#include "compressor.h"

/// A Compressor which stores the "deltas" between successive points
class DeltaCompressor : public Compressor
{
public:
  virtual void compress(obstream& obs, vector<GPSPoint> points) override;
  virtual vector<GPSPoint> decompress(ibstream& ibs) override;
};

#endif

