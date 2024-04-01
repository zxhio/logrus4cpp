CXX = g++
CXXFLAGS = -std=c++17 -march=native -O2 -Wall -Werror

SRCS = \
	logrus.cpp \
	example.cpp \

OBJS = $(patsubst %.cpp, %.o, $(SRCS))
TARGETS = example

all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ -lfmt

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.log $(TARGETS) $(OBJS)