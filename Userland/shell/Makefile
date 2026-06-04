include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin

# Shell sources
SOURCES_C=$(wildcard *.c)
SOURCES_CMD=$(wildcard commands/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)

# Test sources
SOURCES_TEST=$(wildcard ../tests/src/*.c)

# Libc sources
SOURCES_LIBC=$(wildcard ../libc/src/*.c)

OBJECTS_C=$(SOURCES_C:.c=.o)
OBJECTS_CMD=$(SOURCES_CMD:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_TEST=$(SOURCES_TEST:.c=.o)
OBJECTS_LIBC=$(SOURCES_LIBC:.c=.o)

ALL_OBJECTS=$(OBJECTS_C) $(OBJECTS_CMD) $(OBJECTS_ASM) $(OBJECTS_TEST) $(OBJECTS_LIBC)

all: $(MODULE)

$(MODULE): $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(ALL_OBJECTS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I../libc/include -I. -c $< -o $@

commands/%.o: commands/%.c
	$(GCC) $(GCCFLAGS) -I../libc/include -I../tests/include -I. -Icommands -c $< -o $@

../tests/src/%.o: ../tests/src/%.c
	$(GCC) $(GCCFLAGS) -I../libc/include -I../tests/include -c $< -o $@

../libc/src/%.o: ../libc/src/%.c
	$(GCC) $(GCCFLAGS) -I../libc/include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o commands/*.o ../tests/src/*.o ../libc/src/*.o

.PHONY: all clean
