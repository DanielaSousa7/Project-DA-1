CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-reorder -I.
DS_FLAGS = -std=c++17 -I. -w

TARGET = DA_PROJECT_1
SRCS   = main.cpp Application.cpp Menu.cpp Parser.cpp FlowNetwork.cpp RiskAnalysis.cpp
OBJS   = $(SRCS:.cpp=.o) data_structures/createGraphs.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

data_structures/createGraphs.o: data_structures/createGraphs.cpp
	$(CXX) $(DS_FLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
