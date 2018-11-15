SRCS := test.cpp
HDRS :=
TRGT := PCAP
COMP := g++
FLAG := -lpcap -std=c++11 -Wall

all: build
		@echo "Compilation completed."

build:
		$(COMP) $(SRCS) $(FLAG) -o $(TRGT)
