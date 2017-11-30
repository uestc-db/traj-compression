#include "ibstream.h"
#include "util.h"

bool ibstream::read_bit()
{
  if(pos > 7)
  {
    part = is->get();
    pos = 0;
  }

  return ((part >> pos++) & 1) == 1;
}

dynamic_bitset<> ibstream::read_bits(size_t size)
{
  dynamic_bitset<> bitset(size);
  for(size_t i = 0; i < size; ++i)
  {
    bool bit = read_bit();
    bitset[i] = bit;
  }
  return bitset;
}

uint8_t ibstream::read_byte()
{
  uint8_t n = 0;
  for(int i = 0; i < 8; ++i)
  {
    if(read_bit()) n |= 1 << i;
  }
  return n;
}

uint64_t ibstream::read_int(size_t size)
{
  uint64_t n = 0;
  for(size_t i = 0; i < size / 8; ++i)
  {
    uint64_t byte = read_byte();
    n |= byte << (i * 8);
  }
  for(size_t i = (size / 8) * 8; i < size; ++i)
  {
    if(read_bit())
      n |= 1ul << i;
  }
  return n;
}

