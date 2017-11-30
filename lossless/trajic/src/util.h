#ifndef _UTIL_H_
#define _UTIL_H_

#include <cstdint>
#include <string>
using std::string;

/// A union for converting between doubles and longs
union bits64
{
  double dbl;
  uint64_t lng;
};

string file_basename(const string& filename);
string file_ext(const string& filename);

#endif

