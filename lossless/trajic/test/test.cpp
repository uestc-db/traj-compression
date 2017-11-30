#include "test.h"

BOOST_AUTO_TEST_CASE(test_ibstream) {
  char data[] = {0x41, 0x0F};
  stream<basic_array_source<char>> buffer(data, sizeof(data));
  ibstream ibs(&buffer);
  BOOST_CHECK_EQUAL(ibs.read_byte(), 'A');
  BOOST_CHECK_EQUAL(ibs.read_int(4), 15);
  BOOST_CHECK_EQUAL(ibs.read_bit(), false);
}

BOOST_AUTO_TEST_CASE(test_obstream_write_double) {
  char buffer[128] = {0};
  stream<basic_array<char> > bas(buffer, 128);
  obstream obs(&bas);

  // This double has a binary representation of
  // 0x4141414141414141 (note that 0x41 corresponds
  // to the ASCII character 'A').
  obs.write_double(2.2616345098039215e+06);
  obs.close();
  for(int i = 0; i < 8; ++i)
    BOOST_CHECK_EQUAL(buffer[i], 'A');
}

BOOST_AUTO_TEST_CASE(test_obstream_write_int) {
  char buffer[128] = {0};
  stream<basic_array<char> > bas(buffer, 128);
  obstream obs(&bas);

  obs.write_int(0x41, 7);
  obs.close();
  BOOST_CHECK_EQUAL(buffer[0], 'A');
}

BOOST_AUTO_TEST_CASE(test_gpspoint_get_time) {
  GPSPoint point(123, 50.7, 63.7);
  BOOST_CHECK_EQUAL(point.get_time(), 123);
}

BOOST_AUTO_TEST_CASE(test_gpspoint_distance) {
  GPSPoint p1(123, 50.7, 63.7);
  GPSPoint p2(234, 53.7, 67.7);

  BOOST_CHECK_CLOSE(p1.distance(p2), 5, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_huffman_node) {
  char c = 'X';
  Huffman::Node<char> n1(c, 0.4);
  Huffman::Node<char> n2(c, 0);
  Huffman::Node<char> n3(c, 0.2);
  Huffman::Node<char> n4(c, 0.3);
  n2.left = &n3;
  n2.right = &n4;
  BOOST_CHECK(n1 < n2);
}

BOOST_AUTO_TEST_CASE(test_huffman_codebook) {
  vector<char> alphabet;
  alphabet.push_back('A');
  alphabet.push_back('N');
  alphabet.push_back('T');
  char data[] = {0x02, (char)0xE9, (char)0xD2};
  stream<basic_array_source<char>> buffer(data, sizeof(data));
  ibstream ibs(&buffer);
  Huffman::Codebook<char> cb(alphabet, ibs);
  string msg = "";
  for(int i = 0; i < 6; ++i)
    msg += cb.lookup(ibs);
  BOOST_CHECK_EQUAL(msg, "TANANT");
}

BOOST_AUTO_TEST_CASE(test_huffman_create_codewords) {
  double freqs[] = {0.3, 0.6, 0.1};
  vector<string> codewords = Huffman::create_codewords(freqs, 3);
  BOOST_CHECK_EQUAL(codewords[0], "10");
  BOOST_CHECK_EQUAL(codewords[1], "0");
  BOOST_CHECK_EQUAL(codewords[2], "11");
}

BOOST_AUTO_TEST_CASE(test_len_freq_div) {
  double freqs[] = {0, 0.1, 0.6, 0.3, 0};
  LengthFrequencyDivider inst(freqs, 5, 5);
  inst.calculate();

  int expected[] = {2, 3};
  int actual[2];
  inst.get_dividers(actual, 2);
  bool same = true;
  for(int i = 0; same and i < 2; ++i)
    if(expected[i] != actual[i]) same = false;
  BOOST_CHECK(same);

  BOOST_CHECK_CLOSE(inst.get_cost(2), 1.1, 0.0001);
  BOOST_CHECK_CLOSE(inst.get_cost(4), 2, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_delta_compressor) {
  DeltaCompressor c;

  vector<GPSPoint> points;
  points.push_back(GPSPoint(1, 100, 200));
  points.push_back(GPSPoint(2, 103, 207));
  points.push_back(GPSPoint(2, 108, 206));

  char buffer[128] = {0};
  stream<basic_array<char> > bas(buffer, 128);
  obstream obs(&bas);
  c.compress(obs, points);
  obs.close();

  boost::iostreams::seek(bas, 0, std::ios_base::beg);
  ibstream ibs(&bas);
  vector<GPSPoint> new_points = c.decompress(ibs);

  for(size_t i = 0; i < points.size(); ++i)
    BOOST_CHECK(points[i].distance(new_points[i]) < 0.00000001);
}

BOOST_AUTO_TEST_CASE(test_squish_compressor) {
  SquishCompressor c(0.5);

  vector<GPSPoint> points;
  points.push_back(GPSPoint(1, 0, 0));
  points.push_back(GPSPoint(2, 1, 1));
  points.push_back(GPSPoint(3, 2, 2));
  points.push_back(GPSPoint(4, 3, 3));
  points.push_back(GPSPoint(5, 2, 2));
  points.push_back(GPSPoint(6, 1, 1));

  char buffer[1024] = {0};
  stream<basic_array<char> > bas(buffer, 1024);
  obstream obs(&bas);
  c.compress(obs, points);
  obs.close();

  boost::iostreams::seek(bas, 0, std::ios_base::beg);
  ibstream ibs(&bas);
  vector<GPSPoint> new_points = c.decompress(ibs);

  BOOST_CHECK_EQUAL(new_points.size(), 3);
  BOOST_CHECK(points[0].distance(new_points[0]) < 0.00000001);
  BOOST_CHECK(points[3].distance(new_points[1]) < 0.00000001);
  BOOST_CHECK(points[5].distance(new_points[2]) < 0.00000001);
}

BOOST_AUTO_TEST_CASE(test_dynamic_encoder) {
  uint64_t nums[] = {2, 1, 5, 67, 68, 4, 3, 2, 1, 73, 0, 2};

  char buffer[128] = {0};
  stream<basic_array<char> > bas(buffer, 128);
  obstream obs(&bas);

  DynamicEncoder enc(obs, nums, 12);
  for(int i = 0; i < 12; ++i)
    enc.encode(obs, nums[i]);

  obs.close();

  boost::iostreams::seek(bas, 0, std::ios_base::beg);
  ibstream ibs(&bas);

  DynamicEncoder dec(ibs);

  for(int i = 0; i < 12; ++i)
  {
    uint64_t num = dec.decode(ibs);
    BOOST_CHECK_EQUAL(num, nums[i]);
  }
}
