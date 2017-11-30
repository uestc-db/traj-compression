#include "linear_predictor.h"
#include "gps_point.h"
#include <cmath>

bits64 LinearPredictor::predict_time(bits64 tuples[][3], int index)
{
  double cur = tuples[index - 1][0].dbl;
  double prev = index > 1 ? tuples[index - 2][0].dbl : cur;
  return bits64{.dbl = cur * 2 - prev};
}

void LinearPredictor::predict_coords(
  bits64 tuples[][3], int index, bits64* result)
{
  GPSPoint cur(tuples[index - 1]);
  GPSPoint prev = index > 1 ? GPSPoint(tuples[index - 2]) : cur;

  double prevInterval = cur.get_time() - prev.get_time();
  double interval = tuples[index][0].dbl - cur.get_time();
  double ratio = prevInterval > 0 ? interval / prevInterval : 1;

  double time = predict_time(tuples, index).dbl;
  double lat = cur.get_latitude() +
    (cur.get_latitude() - prev.get_latitude()) * ratio;
  double lon = cur.get_longitude() +
    (cur.get_longitude() - prev.get_longitude()) * ratio;

  result[0] = bits64{.dbl = time};
  result[1] = bits64{.dbl = lat};
  result[2] = bits64{.dbl = lon};
}
