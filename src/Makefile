
all: asm2graphs asm2graphs-hist asm2graphs-json asm2graphs-viz

check: check-asm2graphs

clean: clean-asm2graphs
	rm -f asm2graphs-hist asm2graphs-json asm2graphs-viz

########################

asm2graphs: asm2graphs.o
	c++ asm2graphs.o -o asm2graphs

asm2graphs.o: asm2graphs.cpp
	c++ -g -DJSON=1 -DVIZ=1 -DHIST=1 -c asm2graphs.cpp -o asm2graphs.o

asm2graphs-hist: asm2graphs.cpp
	c++ -O3 -DJSON=0 -DVIZ=0 -DHIST=1 asm2graphs.cpp -o asm2graphs-hist

asm2graphs-json: asm2graphs.cpp
	c++ -O3 -DJSON=1 -DVIZ=0 -DHIST=0 asm2graphs.cpp -o asm2graphs-json

asm2graphs-viz: asm2graphs.cpp
	c++ -O3 -DJSON=0 -DVIZ=1 -DHIST=0 asm2graphs.cpp -o asm2graphs-viz

check-asm2graphs: asm2graphs

clean-asm2graphs:
	rm -f asm2graphs asm2graphs.o

