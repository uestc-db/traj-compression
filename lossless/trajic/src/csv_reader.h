#ifndef _CSV_READER_H_
#define _CSV_READER_H_

#include "gps_reader.h"

/// CSVReader reads GPS points from a CSV file with lines of the form
/// "<time>, <latitude>, <longitude>"
class CSVReader : public GPSReader
{
public:
  CSVReader(istream *is) : GPSReader(is) {}
  virtual GPSPoint read_point() override;
};

#endif

