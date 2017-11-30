#include "obstream.h"
#include "util.h"

void obstream::write_int(uint64_t val, size_t n_bits)
{
  part |= (val << pos) & 0xFF;
  val >>= 8 - pos;

  if(n_bits >= 8 - pos)
  {
    os->put(part);

    n_bits -= 8 - pos;

    for(size_t i = 0; i < n_bits / 8; ++i)
    {
      os->put(val & 0xFF);
      val >>= 8;
    }

    pos = n_bits % 8;
    part = val & (0xFF >> (8 - pos));
  }
  else
  {
    pos += n_bits;
  }
}

void obstream::write_double(double val)
{
  write_int(bits64{.dbl = val}.lng, 64);
}

void obstream::write_bit(bool val)
{
  write_int(val ? 1 : 0, 1);
}

/// This method must be called after writing concludes for the output to
/// be properly formed.
void obstream::close()
{
  if(pos > 0)
  {
    os->put(part);
    pos = 0;
  }
}

