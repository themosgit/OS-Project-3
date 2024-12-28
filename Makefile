# Directories
BUILD_DIR := ./build
SRC_DIRS := ./src

# Source files grouped by executable
SRCS_DRIVER := ./src/modules/driver.c ./src/utils/utils.c
SRCS_RECEPTIONIST := ./src/modules/receptionist.c ./src/utils/utils.c
SRCS_VISITOR := ./src/modules/visitor.c ./src/utils/utils.c
SRCS_MONITOR := ./src/utils/monitor.c ./src/utils/utils.c
SRCS_TEST := ./src/tests/test_utils.c ./src/utils/utils.c

# Object files grouped by executable
OBJS_DRIVER := $(SRCS_DRIVER:%=$(BUILD_DIR)/%.o)
OBJS_RECEPTIONIST := $(SRCS_RECEPTIONIST:%=$(BUILD_DIR)/%.o)
OBJS_VISITOR := $(SRCS_VISITOR:%=$(BUILD_DIR)/%.o)
OBJS_MONITOR := $(SRCS_MONITOR:%=$(BUILD_DIR)/%.o)
OBJS_TEST := $(SRCS_TEST:%=$(BUILD_DIR)/%.o)

# Dependencies
DEPS := $(OBJS_DRIVER:.o=.d) $(OBJS_RECEPTIONIST:.o=.d) $(OBJS_VISITOR:.o=.d) $(OBJS_MONITOR:.o=.d) $(OBJS_TEST:.o=.d)

# Include directories
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Flags
CPPFLAGS := $(INC_FLAGS) -MMD -MP
LDFLAGS := -lrt -lpthread -Wall -Werror

all: $(BUILD_DIR)/driver $(BUILD_DIR)/monitor $(BUILD_DIR)/test
# Build rules for executables
$(BUILD_DIR)/driver: $(OBJS_DRIVER)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/receptionist: $(OBJS_RECEPTIONIST)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/visitor: $(OBJS_VISITOR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/monitor: $(OBJS_MONITOR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/test: $(OBJS_TEST)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile source files into object files
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Include dependency files
-include $(DEPS)
