#include "predictive_compressor.h"
#include "dynamic_encoder.h"

#include <cmath>
#include <algorithm>
using namespace std;

Predictor* default_predictor = new LinearPredictor();

int PredictiveCompressor::calculate_discarded_bits(double max_value,
  double error_bound)
{
  // Get exponent field from maxValue
  long x = (bits64{.dbl = max_value}.lng >> 52) & 0x7ff;
  // Calculate number of bits to discard
  return min((int)log2(error_bound * pow(2, 1075 - x) + 1), 52);
}

void PredictiveCompressor::compress(obstream& obs, vector<GPSPoint> points)
{
  double max_time = 0, max_coord = 0;
  for(GPSPoint point : points)
  {
    max_time = max(max_time, abs(point.get_time()));
    max_coord = max(max_coord, abs(point.get_latitude()));
    max_coord = max(max_coord, abs(point.get_longitude()));
  }

  int discard[3];
  discard[0] = calculate_discarded_bits(max_time, max_temporal_err);
  discard[1] = calculate_discarded_bits(max_coord, max_spatial_err);
  discard[2] = discard[1];

  obs.write_int(discard[0], 8);
  obs.write_int(discard[1], 8);

  obs.write_int(points.size(), 32);

  obs.write_double(points[0].get_time());
  obs.write_double(points[0].get_latitude());
  obs.write_double(points[0].get_longitude());

  bits64 tuples[points.size()][3];
  for(size_t i = 0; i < points.size(); ++i)
  {
    tuples[i][0].dbl = points[i].get_time();
    tuples[i][1].dbl = points[i].get_latitude();
    tuples[i][2].dbl = points[i].get_longitude();
  }

  uint64_t residuals[3][points.size() - 1];
  for(size_t i = 0; i < points.size() - 1; ++i)
  {
    if(discard[0] > 0)
    {
      uint64_t pred_time = predictor->predict_time(tuples, i + 1).lng;
      uint64_t residual = tuples[i + 1][0].lng ^ pred_time;
      residual = (residual >> discard[0]) << discard[0];
      tuples[i + 1][0].lng = pred_time ^ residual;
    }

    bits64 pred[3];
    predictor->predict_coords(tuples, i + 1, pred);

    for(int j = 0; j < 3; ++j)
    {
      uint64_t residual = tuples[i + 1][j].lng ^ pred[j].lng;
      residual >>= discard[j];
      residuals[j][i] = residual;
      residual <<= discard[j];
      tuples[i + 1][j].lng = pred[j].lng ^ residual;
    }
  }

  Encoder *encoders[3];

  for(int j = 0; j < 3; ++j)
    encoders[j] = new DynamicEncoder(obs, residuals[j], points.size() - 1);

  for(size_t i = 0; i < points.size() - 1; ++i)
    for(int j = 0; j < 3; ++j)
      encoders[j]->encode(obs, residuals[j][i]);

  for(int j = 0; j < 3; ++j)
    delete encoders[j];
}

vector<GPSPoint> PredictiveCompressor::decompress(ibstream& ibs)
{
  int discard[3];
  discard[0] = ibs.read_byte();
  discard[1] = ibs.read_byte();
  discard[2] = discard[1];

  int n_points = ibs.read_int(32);
  bits64 tuples[n_points][3];

  for(int i = 0; i < 3; ++i)
    tuples[0][i].lng = ibs.read_int(64);

  Encoder* decoders[3];
  for(int j = 0; j < 3; ++j)
    decoders[j] = new DynamicEncoder(ibs);

  for(int i = 1; i < n_points; ++i)
  {
    uint64_t residuals[3];

    for(int j = 0; j < 3; ++j)
      residuals[j] = decoders[j]->decode(ibs) << discard[j];

    uint64_t time = predictor->predict_time(tuples, i).lng ^ residuals[0];
    tuples[i][0].lng = time;
    bits64 pred[3];
    predictor->predict_coords(tuples, i, pred);

    for(int j = 0; j < 3; ++j)
      tuples[i][j].lng = pred[j].lng ^ residuals[j];
  }

  vector<GPSPoint> points;

  for(int i = 0; i < n_points; ++i)
  {
    GPSPoint point(tuples[i][0].dbl, tuples[i][1].dbl, tuples[i][2].dbl);
    points.push_back(point);
  }

  for(int j = 0; j < 3; ++j)
    delete decoders[j];

  return points;
}

