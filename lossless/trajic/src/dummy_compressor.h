#ifndef _DUMMY_COMPRESSOR_H_
#define _DUMMY_COMPRESSOR_H_

#include "compressor.h"

/// A dummy Compressor which stores raw points
class DummyCompressor : public Compressor
{
public:
  virtual void compress(obstream& obs, vector<GPSPoint> points) override;
  virtual vector<GPSPoint> decompress(ibstream& ibs) override;
};

#endif

