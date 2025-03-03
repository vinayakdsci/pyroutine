CXX = g++

CXXFLAGS = -Wall -fcoroutines -pedantic -std=c++20 

TARGET = coro

all:
	$(TARGET)
	
$(TARGET):
	$(CXX) $(CXXFLAGS) -o $(TARGET).out ./*.cpp
