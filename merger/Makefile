CXX = g++
CXX_FLAGS = -O2 -Wall -std=c++0x -DGEN_DETAIL=0
# assign GEN_DETAIL=1 to get more information, but larger log

SOURCES = combine.cpp trace2symbol.cpp pattern.cpp
TARGETS = $(SOURCES:.cpp=)

.PHONY: tests

%: %.cpp
	$(CXX) $^ $(INCS) $(LIBS) -o $@ $(CXX_FLAGS)

all: $(TARGETS)

tests:
	make -C tests

clean:
	rm -rf $(TARGETS)
	make -C tests clean
