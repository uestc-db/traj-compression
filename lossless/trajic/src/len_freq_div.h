#ifndef _LEN_FREQ_DIV_H_
#define _LEN_FREQ_DIV_H_

/*
 * This class finds the optimal way to place dividers amongst an array of
 * bit-length frequencies using a variation of the linear partitioning
 * algorithm.
 */
class LengthFrequencyDivider
{
public:
  /*
   * freqs:     an array containing the distribution of residual bit-lengths
   *            (each element should be between 0 and 1, with all summing to 1)
   * n_freqs:   size of freqs array
   * max_divs:  the desired number of dividers
   * force_max: set to true if a divider must correspond to maximum
   *            possible length
   */
  LengthFrequencyDivider(double* freqs, int n_freqs,
                              int max_divs, bool force_max = false)
  : freqs(freqs), n_freqs(n_freqs), max_divs(max_divs), force_max(force_max)
  {
    costs = new double*[n_freqs];
    for(int i = 0; i < n_freqs; ++i)
      costs[i] = new double[max_divs];
    
    path = new int*[n_freqs];
    for(int i = 0; i < n_freqs; ++i)
      path[i] = new int[max_divs];
  }
  
  void calculate();
  void get_dividers(int* arr, int n_divs);
  double get_cost(int n_divs);
  
  ~LengthFrequencyDivider()
  {
    for(int i = 0; i < n_freqs; ++i)
    {
      delete[] costs[i];
      delete[] path[i];
    }
    delete[] costs;
    delete[] path;
  }
private:
  double* freqs;
  int n_freqs;
  int max_divs;
  bool force_max;
  double** costs;
  int** path;
  
  int last_div(int n_divs);
};

#endif

