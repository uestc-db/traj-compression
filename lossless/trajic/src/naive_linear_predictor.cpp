#include "naive_linear_predictor.h"
#include "gps_point.h"
#include <cmath>

bits64 NaiveLinearPredictor::predict_time(bits64 tuples[][3], int index)
{
  double cur = tuples[index - 1][0].dbl;
  double prev = index > 1 ? tuples[index - 2][0].dbl : cur;
  return bits64{.dbl = cur * 2 - prev};
}

void NaiveLinearPredictor::predict_coords(
  bits64 tuples[][3], int index, bits64* result)
{
  GPSPoint cur(tuples[index - 1]);
  GPSPoint prev = index > 1 ? GPSPoint(tuples[index - 2]) : cur;

  double time = predict_time(tuples, index).dbl;
  double lat = cur.get_latitude() * 2 - prev.get_latitude();
  double lon = cur.get_longitude() * 2 - prev.get_longitude();

  result[0] = bits64{.dbl = time};
  result[1] = bits64{.dbl = lat};
  result[2] = bits64{.dbl = lon};
}
