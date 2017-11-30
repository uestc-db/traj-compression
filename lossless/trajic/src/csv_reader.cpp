#include "csv_reader.h"

#include <string>
#include <ctime>
#include <limits>
using namespace std;

GPSPoint CSVReader::read_point()
{
  double time, lat, lon;
  *is >> lat;
  is->ignore(numeric_limits<streamsize>::max(), ' ');
  *is >> lon;
  is->ignore(numeric_limits<streamsize>::max(), ' ');
  *is >> time;
  is->ignore(numeric_limits<streamsize>::max(), '\n');

  return GPSPoint(time, lat, lon);
}


