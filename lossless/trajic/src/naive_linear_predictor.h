#ifndef _NAIVE_LINEAR_PREDICTOR_H_
#define _NAIVE_LINEAR_PREDICTOR_H_

#include "predictor.h"

class NaiveLinearPredictor : public Predictor
{
public:
  virtual bits64 predict_time(
    bits64 tuples[][3], int index) override;
  virtual void predict_coords(
    bits64 tuples[][3], int index, bits64* result) override;
};

#endif
