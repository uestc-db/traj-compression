#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "util.h"

/// Predictor is an abstract base class from which implementations of
/// GPS trajectory extrapolation algorithms may be based
class Predictor
{
public:
  /// Predicts the sample time of the next GPS point in the trajectory.
  /// Points 0 to index-1 of the trajectory are made available in the
  /// `tuples` array
  virtual bits64 predict_time(
    bits64 tuples[][3], int index) = 0;
  /// Predicts the coordinates of the next GPS point in the trajectory.
  /// Points 0 to index-1 of the trajectory are made available in the
  /// `tuples` array. The time coordinate returned by this method must
  /// always be the same as the value returned by predict_time
  virtual void predict_coords(
    bits64 tuples[][3], int index, bits64* result) = 0;
};

#endif

