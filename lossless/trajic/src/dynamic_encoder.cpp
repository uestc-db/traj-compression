#include "dynamic_encoder.h"

DynamicEncoder::DynamicEncoder(obstream& obs, uint64_t *nums, int len)
{
  double freqs[65] = {0};
  double step = 1.0 / len;
  int n_freqs = 0;

  for(int i = 0; i < len; ++i)
  {
    int min_len = 0;
    if(nums[i] > 0)
      min_len = (int)log2(nums[i]) + 1;
    freqs[min_len] += step;
    n_freqs = max(n_freqs, min_len + 1);
  }

  // Predict the number of dividers beyond which there will be no compression
  // gain.
  int max_divs = 0;
  for(int i = 0; i < n_freqs; ++i)
  {
    if(freqs[i] > 0.02) max_divs += 1;
  }
  if(max_divs < 4) max_divs = 4;
  if(max_divs > 32) max_divs = 32;

  int dividers[max_divs];
  double min_cost = numeric_limits<double>::max();
  LengthFrequencyDivider lfd(freqs, n_freqs, max_divs);
  lfd.calculate();

  int n_divs = max_divs;

  for(int n_codewords = 2; n_codewords <= max_divs; ++n_codewords)
  {
    double cost = lfd.get_cost(n_codewords) +
      7.0 * n_codewords / len;
    if(cost < min_cost)
    {
       min_cost = cost;
       n_divs = n_codewords;
       lfd.get_dividers(dividers, n_divs);
    }
    else
    {
      // Breaking seems to work here, but can't prove why.
      // break;
    }
  }

  double clumped_freqs[n_divs];
  int b = 0;
  for(int i = 0; i < n_freqs and b < n_divs; ++i)
  {
    clumped_freqs[b] += freqs[i];
    if(i == dividers[b]) ++b;
  }

  vector<int> div_vec;
  div_vec.assign(dividers, dividers + n_divs);
  codebook = new Huffman::Codebook<int>(div_vec,
    Huffman::create_codewords(clumped_freqs, n_divs));

  // Write out alphabet and codebook
  obs.write_int(codebook->get_alphabet().size(), 8);
  for(int symbol : codebook->get_alphabet())
    obs.write_int(symbol, 8);
  codebook->encode(obs);
}

DynamicEncoder::DynamicEncoder(ibstream& ibs)
{
  int alphabet_len = ibs.read_byte();
  vector<int> alphabet(alphabet_len);
  for(int i = 0; i < alphabet_len; ++i)
  {
    alphabet[i] = ibs.read_byte();
  }
  codebook = new Huffman::Codebook<int>(alphabet, ibs);
}

void DynamicEncoder::encode(obstream& obs, uint64_t num)
{
  vector<int> dividers = codebook->get_alphabet();
  int min_len = 0;
  if(num > 0)
    min_len = (int)log2(num) + 1;
  int index = 0;
  while(dividers[index] < min_len)
    ++index;
  for(char c : codebook->get_codewords()[index])
    obs.write_bit(c != '0');
  obs.write_int(num, dividers[index]);
}

uint64_t DynamicEncoder::decode(ibstream& ibs)
{
  int num_len = codebook->lookup(ibs);
  return ibs.read_int(num_len);
}

