PI_VERSION ?= 1

CC=arm-none-eabi

CPU=arm1176jzf-s

SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) \
			$(wildcard $(SRC_DIR)/*.S) \
			$(wildcard $(SRC_DIR)/*/*.c) \
			$(wildcard $(SRC_DIR)/*/*.S)

SRC_BUILD = $(patsubst %.S, %_S.o, $(patsubst %.c, %_c.o, $(SRC_FILES)))

OBJ_SRC = $(foreach src, $(SRC_BUILD), $(subst $(SRC_DIR)/, $(BUILD_DIR)/, $(src)))

DEP_SRC = $(OBJ_SRC:%.o=%.d)
-include $(DEP_SRC)

BUILD_DIR = build
TEST_DIR = test

INCLUDE = -Iinclude

CC_OPTS = -DPI_VERSION=$(PI_VERSION) -Wall -Werror -nostdlib -nostartfiles \
	-ffreestanding $(INCLUDE) -mgeneral-regs-only -mcpu=$(CPU)

ASM_OPTS = $(INCLUDE)

TARGET = kernel-pioneer.img

all: $(TARGET)

clean:
	@rm -rf $(BUILD_DIR) *.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC)-gcc $(CC_OPTS) -c $< -o $@

$(BUILD_DIR)/%_S.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC)-gcc $(ASM_OPTS) -c $< -o $@

$(TARGET): $(SRC_DIR)/linker.ld $(OBJ_SRC) config.txt
	@echo ""
	@echo "Building kernel for the RaspberryPI $(PI_VERSION)"
	@echo ""
	$(CC)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel.elf $(OBJ_SRC)
	$(CC)-objcopy $(BUILD_DIR)/kernel.elf -O binary $(TARGET)

.PHONY: clean all