
TARGET=test 0A32eTdBKayjCWhZqDOQ 0ACDbR5M3ZhBJajygTuf

check: $(TARGET)

clean:
	rm -rf $(TARGET)

current_dir=$(shell pwd)

test: %: %.asm ../src/asm2graphs-all
	mkdir test ; cd test ; ../../src/asm2graphs-all ../test.asm ; cd ..

0ACDbR5M3ZhBJajygTuf: 0ACDbR5M3ZhBJajygTuf.asm ../src/asm2graphs-all
	../scripts/generate.sh all $(current_dir) 0ACDbR5M3ZhBJajygTuf

0A32eTdBKayjCWhZqDOQ: 0A32eTdBKayjCWhZqDOQ.asm ../src/asm2graphs-all
	../scripts/generate.sh all $(current_dir) 0A32eTdBKayjCWhZqDOQ

../src/asm2graphs-all:
	make -C ../src asm2graphs-all

