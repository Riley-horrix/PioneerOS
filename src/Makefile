include Makefile.source

BUILD_TARGETS_ASM 	:= $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_TARGETS))
BUILD_TARGETS 		:= $(patsubst %.S, $(BUILD_DIR)/%.o, $(BUILD_TARGETS_ASM))

BUILD_TARGETS_DEBUG_ASM := $(patsubst %.c, $(BUILD_DIR)/%_d.o, $(SRC_TARGETS))
BUILD_TARGETS_DEBUG 	:= $(patsubst %.S, $(BUILD_DIR)/%_d.o, $(BUILD_TARGETS_DEBUG_ASM))

C_FILES := $(filter %.c, $(SRC_TARGETS))

BUILD_DEPENDENCIES_ASM	:= $(patsubst %.c, $(BUILD_DIR)/%.d, $(SRC_TARGETS))
BUILD_DEPENDENCIES		:= $(patsubst %.S, $(BUILD_DIR)/%.d, $(BUILD_DEPENDENCIES_ASM))

BUILD_DEPENDENCIES_DEBUG_ASM 	:= $(patsubst %.c, $(BUILD_DIR)/%_d.d, $(SRC_TARGETS))
BUILD_DEPENDENCIES_DEBUG 		:= $(patsubst %.S, $(BUILD_DIR)/%_d.d, $(BUILD_DEPENDENCIES_DEBUG_ASM))

-include $(BUILD_DEPENDENCIES)
-include $(BUILD_DEPENDENCIES_DEBUG)

$(KERNEL): $(BUILD_TARGETS)
	$(LD) -T linker.ld -o $(KERNEL) $(LD_OPT) $(RELEASE) $(BUILD_TARGETS)

$(KERNEL_DEBUG): $(BUILD_TARGETS_DEBUG)
	$(LD) -T linker.ld -o $(KERNEL_DEBUG) $(LD_OPT) $(BUILD_TARGETS_DEBUG)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CC_OPT) -MMD -MP $(RELEASE) -o $@ $<

$(BUILD_DIR)/%.o: %.S
	$(CC) $(CC_OPT) -MMD -MP $(RELEASE) -o $@ $<

$(BUILD_DIR)/%_d.o: %.c
	$(CC) $(CC_OPT) -MMD -MP -g -o $@ $<

$(BUILD_DIR)/%_d.o: %.S
	$(CC) $(CC_OPT) -MMD -MP -g -o $@ $<

HEADER_FILES = $(shell find ../include -name "*.h")

format:
	clang-format -i --verbose $(C_FILES) $(HEADER_FILES)

format-check:
	clang-format --dry-run $(C_FILES) $(HEADER_FILES) --verbose -Werror

.PHONY: format format-check