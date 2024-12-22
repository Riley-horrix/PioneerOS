# Raspberry PI Version To Build For
RPI_VERSION ?= 1

# Current Version of the Operating System
PIOS_VERSION := 0.0.1

# Define absolute paths to directories
ROOT_DIR	:= $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR 	:= $(ROOT_DIR)/build

# Cross compiler for code
CC_BASE ?= arm-none-eabi
CC 		= $(CC_BASE)-gcc
CC_ASM	= $(CC_BASE)-as
LD		= $(CC_BASE)-ld
OCPY	= $(CC_BASE)-objcopy

# Command line options for compiler
# -mcpu=arm1176jzf-s -mfpu=vfpv2 -march=armv6 
CC_OPT		= -mcpu=arm1176jzf-s -mfpu=vfpv2 -fpic -std=c17 -Wall -Wextra -nostdlib -nostartfiles -fasm -ffreestanding -c -I $(ROOT_DIR)/include
CC_ASM_OPT	= -mcpu=arm1176jzf-s -mfpu=vfpv2 
LD_OPT		= -nostdlib

RELEASE = -O2

# Command line options for QEMU
# -M = model
# -serial mon:stdio = redirect serial output to terminal
# -kernel
QEMU_OPT = -nographic -m 512 -M raspi1ap -serial mon:stdio

KERNEL			= $(BUILD_DIR)/kernel.elf
KERNEL_DEBUG	= $(BUILD_DIR)/kerneld.elf

export

all: $(BUILD_DIR) kernel

$(BUILD_DIR): $(BUILD_DIR)/boot $(BUILD_DIR)/common $(BUILD_DIR)/kernel
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%:
	mkdir -p $@

kernel: $(BUILD_DIR)
	@echo
	@echo Building PioneerOS
	@echo
	@echo Raspberry PI target   : $(RPI_VERSION)
	@echo PioneerOS Version     : $(PIOS_VERSION)
	@echo
	$(MAKE) -C ./src $(KERNEL)

kernel-debug: $(BUILD_DIR)
	$(MAKE) -C ./src $(KERNEL_DEBUG)

qemu: kernel
	qemu-system-arm $(QEMU_OPT) -kernel $(KERNEL) 

qemu-debug: kernel-debug
	qemu-system-arm $(QEMU_OPT) -S -s -d in_asm,cpu -D $(BUILD_DIR)/qemu.log -kernel $(KERNEL_DEBUG) 

lldb:
	lldb --arch armv6m --one-line "gdb-remote 1234" $(KERNEL_DEBUG)

format-check:
	$(MAKE) -C ./src format-check

format:
	$(MAKE) -C ./src format

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all kernel kernel-debug qemu qemu-debug lldb format format-check clean 