#ifndef _LINEAR_PREDICTOR_H_
#define _LINEAR_PREDICTOR_H_

#include "predictor.h"

class LinearPredictor : public Predictor
{
public:
  virtual bits64 predict_time(
    bits64 tuples[][3], int index) override;
  virtual void predict_coords(
    bits64 tuples[][3], int index, bits64* result) override;
};

#endif
