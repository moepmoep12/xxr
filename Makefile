CC      = g++
CFLAGS  = -Wall -O2 -std=c++14
LDFLAGS = 

all: xcs xcsr

xcs: XCS/main.cpp
	$(CC) -o $@ $^ $(LDFLAGS)

xcsr: XCSR/main.cpp
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm xcs xcsr