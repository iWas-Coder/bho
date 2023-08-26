# Name
APP = bho

# Version
VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = -rc1
NAME = Butane Hash Oil
ifeq ($(SUBLEVEL), 0)
	FULL_VERSION = $(VERSION).$(PATCHLEVEL)$(EXTRAVERSION)
else
	FULL_VERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
endif

# Makefile flags
ifndef VERBOSE
	MAKEFLAGS += --no-print-directory
endif

# Pretty Printing Output (PPO)
PPO_MKDIR = MKDIR
PPO_CLEAN = CLEAN
PPO_CC = CC
PPO_LD = LD

# Directories
SRC_DIR = src
HDR_DIR = include
BUILD_DIR = build

# Files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Compilation flags
CC = gcc
CPPFLAGS = -I $(HDR_DIR)
ifdef DEBUG
	CPPFLAGS += -DDEBUG
endif
CFLAGS = -Wall -Wextra -Werror -pedantic -std=gnu2x -O3 -flto -fno-semantic-interposition
LDFLAGS =


.PHONY: all clean mrproper help


# Root target
all: $(APP)

# Linking stage
$(APP): $(OBJS)
	@printf "  $(PPO_LD)\t$@\n"
	@$(CC) $^ $(LDFLAGS) -o $@

# Compilation stage
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if [ ! -d $(BUILD_DIR) ]; then                  \
		printf "  $(PPO_MKDIR)\t$(BUILD_DIR)\n"; \
		mkdir $(BUILD_DIR);                      \
	fi
	@printf "  $(PPO_CC)\t$@\n"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


# Clean build directory
clean:
	@if [ -d $(BUILD_DIR) ]; then                    \
		printf "  $(PPO_CLEAN)\t$(BUILD_DIR)\n"; \
		rm -r $(BUILD_DIR);                      \
	fi

# Clean final products (depends on 'build')
mrproper: clean
	@if [ -e $(APP) ]; then                    \
		printf "  $(PPO_CLEAN)\t$(APP)\n"; \
		rm $(APP);                         \
	fi

# Help
help:
	@printf "all\t\t- Build all targets marked with [*]\n"
	@printf "* $(APP)\t\t- Build the '$(APP)' binary\n"
	@printf "clean\t\t- Clean build directory\n"
	@printf "mrproper\t- Clean final products (depends on 'build')\n"
	@echo
	@printf "Execute 'make' or 'make all' to build all targets marked with [*]\n"
	@printf "For further info see the 'README.md' file and the documentation generated\n"
