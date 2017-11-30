#ifndef _PLT_READER_H_
#define _PLT_READER_H_

#include "gps_reader.h"

/// PLTReader reads GPS points from a PLT file of the format used in the
/// Microsoft GeoLife data set
class PLTReader : public GPSReader
{
public:
  PLTReader(istream *is);
  virtual GPSPoint read_point() override;
};

#endif

