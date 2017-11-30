#ifndef _ILLINOIS_READER_H_
#define _ILLINOIS_READER_H_

#include "gps_reader.h"

/// IllinoisReader reads GPS points from the data set available at
/// http://www.cs.uic.edu/~boxu/mp2p/gps_data.html
class IllinoisReader : public GPSReader
{
public:
  IllinoisReader(istream *is) : GPSReader(is) {}
  virtual GPSPoint read_point() override;
};

#endif

