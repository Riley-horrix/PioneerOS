# Raspberry PI Version To Build For
RPI_VERSION ?= 1

# Current Version of the Operating System
PIOS_VERSION := 0.0.1

# Define absolute paths to directories
ROOT_DIR	:= $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR 	:= $(ROOT_DIR)/build
TEST_BUILD_DIR := $(ROOT_DIR)/build/test

# Cross compiler for code
CC_BASE ?= arm-none-eabi
CC 		= $(CC_BASE)-gcc
CC_ASM	= $(CC_BASE)-as
LD		= $(CC_BASE)-ld
OCPY	= $(CC_BASE)-objcopy
ODMP 	= $(CC_BASE)-objdump

# Command line options for compiler
CC_OPT		= -mcpu=arm1176jzf-s -mfpu=vfpv2 -mfloat-abi=hard -std=c17 -Wall -Wextra -Werror -nostdlib -nostartfiles -fasm -ffreestanding -c -I $(ROOT_DIR)/include -DRPI_VERSION=$(RPI_VERSION)
CC_ASM_OPT	= -mcpu=arm1176jzf-s -mfpu=vfpv2 
LD_OPT		= -nostdlib

ODMP_OPT	= -m arm -SwCrR

RELEASE = -O1

# Command line options for QEMU
# -M = model
# -serial mon:stdio = redirect serial output to terminal
# -kernel
DTB ?= qemu/bcm2708-rpi-b-plus.dtb
SD ?= qemu/raspios.img
QEMU_OPT = -m 512 -M raspi1ap -serial mon:stdio -dtb $(DTB) -drive file=$(SD),if=sd,format=raw

KERNEL			= $(BUILD_DIR)/kernel.elf
KERNEL_DEBUG	= $(BUILD_DIR)/kerneld.elf
KERNEL_IMG 		= $(BUILD_DIR)/kernel.img
KERNEL_ASM		= $(BUILD_DIR)/kernel.asm

KERNEL_TEST_LIB	= $(BUILD_DIR)/kernel_lib.a

KERNEL_BASE_ADDR = 0x10000

export

all: $(BUILD_DIR) kernel

$(BUILD_DIR): $(BUILD_DIR)/boot $(BUILD_DIR)/common $(BUILD_DIR)/kernel $(BUILD_DIR)/drivers
	mkdir -p $(BUILD_DIR)

$(TEST_BUILD_DIR): $(TEST_BUILD_DIR)/src/common
	mkdir -p $(TEST_BUILD_DIR)

$(BUILD_DIR)/%:
	mkdir -p $@

$(TEST_BUILD_DIR)/%:
	mkdir -p $@

kernel: $(BUILD_DIR)
	@echo
	@echo Building PioneerOS
	@echo
	@echo Raspberry PI target : $(RPI_VERSION)
	@echo PioneerOS Version : $(PIOS_VERSION)
	@echo
	$(MAKE) -C ./src $(KERNEL)

kernel-debug: $(BUILD_DIR)
	$(MAKE) -C ./src $(KERNEL_DEBUG)

qemu: kernel
	qemu-system-arm -nographic $(QEMU_OPT) -device loader,file=$(KERNEL_IMG),addr=$(KERNEL_BASE_ADDR),cpu-num=0

qemu-console: kernel
	qemu-system-arm $(QEMU_OPT) -device loader,file=$(KERNEL_IMG),addr=$(KERNEL_BASE_ADDR),cpu-num=0

qemu-debug: kernel-debug kernel
	qemu-system-arm -nographic $(QEMU_OPT) -S -s -device loader,file=$(KERNEL_DEBUG),addr=$(KERNEL_BASE_ADDR),cpu-num=0

lldb:
	lldb --arch armv6m --one-line "gdb-remote 1234" $(KERNEL_DEBUG)

check: CC = gcc
check: CC_ASM = gcc
check: CC_OPT = -Wall -Wextra -Werror -std=c17
check: CC_ASM_OPT = -Wall -Wextra -Werror
check: $(BUILD_DIR) $(TEST_BUILD_DIR)
	@echo
	@echo Building PioneerOS Testing Suite
	@echo
	@echo PioneerOS Version : $(PIOS_VERSION)
	@echo
	$(MAKE) -C ./src $(KERNEL_TEST_LIB)
	$(MAKE) -C ./src $(KERNEL_TEST)
	./$(KERNEL_TEST)

format-check:
	$(MAKE) -C ./src format-check
	$(MAKE) -C ./test format-check

format:
	$(MAKE) -C ./src format
	$(MAKE) -C ./test format

doc:
	doxygen

download-doc:
	@echo "--TODO--"

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all kernel kernel-debug qemu qemu-debug lldb format format-check clean 