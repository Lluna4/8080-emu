COMPILER_FLAGS = -std=c++23

all : main.cpp
	$(CXX) main.cpp -o  output/emu.out $(COMPILER_FLAGS)
	$(CXX) assembler.cpp -o  output/comp.out $(COMPILER_FLAGS)

clean:
	rm -rf *.out

re : clean all

.PHONY: all clean re
