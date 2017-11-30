#ifndef _CREATE_READER_H_
#define _CREATE_READER_H_

#include "util.h"
#include "plt_reader.h"
#include "illinois_reader.h"
#include "csv_reader.h"

/// Attempts to create an appropriate reader for the specified file, then
/// uses it to read the trajectory
vector<GPSPoint> read_points(const string& filename)
{
  ifstream fin(filename);
  GPSReader* reader;

  /*if(file_ext(filename) == "plt")
    reader = new PLTReader(&fin);
  else if(file_ext(filename) == "txt")
    reader = new IllinoisReader(&fin);
  else*/
    reader = new CSVReader(&fin);
  
  vector<GPSPoint> points = reader->read_points();
  fin.close();
  delete reader;
  
  return points;
}

#endif

