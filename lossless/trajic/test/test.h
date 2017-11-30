#ifndef __TEST_H__
#define __TEST_H__

#define BOOST_TEST_MODULE Trajic
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "gps_point.h"
#include "huffman.h"
#include "len_freq_div.h"
#include "delta_compressor.h"
#include "squish_compressor.h"
#include "dynamic_encoder.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
using boost::iostreams::basic_array;
using boost::iostreams::basic_array_source;
using boost::iostreams::stream;

#endif
