#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
using namespace std;

#include <boost/lexical_cast.hpp>

#include "util.h"
#include "read_points.h"
#include "predictive_compressor.h"

void compress(string filename, double mte=0, double mse=0)
{
  PredictiveCompressor c(mte, mse);

  cout << "Reading file..." << endl;
  vector<GPSPoint> points = read_points(filename);

  cout << "Compressing..." << endl;
  ofstream fout(file_basename(filename) + ".tjc");
  obstream obs(&fout);
  c.compress(obs, points);
  obs.close();
  fout.close();

  cout << "Done." << endl;
}

void decompress(string filename)
{
  PredictiveCompressor c;

  cout << "Decompressing..." << endl;
  ifstream fin(filename);
  ibstream ibs(&fin);
  vector<GPSPoint> points = c.decompress(ibs);
  fin.close();

  cout << "Writing file..." << endl;
  ofstream fout(file_basename(filename) + ".csv");
  obstream obs(&fout);
  for(GPSPoint p : points)
  {
    fout << noshowpoint << setprecision(15) << p.get_time() << showpoint << ","
         << p.get_latitude() << "," << p.get_longitude() << endl;
  }
  obs.close();
  fout.close();

  cout << "Done." << endl;
}

int main(int argc, char** args)
{
  if(argc < 3)
  {
    cout << "trajic <mode> <infile>"
         << " [<max_temporal error> <max_spatial_error>]" << endl;
  }
  else
  {
    string mode = args[1];
    string infile = args[2];
    if(mode == "c")
    {
      double mte = 0, mse = 0;

      if(argc > 3)
      {
        try
        {
          mte = boost::lexical_cast<double>(args[3]);
        }
        catch(boost::bad_lexical_cast const&) {}
      }

      if(argc > 4)
      {
        try
        {
          mse = boost::lexical_cast<double>(args[4]);
        }
        catch(boost::bad_lexical_cast const&) {}
      }

      compress(infile, mte, mse);
    }
    else if(mode == "d")
    {
      decompress(infile);
    }
    else
    {
      cout << "Accepted modes are [c]ompress and [d]ecompress" << endl;
    }
  }

  return 0;
}
