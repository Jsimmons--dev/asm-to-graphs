
all: asm2cfg

check: check-asm2cfg

clean: clean-asm2cfg

########################

asm2cfg: asm2cfg.o
	c++ asm2cfg.o -o asm2cfg

asm2cfg.o: asm2cfg.cpp
	c++ -g -c asm2cfg.cpp -o asm2cfg.o

check-asm2cfg: asm2cfg

clean-asm2cfg:
	rm -f asm2cfg asm2cfg.o

