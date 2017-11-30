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

#include "boost/date_time/posix_time/posix_time.hpp"
using boost::posix_time::microsec_clock;
using boost::posix_time::ptime;

#include "util.h"
#include "read_points.h"
#include "predictive_compressor.h"
#include "delta_compressor.h"
#include "dummy_compressor.h"
#include "dp_compressor.h"
#include "squish_compressor.h"

// 1 MiB buffer
const int BUFFER_SIZE = 1 * 1024 * 1024;
string alg;
string result_file;
void stats(string filename, Compressor *c)
{
  ptime start_time;
  long compr_time, decompr_time;

  vector<GPSPoint> points = read_points(filename);

  char* buffer = new char[BUFFER_SIZE];
  stream<basic_array<char> > bas(buffer, BUFFER_SIZE);

  obstream obs(&bas);

  start_time = microsec_clock::universal_time();
  c->compress(obs, points);
  compr_time = (microsec_clock::universal_time() -
                start_time).total_microseconds();
  obs.close();

  boost::iostreams::seek(bas, 0, std::ios_base::beg);
  ibstream ibs(&bas);
  start_time = microsec_clock::universal_time();
  vector<GPSPoint> new_points = c->decompress(ibs);
  decompr_time = (microsec_clock::universal_time() -
                  start_time).total_microseconds();
  ofstream fout(result_file);
  obstream obs_result(&fout);
  for(GPSPoint p : new_points)
  {
    //fout << noshowpoint << setprecision(15) << p.get_time() << showpoint << ","
    //     << p.get_latitude() << "," << p.get_longitude() << endl; 
      //fout <<p.get_latitude() << " " << p.get_longitude() << " "<< setprecision(15) << p.get_time() << endl;
	fout << noshowpoint <<p.get_latitude() << " "<< p.get_longitude() << " "<< setprecision(15) << p.get_time()  << showpoint<< endl;
  }
  int raw_size = points.size() * 8 * 3;
  int cmp_size = boost::iostreams::seek(bas, 0, std::ios_base::cur);
  fout << compr_time << " " << decompr_time << endl;
  fout << raw_size << " " << cmp_size << endl;
  obs_result.close();
  fout.close();
  delete[] buffer;
  /*
  double max_km = -1;

  // if(typeid(*c) == typeid(DPCompressor))
  // {
  //   max_km = dynamic_cast<DPCompressor*>(c)->max_error_kms;
  // }
  // else
  // {
  //   for(int i = 0; i < points.size(); ++i)
  //   {
  //     max_km = max(max_km, points[i].distance_kms(new_points[i]));
  //   }
  // }

  int new_points_index = 0;
  for(size_t i = 0; i < points.size(); ++i)
  {
    GPSPoint new_point = new_points[new_points_index];

    if(new_points_index > 0) {
      GPSPoint segment_start = new_points[new_points_index - 1];
      GPSPoint segment_end = new_points[new_points_index];
      double numerator = points[i].get_time() - segment_start.get_time();
      double denominator = segment_end.get_time() - segment_start.get_time();
      double time_ratio = denominator == 0 ? 1 : numerator / denominator;
      double lat = segment_start.get_latitude() +
        (segment_end.get_latitude() - segment_start.get_latitude()) * time_ratio;
      double lon = segment_start.get_longitude() +
        (segment_end.get_longitude() - segment_start.get_longitude()) * time_ratio;

      new_point = GPSPoint(points[i].get_time(), lat, lon);
    }

    max_km = max(max_km, points[i].distance_kms(new_point));

    if(points[i].get_time() >= new_points[new_points_index].get_time())
    {
      ++new_points_index;
    }
  }
  */
  /*
  cout << "raw_size=" << points.size() * 8 * 3 << endl;
  cout << "compr_size=" << boost::iostreams::seek(bas, 0, std::ios_base::cur)
       << endl;
  //cout << "max_error_kms=" << setprecision(10) << showpoint << max_km << endl;
  cout << "compr_time=" << compr_time << endl;
  cout << "decompr_time=" << decompr_time << endl;

  // Check for "pure" lossless (point for point correspondence)
  bool lossless = true;
  for(size_t i = 0; lossless and i < points.size(); ++i)
  {
    lossless = points[i].distance(new_points[i]) < 0.000000001;
  }
  cout << "lossless=" << boolalpha << lossless << endl;
  */
}

int main(int argc, char** args)
{
  if(argc < 4)
  {
    cout << "stats [trajic|delta|dp|dpd|squish] <infile> <result_file>"
         << " <max_temporal error=0> <max_spatial_error=0>" << endl;
  }
  else
  {
    alg = args[1];
    string infile = args[2];
    result_file = args[3];
    double mte = 0, mse = 0;

    if(argc > 4)
    {
      try
      {
        mte = boost::lexical_cast<double>(args[4]);
      }
      catch(boost::bad_lexical_cast const&) {}
    }

    if(argc > 5)
    {
      try
      {
        mse = boost::lexical_cast<double>(args[5]);
      }
      catch(boost::bad_lexical_cast const&) {}
    }

    if(alg == "trajic") // Trajic
    {
      PredictiveCompressor c(mte, mse);
      stats(infile, &c);
    }
    else if(alg == "dp") // Douglas-Peucker
    {
      DummyCompressor dc;
      DPCompressor c(&dc, mse);
      stats(infile, &c);
    }
    else if(alg == "dpd") // Douglas-Peucker with delta compression
    {
      DeltaCompressor dc;
      DPCompressor c(&dc, mse);
      stats(infile, &c);
    }
    else if(alg == "delta") // Pure delta compression
    {
      DeltaCompressor c;
      stats(infile, &c);
    }
    else if(alg == "squish") // SQUISH
    {
      SquishCompressor c(mse); // mse means compression_ratio here, I know it's bad
      stats(infile, &c);
    }
    else
    {
      cerr << "Invalid algorithm specified" << endl;
      return 1;
    }
  }

  return 0;
}
