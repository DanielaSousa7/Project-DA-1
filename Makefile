CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

TARGET = DA_PROJECT_1
SRCS   = main.cpp Parser.cpp FlowNetwork.cpp RiskAnalysis.cpp data_structures/createGraphs.cpp
OBJS   = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
