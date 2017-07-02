#!/bin/bash
CPP = g++
CPP_tag = -std=c++11 -O3 -Wno-unknown-pragmas -Wall -fopenmp

MAIN_DIR = ~/github/pserver

INCLUDEPATH = -I/usr/local/include/ -I/usr/include -I$(MAIN_DIR)/ps-lite/src -I$(MAIN_DIR)/ps-lite/deps/include -I$(MAIN_DIR)/ps-lite/include -I./include

LIBRARY = $(MAIN_DIR)/ps-lite/deps/lib/libprotoc.a $(MAIN_DIR)/ps-lite/deps/lib/libprotobuf.a $(MAIN_DIR)/ps-lite/deps/lib/libzmq.a $(MAIN_DIR)/ps-lite/build/libps.a -lz -lpthread


all: ps_lr

ps_lr: main.o lr.o metric.o util.o $(LIBRARY)
	$(CPP) $(CPP_tag) -o $@ $^ $(LIBRARY)
	mkdir bin
	mv ps_lr bin

main.o: src/main.cpp
	$(CPP) $(CPP_tag) $(INCLUDEPATH) -c src/main.cpp

lr.o: src/lr.cpp
	$(CPP) $(CPP_tag) $(INCLUDEPATH) -c src/lr.cpp

metric.o: src/metric.cpp
	$(CPP) $(CPP_tag) $(INCLUDEPATH) -c src/metric.cpp

util.o: src/util.cpp
	$(CPP) $(CPP_tag) $(INCLUDEPATH) -c src/util.cpp


clean:
	rm -f src/*.o *.o
	rm -rf bin


