#ifndef _IBSTREAM_H_
#define _IBSTREAM_H_

#include <cstdint>
#include <cstring>
#include <iostream>
using namespace std;

#include <boost/dynamic_bitset.hpp>
using boost::dynamic_bitset;

class ibstream
{
public:
  ibstream(istream *is) : is(is) {}
  /// Reads a single bit
  bool read_bit();
  /// Reads the specified number of bits
  dynamic_bitset<> read_bits(size_t size);
  /// Reads a single byte, 8 bits
  uint8_t read_byte();
  /// Reads the specified number of bits as an integer
  uint64_t read_int(size_t size);
private:
  istream *is;
  uint8_t part = 0;
  int pos = 8;
};

#endif
