include Makefile.inc

KERNEL=kernel.bin

SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_IDT=$(wildcard idt/*.c)
SOURCES_LIB=$(wildcard lib/*.c)
SOURCES_MEMORY=$(wildcard memory/*.c)
SOURCES_PROCESSES=$(wildcard processes/*.c)
SOURCES_SEMAPHORES=$(wildcard semaphores/*.c)
SOURCES_PIPES=$(wildcard pipes/*.c)

OBJECTS_C=$(SOURCES_C:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_IDT=$(SOURCES_IDT:.c=.o)
OBJECTS_LIB=$(SOURCES_LIB:.c=.o)
OBJECTS_MEMORY=$(SOURCES_MEMORY:.c=.o)
OBJECTS_PROCESSES=$(SOURCES_PROCESSES:.c=.o)
OBJECTS_SEMAPHORES=$(SOURCES_SEMAPHORES:.c=.o)
OBJECTS_PIPES=$(SOURCES_PIPES:.c=.o)

ALL_OBJECTS=$(OBJECTS_C) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_IDT) $(OBJECTS_LIB) $(OBJECTS_MEMORY) $(OBJECTS_PROCESSES) $(OBJECTS_SEMAPHORES) $(OBJECTS_PIPES)

LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(ALL_OBJECTS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(ALL_OBJECTS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(ALL_OBJECTS) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin *.elf drivers/*.o idt/*.o lib/*.o memory/*.o processes/*.o semaphores/*.o pipes/*.o

.PHONY: all clean
