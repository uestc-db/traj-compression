#include "util.h"

string file_basename(const string& filename)
{
  size_t lastdot = filename.find_last_of(".");
  if (lastdot == std::string::npos) return filename;
  return filename.substr(0, lastdot);
}

string file_ext(const string& filename)
{
  size_t lastdot = filename.find_last_of(".");
  if (lastdot == std::string::npos) return filename;
  return filename.substr(lastdot + 1, filename.length());
}
