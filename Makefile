CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -w

# Ficheiros fonte (apenas os que constam na imagem)
SRCS = Connection.cpp main.cpp menu.cpp Output.cpp Parser.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = myProg

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)