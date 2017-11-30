Trajic
======

[![Build Status](https://travis-ci.org/anibali/trajic.svg?branch=master)](https://travis-ci.org/anibali/trajic)

Trajic is an algorithm for compressing GPS trajectory data. For a more in-depth
academic explanation of how Trajic works, be sure to read the
[research paper](https://raw.githubusercontent.com/anibali/trajic/gh-pages/trajic_paper.pdf)
I wrote with [Dr Zhen He](http://homepage.cs.latrobe.edu.au/zhe/).

This project contains a reference implementation of Trajic along with
implementations of various other GPS trajectory compression schemes which were
used for benchmarks and experiments.

Development
-----------

### Compiling

```sh
make
```

The default `make` task just builds the `trajic` binary.

```sh
make all
```

The `all` task runs tests and produces both the `trajic` and `stats` binaries
(the latter of which is used for running experiments).

### Dependencies

* [libboost-test-dev](http://www.boost.org/doc/libs/1_54_0/libs/test/doc/html/index.html)
* [libboost-iostreams-dev](http://www.boost.org/doc/libs/1_54_0/libs/iostreams/doc/index.html)

Experiments also require

* [Ruby](https://www.ruby-lang.org/)
* [gnuplot](http://www.gnuplot.info/)
* [libjansson-dev](http://www.digip.org/jansson/)

Usage
-----

Compress a trajectory losslessly:

    trajic c traj.plt

Compress a trajectory with max errors of 0.1 s temporally and 0.001 degrees
spatially:

    trajic c traj.plt 0.1 0.001

Decompress a trajectory:

    trajic d traj.tjc

Important classes
-----------------

* @ref PredictiveCompressor
* @ref ibstream
* @ref obstream
* @ref GPSPoint
