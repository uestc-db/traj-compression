#include "constant_predictor.h"
#include "gps_point.h"
#include <cmath>

bits64 ConstantPredictor::predict_time(bits64 tuples[][3], int index)
{
  double cur = tuples[index - 1][0].dbl;
  return bits64{.dbl = cur};
}

void ConstantPredictor::predict_coords(
  bits64 tuples[][3], int index, bits64* result)
{
  GPSPoint cur(tuples[index - 1]);

  double time = predict_time(tuples, index).dbl;
  double lat = cur.get_latitude();
  double lon = cur.get_longitude();

  result[0] = bits64{.dbl = time};
  result[1] = bits64{.dbl = lat};
  result[2] = bits64{.dbl = lon};
}
