
TARGET=test 0A32eTdBKayjCWhZqDOQ 0ACDbR5M3ZhBJajygTuf

all: $(TARGET)

clean:
	rm -rf $(TARGET)

test: %: %.asm
	mkdir test ; cd test ; ../../src/asm2graphs ../test.asm ; cd ..

0A32eTdBKayjCWhZqDOQ 0ACDbR5M3ZhBJajygTuf: %: %.asm
	../scripts/generate.sh $<

