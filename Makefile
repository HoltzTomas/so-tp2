MEMORY_MANAGER ?= FIRSTFIT

all: toolchain bootloader kernel userland image

buddy:
	$(MAKE) MEMORY_MANAGER=BUDDY all

toolchain:
	cd Toolchain/ModulePacker; make all

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all EXTRA_GCCFLAGS="-D$(MEMORY_MANAGER)"

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	cd Toolchain/ModulePacker; make clean

.PHONY: toolchain bootloader image kernel userland all buddy clean
