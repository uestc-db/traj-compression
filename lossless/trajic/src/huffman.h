#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include "ibstream.h"
#include "obstream.h"
using namespace std;

namespace Huffman
{

/// Node represents a node in a Huffman tree
template <class T>
class Node
{
public:
  Node<T> *left = nullptr;
  Node<T> *right = nullptr;
  T data;

  Node() = default;
  Node(T& data, double freq) : data(data), freq(freq) {}
  Node(Node<T>* left, Node<T>* right) : left(left), right(right) {}
  bool is_leaf() const { return left == nullptr and right == nullptr; }
  double get_frequency() const
  {
    return is_leaf() ? freq : left->get_frequency() + right->get_frequency();
  }

  /// Treverses the tree and deletes each node with the `delete` operator
  void delete_tree()
  {
    if(left != nullptr)
    {
      left->delete_tree();
      delete left;
    }
    if(right != nullptr)
    {
      right->delete_tree();
      delete right;
    }
  }
private:
  double freq;
};

template <class T>
bool operator<(const Node<T>& a, const Node<T>& b)
{
  return a.get_frequency() < b.get_frequency();
}

/// Constructs a Huffman tree from an array of frequencies
Node<int>* create_tree(double *freqs, int n_freqs);
/// Obtains the canonical representation of an array of Huffman codewords
vector<string> canonicalize(pair<int, string> *cws, int n_codewords);
/// Creates canonical codewords from an array of frequencies
vector<string> create_codewords(double *freqs, int n_freqs);

/// Codebook maps Huffman codewords to an alphabet
template <class T>
class Codebook
{
public:
  Codebook(vector<T> alphabet, vector<string> codewords)
    : alphabet(alphabet), codewords(codewords)
  {
    build_tree();
  }

  Codebook(vector<T> alphabet, ibstream& ibs)
    : alphabet(alphabet)
  {
    int n_codewords = alphabet.size();
    int max_len = ibs.read_byte();
    auto cws = new pair<int, string>[n_codewords];
    for(int i = 0; i < n_codewords; ++i)
    {
      int len = ibs.read_int(max_len);
      string binary = "";
      for(int j = 0; j < len; ++j)
        binary += '0';
      cws[i].first = i;
      cws[i].second = binary;
    }

    codewords = canonicalize(cws, n_codewords);

    delete[] cws;

    build_tree();
  }

  const vector<T>& get_alphabet() const
  {
    return alphabet;
  }

  const vector<string>& get_codewords() const
  {
    return codewords;
  }

  T& lookup(ibstream& ibs) const
  {
    Node<T>* node = root;
    while(!node->is_leaf())
      node = !ibs.read_bit() ? node->left : node->right;
    return node->data;
  }

  void encode(obstream& obs) const
  {
    size_t max_len = 0;
    for(string codeword : codewords)
      if(codeword.length() > max_len)
        max_len = codeword.length();
    max_len = (int)log2(max_len) + 1;
    obs.write_int(max_len, 8);
    for(string codeword : codewords)
      obs.write_int(codeword.length(), max_len);
  }

  ~Codebook()
  {
    root->delete_tree();
    delete root;
  }

private:
  vector<T> alphabet;
  vector<string> codewords;
  Node<T>* root;

  void build_tree()
  {
    root = new Node<T>;
    for(size_t i = 0; i < alphabet.size(); ++i)
    {
      Node<T>* node = root;
      for(char c : codewords[i])
      {
        Node<T>* n = c == '0' ? node->left : node->right;
        if(n == nullptr)
        {
          n = new Node<T>;
          if(c == '0')
            node->left = n;
          else
            node->right = n;
        }
        node = n;
      }
      node->data = alphabet[i];
    }
  }
};

}

#endif

