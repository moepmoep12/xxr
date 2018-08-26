CC       = g++
CXXFLAGS = -Wall -O2 -std=c++14 -Ithird_party/cxxopts/include
LDFLAGS  = 

all: xcs xcsr

xcs: XCS/main.cpp
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

xcsr: XCSR/main.cpp
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm xcs xcsr
