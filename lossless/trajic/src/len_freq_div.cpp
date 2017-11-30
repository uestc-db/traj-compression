#include <limits>
#include <cmath>
using namespace std;

#include "len_freq_div.h"

void LengthFrequencyDivider::calculate()
{
  for(int i = 0; i < n_freqs; ++i)
  {
    costs[i][0] = 0;
    for(int y = 0; y < i; ++y)
      costs[i][0] += (i - y) * freqs[y];
  }
  
  for(int j = 1; j < max_divs; ++j)
    costs[0][j] = 0;
  
  for(int i = 1; i < n_freqs; ++i)
  {
    for(int j = 1; j < max_divs; ++j)
    {
      costs[i][j] = numeric_limits<double>::max();
      
      for(int x = j - 1; x < i; ++x)
      {
        double c = costs[x][j - 1];
        for(int y = x + 1; y < i; ++y)
        {
          c += (i - y) * freqs[y];
        }
        if(c < costs[i][j])
        {
          costs[i][j] = c;
          path[i][j] = x;
        }
      }
    }
  }
}

int LengthFrequencyDivider::last_div(int n_divs)
{
  int x = n_freqs - 1;
  if(!force_max)
    while(x > n_divs and freqs[x] == 0) --x;
  return x;
}

void LengthFrequencyDivider::get_dividers(int* arr, int n_divs)
{
  arr[n_divs - 1] = last_div(n_divs);
  for(int j = n_divs - 2; j >= 0; --j)
    arr[j] = path[arr[j + 1]][j + 1];
}

double LengthFrequencyDivider::get_cost(int n_divs)
{
  return costs[last_div(n_divs)][n_divs - 1] + log2(n_divs);
}

