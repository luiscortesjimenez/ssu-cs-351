#! make

CUDAFILES = $(wildcard *.cu)
CPPFILES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

STD = -std=c++17
OPT ?= -O2
CXXFLAGS = $(OPT) $(STD)

NVCC = nvcc
DIRT = $(wildcard *.i *.ppm *.cpu *.gpu)

default all: cpu gpu

cpu: $(CPPFILES:.cpp=.cpu)

%.cpu : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

gpu: $(CUDAFILES:.cu=.gpu)

%.gpu : %.cu $(HEADERS)
	$(NVCC) $(CXXFLAGS) $< -o $@

clean:
	$(RM) $(DIRT)

.PHONY: default all cpu gpu clean