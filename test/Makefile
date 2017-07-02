#!/bin/bash
CPP = g++
CPP_tag = -std=c++11 -O3 -Wno-unknown-pragmas -Wall -fopenmp

MAIN_DIR = ~/github/pserver

INCLUDEPATH = -I$(MAIN_DIR)/ps-lite/src -I$(MAIN_DIR)/ps-lite/deps/include -I$(MAIN_DIR)/ps-lite/include

LIBRARY = $(MAIN_DIR)/ps-lite/deps/lib/libprotoc.a $(MAIN_DIR)/ps-lite/deps/lib/libprotobuf.a $(MAIN_DIR)/ps-lite/deps/lib/libzmq.a $(MAIN_DIR)/ps-lite/build/libps.a -lz -lpthread


all: ps_test

ps_test: test.o $(LIBRARY)
	$(CPP) $(CPP_tag) -o $@ $^ $(LIBRARY)

test.o: test.cpp
	$(CPP) $(CPP_tag) $(INCLUDEPATH) -c test.cpp


clean:
	rm -f *.o ps_test


