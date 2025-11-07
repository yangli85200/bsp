# Makefile for driver-demo kernel module
# This Makefile can be used both standalone and within buildroot

obj-m += driver-demo.o

# For standalone compilation
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Default target for standalone build
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

help:
	@echo "Standalone build targets:"
	@echo "  make          - Build the kernel module"
	@echo "  make clean    - Clean build files"
	@echo "  make install  - Install the module to system"
	@echo ""
	@echo "Environment variables:"
	@echo "  KDIR          - Kernel build directory (default: /lib/modules/\$$(uname -r)/build)"
	@echo "  CROSS_COMPILE - Cross compiler prefix (optional)"
	@echo "  ARCH          - Target architecture (optional)"
	@echo ""
	@echo "Example for cross-compilation:"
	@echo "  make ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- KDIR=/path/to/kernel"

.PHONY: all clean install help

