CC       = g++
CXXFLAGS = -Wall -O2 -std=c++14 -Isrc/third_party/cxxopts/include
LDFLAGS  = 

all: xcs xcsr

xcs: src/XCS/main.cpp
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

xcsr: src/XCSR/main.cpp
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm xcs xcsr
