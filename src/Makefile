
all: asm2graphs

check: check-asm2graphs

clean: clean-asm2graphs

########################

asm2graphs: asm2graphs.o
	c++ asm2graphs.o -o asm2graphs

asm2graphs.o: asm2graphs.cpp
	c++ -g -DJSON=1 -DVIZ=1 -DHIST=1 -c asm2graphs.cpp -o asm2graphs.o

check-asm2graphs: asm2graphs

clean-asm2graphs:
	rm -f asm2graphs asm2graphs.o

