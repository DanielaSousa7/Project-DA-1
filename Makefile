CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-reorder -I.
DS_FLAGS = -std=c++17 -I. -w

TARGET = proj
SRCS   = main.cpp menu.cpp Parser.cpp Connection.cpp Output.cpp
OBJS   = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
