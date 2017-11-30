#include "huffman.h"

namespace Huffman
{

Node<int>* create_tree(double *freqs, int n_freqs)
{
  auto comp = [](Node<int>* a, Node<int>* b)
    { return a->get_frequency() > b->get_frequency(); };
  priority_queue<Node<int>*, vector<Node<int>*>, decltype(comp)> pq(comp);

  for(int i = 0; i < n_freqs; ++i)
    pq.push(new Node<int>(i, freqs[i]));

  while(pq.size() > 1)
  {
    Node<int> *left = pq.top(); pq.pop();
    Node<int> *right = pq.top(); pq.pop();
    pq.push(new Node<int>(left, right));
  }

  return pq.top();
}

vector<string> canonicalize(pair<int, string> *cws, int n_codewords)
{
  sort(cws, cws + n_codewords,
  [](const pair<int, string>& a, const pair<int, string>& b) -> bool
  {
    return a.second.size() < b.second.size();
  });

  int code = 0;
  for(int i = 0; i < n_codewords; ++i)
  {
    boost::dynamic_bitset<> bits(cws[i].second.length(), code);
    string binary;
    boost::to_string(bits, binary);
    cws[i] = pair<int, string>(cws[i].first, binary);
    if(i < n_codewords - 1)
      code = (code + 1) <<
        (cws[i + 1].second.length() - cws[i].second.length());
  }

  sort(cws, cws + n_codewords,
  [](const pair<int, string>& a, const pair<int, string>& b) -> bool
  {
    return a.first < b.first;
  });
  vector<string> codewords(n_codewords);
  for(int i = 0; i < n_codewords; ++i)
    codewords[i] = cws[i].second;

  return codewords;
}

// Creates canonical codewords
vector<string> create_codewords(double *freqs, int n_freqs)
{
  typedef pair<Node<int>*, string> NodeCode;
  vector<string> codewords(n_freqs);
  queue<NodeCode> q;
  Node<int>* root = create_tree(freqs, n_freqs);
  q.push(NodeCode(root, ""));
  while(!q.empty())
  {
    NodeCode nc = q.front(); q.pop();
    if(nc.first->is_leaf())
    {
      codewords[nc.first->data] = nc.second;
    }
    else
    {
      q.push(NodeCode(nc.first->left, nc.second + "0"));
      q.push(NodeCode(nc.first->right, nc.second + "1"));
    }
  }

  auto cws = new pair<int, string>[n_freqs]; // max n_freqs
  for(int i = 0; i < n_freqs; ++i)
  {
    cws[i] = pair<int, string>(i, codewords[i]);
  }

  vector<string> canonical = canonicalize(cws, n_freqs);

  delete[] cws;

  return canonical;
}

}

