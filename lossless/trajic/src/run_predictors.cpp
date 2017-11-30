#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <typeinfo>
using namespace std;

#include <boost/lexical_cast.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
using boost::iostreams::basic_array;
using boost::iostreams::stream;

#include <boost/date_time/posix_time/posix_time.hpp>
using boost::posix_time::microsec_clock;
using boost::posix_time::ptime;

#include <jansson.h>

#include "util.h"
#include "read_points.h"

#include "linear_predictor.h"
#include "naive_linear_predictor.h"
#include "constant_predictor.h"

int residual_length(uint64_t residual) {
  if(residual > 0)
    return (int)log2(residual) + 1;
  else
    return 0;
}

json_t* run_predictor(Predictor *predictor, vector<GPSPoint> points) {
  ptime start_time = microsec_clock::universal_time();

  bits64 tuples[points.size()][3];

  for(size_t i = 0; i < points.size(); ++i) {
    tuples[i][0].dbl = points[i].get_time();
    tuples[i][1].dbl = points[i].get_latitude();
    tuples[i][2].dbl = points[i].get_longitude();
  }

  long temporal_residual_length_sum = 0;
  long spatial_residual_length_sum = 0;

  for(size_t i = 0; i < points.size() - 1; ++i) {
    bits64 pred[3];
    predictor->predict_coords(tuples, i + 1, pred);

    for(int j = 0; j < 3; ++j)
    {
      uint64_t residual = tuples[i + 1][j].lng ^ pred[j].lng;
      tuples[i + 1][j].lng = pred[j].lng ^ residual;

      if(j == 0)
        temporal_residual_length_sum += residual_length(residual);
      else
        spatial_residual_length_sum += residual_length(residual);
    }
  }

  long pred_time = (microsec_clock::universal_time() - start_time).total_microseconds();

  long n_temporal_residuals = (points.size() - 1);
  long n_spatial_residuals = 2 * (points.size() - 1);
  double mean_temporal_residual_length =
    (double)temporal_residual_length_sum / n_temporal_residuals;
  double mean_spatial_residual_length =
    (double)spatial_residual_length_sum / n_spatial_residuals;
  double mean_residual_length =
    (double)(temporal_residual_length_sum + spatial_residual_length_sum) /
    (n_temporal_residuals + n_spatial_residuals);

  return json_pack("{s:i, s:f, s:f, s:f}",
    "time_us", pred_time,
    "mean_temporal_residual_length", mean_temporal_residual_length,
    "mean_spatial_residual_length", mean_spatial_residual_length,
    "mean_residual_length", mean_residual_length);
}

void run_predictors(string filename)
{
  vector<GPSPoint> points = read_points(filename);

  json_t* trajectory_stats = json_pack("{s:i}",
    "n_points", points.size());

  json_t* linear_results = run_predictor(new LinearPredictor(), points);
  json_object_set(linear_results, "name", json_string("LinearPredictor"));

  json_t* naive_linear_results = run_predictor(new NaiveLinearPredictor(), points);
  json_object_set(naive_linear_results, "name", json_string("NaiveLinearPredictor"));

  json_t* constant_results = run_predictor(new ConstantPredictor(), points);
  json_object_set(constant_results, "name", json_string("ConstantPredictor"));

  json_t* predictor_results = json_pack("[o, o, o]",
    linear_results, naive_linear_results, constant_results);

  json_t* json_root = json_pack("{s:o, s:o}",
    "trajectory", trajectory_stats,
    "predictors", predictor_results);

  char* json_string = json_dumps(json_root, JSON_INDENT(2));
  printf("%s\n", json_string);
}

int main(int argc, char** args)
{
  if(argc < 2)
  {
    cout << "run_predictors <infile>" << endl;
  }
  else
  {
    string infile = args[1];

    run_predictors(infile);
  }

  return 0;
}
