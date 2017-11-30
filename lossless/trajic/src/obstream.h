#ifndef _OBSTREAM_H_
#define _OBSTREAM_H_

#include <cstdint>
#include <cstring>
#include <iostream>
using namespace std;

class obstream
{
public:
  obstream(ostream *os) : os(os) {}
  /// Writes an integer using the specified number of bits
  void write_int(uint64_t val, size_t n_bits);
  /// Writes a double using 64 bits
  void write_double(double val);
  /// Writes a single bit
  void write_bit(bool val);
  /// Flushes and closes the stream
  void close();
private:
  ostream *os;
  int8_t part = 0;
  size_t pos = 0;
};

#endif
