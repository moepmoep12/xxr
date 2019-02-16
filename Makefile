CC       = g++
CXXFLAGS = -Wall -O2 -std=c++14 -Iinclude -Isrc/third_party/cxxopts/include
LDFLAGS  = 

all: xcs xcsr

xcs: src/xcs.cpp
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

xcsr: src/xcsr.cpp
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm xcs xcsr
