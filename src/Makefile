
all: asm2graphs-all asm2graphs-hist asm2graphs-json asm2graphs-viz

clean:
	rm -f asm2graphs-all asm2graphs-hist asm2graphs-json asm2graphs-viz

########################

asm2graphs-all: asm2graphs.cpp
	c++  -g -DJSON=1 -DVIZ=1 -DHIST=1 asm2graphs.cpp -o asm2graphs-all

asm2graphs-hist: asm2graphs.cpp
	c++ -O3 -DJSON=0 -DVIZ=0 -DHIST=1 asm2graphs.cpp -o asm2graphs-hist

asm2graphs-json: asm2graphs.cpp
	c++ -O3 -DJSON=1 -DVIZ=0 -DHIST=0 asm2graphs.cpp -o asm2graphs-json

asm2graphs-viz: asm2graphs.cpp
	c++ -O3 -DJSON=0 -DVIZ=1 -DHIST=0 asm2graphs.cpp -o asm2graphs-viz

