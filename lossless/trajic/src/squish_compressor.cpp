#include "squish_compressor.h"

#include <cmath>

typedef struct {
  GPSPoint point;
  double sed;
} GPSPointWithSED;

// FIXME: Results look weird, is this broken?
void SquishCompressor::compress(obstream& obs, vector<GPSPoint> points)
{
  size_t max_buffer_size = (size_t)(compression_ratio * points.size());
  vector<GPSPointWithSED> buffer;
  buffer.reserve(max_buffer_size + 1);

  buffer.push_back({points[0], 0});

  if(max_buffer_size > 2) {
    buffer.push_back({points[1], 0});

    for(size_t i = 2; i < points.size(); ++i)
    {
      buffer.push_back({points[i], 0});

      // Compute SED for previous point
      GPSPoint segment_start = buffer[buffer.size() - 3].point;
      GPSPointWithSED* previous = &buffer[buffer.size() - 2];
      GPSPoint segment_end = buffer[buffer.size() - 1].point;
      double numerator = previous->point.get_time() - segment_start.get_time();
      double denominator = segment_end.get_time() - segment_start.get_time();
      double time_ratio = denominator == 0 ? 1 : numerator / denominator;
      double lat = segment_start.get_latitude() +
        (segment_end.get_latitude() - segment_start.get_latitude()) * time_ratio;
      double lon = segment_start.get_longitude() +
        (segment_end.get_longitude() - segment_start.get_longitude()) * time_ratio;
      GPSPoint previous_approx(previous->point.get_time(), lat, lon);
      previous->sed += previous_approx.distance(previous->point);

      // Buffer full, remove a point
      if(buffer.size() > max_buffer_size) {
        auto start = buffer.begin();
        ++start;
        auto to_remove = buffer.end();
        for(auto it = buffer.begin() + 1; it != buffer.end() - 1; ++it) {
          if(to_remove == buffer.end() || it->sed < to_remove->sed) {
            to_remove = it;
          }
        }
        (to_remove - 1)->sed += to_remove->sed;
        (to_remove + 1)->sed += to_remove->sed;
        buffer.erase(to_remove);
      }
    }
  } else {
    buffer.push_back({points[points.size() - 1], 0});
  }

  obs.write_int(buffer.size(), 32);

  for(size_t i = 0; i < buffer.size(); ++i)
  {
    obs.write_double(buffer[i].point.get_time());
    obs.write_double(buffer[i].point.get_latitude());
    obs.write_double(buffer[i].point.get_longitude());
  }
}

vector<GPSPoint> SquishCompressor::decompress(ibstream& ibs)
{
  // Old dummy implementation
  int size = ibs.read_int(32);
  vector<GPSPoint> points;

  bits64 tuple[3];

  for(int i = 0; i < size; ++i)
  {
    tuple[0].lng = ibs.read_int(64);
    tuple[1].lng = ibs.read_int(64);
    tuple[2].lng = ibs.read_int(64);
    points.push_back(GPSPoint(tuple[0].dbl, tuple[1].dbl, tuple[2].dbl));
  }

  return points;
}

