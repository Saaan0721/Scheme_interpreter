CC=g++
CFLAGS=--std=c++14 
TARGET=main

$(TARGET):
	$(CC) $(CFLAGS) -g -o $@ main.cpp

clean:
	del $(TARGET).exe
