CFLAGS = -std=c99 -O3 -Wall
CXXFLAGS = -std=c++11 -O3 -Wall
OBJDIR = obj
SRCDIR = src
INCLUDEDIR = src
vpath %.c $(SRCDIR)
vpath %.cpp $(SRCDIR)
vpath %.h $(INCLUDEDIR)

OBJS = $(addprefix $(OBJDIR)/, ibstream.o obstream.o gps_point.o huffman.o plt_reader.o \
len_freq_div.o linear_predictor.o predictive_compressor.o illinois_reader.o \
util.o csv_reader.o dynamic_encoder.o)

STATS_OBJS = $(addprefix $(OBJDIR)/, delta_compressor.o dp_compressor.o dummy_compressor.o \
squish_compressor.o)

PRED_OBJS = $(addprefix $(OBJDIR)/, constant_predictor.o naive_linear_predictor.o)

default: trajic clean

all: trajic test experiments clean

test: $(OBJS) $(STATS_OBJS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) test/test.cpp -lboost_unit_test_framework $(OBJS) $(STATS_OBJS) -o test/test
	test/test

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir -p $@

trajic: $(OBJS) $(OBJDIR)/main.o
	$(CXX) $(OBJS) $(OBJDIR)/main.o -o bin/trajic

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -rf $(OBJDIR)

doc:
	doxygen

experiments: $(OBJS) $(STATS_OBJS) $(PRED_OBJS) $(OBJDIR)/stats.o $(OBJDIR)/run_predictors.o .force
	$(CXX) $(OBJS) $(STATS_OBJS) $(OBJDIR)/stats.o -o experiments/stats
	$(CXX) $(OBJS) $(PRED_OBJS) $(OBJDIR)/run_predictors.o -ljansson -o experiments/run_predictors

.force:
