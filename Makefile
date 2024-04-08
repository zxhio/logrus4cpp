BENCHMARK_INCLUDE = third-party/benchmark/include
BENCHMARK_LIB = third-party/benchmark/build/src

CXXFLAGS += -I $(BENCHMARK_INCLUDE) -Wall -Werror -O2 -g -std=c++17
LDFLAGS += -L./ -llogrus -lfmt

LIB_OBJS	   = logrus.o
EXAMPLE_OBJS   = example.o
BENCHMARK_OBJS = benchmark.o

LIB_TARGET       = liblogrus.a
EXAMPLE_TARGET   = example
BENCHMARK_TARGET = benchmark

%.o: %.cpp
	g++ $(CXXFLAGS) -c $< -o $@

%.d: %.cpp
	g++ -MM $(CXXFLAGS) $< > $@

all: $(LIB_TARGET) $(EXAMPLE_TARGET) $(BENCHMARK_TARGET)

$(EXAMPLE_TARGET): $(EXAMPLE_OBJS) $(LIB_TARGET)
	g++ -o $@ $(patsubst %.o,%.o,$(^F)) $(LDFLAGS)

$(BENCHMARK_TARGET): $(BENCHMARK_OBJS) $(LIB_TARGET)
	g++ -o $@ $(patsubst %.o,%.o,$(^F)) $(LDFLAGS) -L$(BENCHMARK_LIB) -lbenchmark

$(LIB_TARGET): $(LIB_OBJS)
	ar rcs $@ $^

.PHONY: clean
clean:
	rm -f $(LIB_TARGET) $(EXAMPLE_TARGET) $(BENCHMARK_TARGET) *.o *.d *.a *.log

-include $(patsubst %.cpp, %.d, $(wildcard *.cpp))