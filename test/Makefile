include Makefile.test_source

INCLUDE_DIR = ../include ./munit

BUILD_TARGETS_ASM 	:= $(patsubst %.c, $(TEST_BUILD_DIR)/%.o, $(TEST_SRC))
BUILD_TARGETS 		:= $(patsubst %.S, $(TEST_BUILD_DIR)/%.o, $(BUILD_TARGETS_ASM))

BUILD_DEPENDENCIES_ASM	:= $(patsubst %.c, $(TEST_BUILD_DIR)/%.d, $(TEST_SRC))
BUILD_DEPENDENCIES		:= $(patsubst %.S, $(TEST_BUILD_DIR)/%.d, $(BUILD_DEPENDENCIES_ASM))

C_FILES := $(filter %.c, $(SRC_TARGETS))

-include $(BUILD_DEPENDENCIES)

$(KERNEL_TEST): $(BUILD_TARGETS)
	$(CC) -l$(KERNEL_TEST_LIB) $(CC_OPT) $(BUILD_TARGETS) -o $(KERNEL_TEST)

HEADER_FILES = $(shell find ../include -name "*.h")

format:
	clang-format -i --verbose $(C_FILES) $(HEADER_FILES)

format-check:
	clang-format --dry-run $(C_FILES) $(HEADER_FILES) --verbose -Werror

.PHONY: format format-check