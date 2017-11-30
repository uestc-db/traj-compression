#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "obstream.h"
#include "ibstream.h"

class Encoder
{
public:
  virtual void encode(obstream& obs, uint64_t num) = 0;
  virtual uint64_t decode(ibstream& ibs) = 0;
  virtual ~Encoder() {};
};

#endif

