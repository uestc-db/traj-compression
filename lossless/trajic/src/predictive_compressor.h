#ifndef _PREDICTIVE_COMPRESSOR_H_
#define _PREDICTIVE_COMPRESSOR_H_

#include "compressor.h"
#include "predictor.h"
#include "linear_predictor.h"

/// The default predictor, which is currently a LinearPredictor
extern Predictor* default_predictor;

/// A Compressor based around a prediction-based algorithm
class PredictiveCompressor : public Compressor
{
public:
  /// Constructs a PredictiveCompressor with the specified prediction algorithm
  /// and error bounds
  PredictiveCompressor(Predictor *predictor, double max_temporal_err = 0,
                       double max_spatial_err = 0)
    : predictor(predictor), max_temporal_err(max_temporal_err),
      max_spatial_err(max_spatial_err) {}
  /// Constructs a PredictiveCompressor with the ::default_predictor and the
  /// specified error bounds
  PredictiveCompressor(double max_temporal_err = 0, double max_spatial_err = 0)
    : PredictiveCompressor(default_predictor, max_temporal_err, max_spatial_err)
  {}
  virtual void compress(obstream& obs, vector<GPSPoint> points) override;
  virtual vector<GPSPoint> decompress(ibstream& ibs) override;

private:
  Predictor *predictor;
  double max_temporal_err, max_spatial_err;

  int calculate_discarded_bits(double max_value, double error_bound);
};

#endif

