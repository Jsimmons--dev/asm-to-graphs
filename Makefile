
all:
	make -C src

check:
	make -C tests check

clean:
	make -C src clean
	make -C tests clean

