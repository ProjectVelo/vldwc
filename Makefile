# Project configuration
PROJECT_NAME = vldwc
VERSION = 1.0.0

# FreeBSD-specific compiler and tools
CC ?= clang
AR ?= ar
STRIP ?= strip
INSTALL ?= install

# Directories
SRC_DIR = .
BUILD_DIR = build
INCLUDE_DIR = include
DIST_DIR = dist
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

# Source files
CORE_SRC = core/compositor.c core/render_sdl.c core/background.c
MAIN_SRC = main.c
SRC = $(CORE_SRC) $(MAIN_SRC)

# Object files with build directory
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

# Binary output
BIN = $(BUILD_DIR)/$(PROJECT_NAME)

# FreeBSD-specific compiler flags
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -Wformat=2 -Wstrict-prototypes
CFLAGS += -I$(INCLUDE_DIR) -I$(PREFIX)/include
CPPFLAGS = -MMD -MP

# FreeBSD-specific library paths
LDFLAGS ?= -L$(PREFIX)/lib
LDLIBS = -lSDL2

# Build type configurations
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG -fomit-frame-pointer
PROFILE_FLAGS = -O2 -g -pg

# FreeBSD system detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),FreeBSD)
    # FreeBSD-specific optimizations
    CFLAGS += -march=native -mtune=native
    
    # Use pkg-config if available
    PKG_CONFIG ?= pkg-config
    ifeq ($(shell which $(PKG_CONFIG) 2>/dev/null),)
        # Fallback for systems without pkg-config
        CFLAGS += -I/usr/local/include/SDL2
        LDLIBS = -L/usr/local/lib -lSDL2
    else
        CFLAGS += $(shell $(PKG_CONFIG) --cflags sdl2)
        LDLIBS = $(shell $(PKG_CONFIG) --libs sdl2)
    endif
endif

# Build type (default: release)
BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE),debug)
    CFLAGS += $(DEBUG_FLAGS)
else ifeq ($(BUILD_TYPE),profile)
    CFLAGS += $(PROFILE_FLAGS)
else
    CFLAGS += $(RELEASE_FLAGS)
endif

# Default target
.DEFAULT_GOAL := all

# Main targets
all: $(BIN)

$(BIN): $(OBJ) | $(BUILD_DIR)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Object file compilation with dependency generation
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR) $(BUILD_DIR)/core

# Development targets
debug:
	@$(MAKE) BUILD_TYPE=debug

release:
	@$(MAKE) BUILD_TYPE=release

profile:
	@$(MAKE) BUILD_TYPE=profile

# Static analysis (FreeBSD ports)
analyze:
	@echo "Running static analysis..."
	@if which cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 $(SRC); \
	else \
		echo "cppcheck not found. Install with: pkg install cppcheck"; \
	fi
	@if which clang-tidy >/dev/null 2>&1; then \
		clang-tidy $(SRC) -- $(CFLAGS); \
	else \
		echo "clang-tidy not found. Install with: pkg install llvm"; \
	fi

# Code formatting
format:
	@echo "Formatting code..."
	@if which clang-format >/dev/null 2>&1; then \
		clang-format -i $(SRC); \
	else \
		echo "clang-format not found. Install with: pkg install llvm"; \
	fi

# Testing
test: debug
	@echo "Running tests..."
	@./$(BIN) --test || echo "No tests implemented"

# Memory check (FreeBSD)
memcheck: debug
	@echo "Running valgrind..."
	@if which valgrind >/dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all ./$(BIN); \
	else \
		echo "valgrind not found. Install with: pkg install valgrind"; \
	fi

# Coverage analysis
coverage:
	@$(MAKE) BUILD_TYPE=debug CFLAGS="$(CFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage"
	@echo "Running coverage analysis..."
	@./$(BIN) || true
	@if which gcov >/dev/null 2>&1; then \
		gcov $(SRC); \
	else \
		echo "gcov not found. Install with: pkg install gcc"; \
	fi

# FreeBSD-style installation
install: release
	@echo "Installing $(PROJECT_NAME)..."
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -s -m 755 $(BIN) $(DESTDIR)$(BINDIR)/$(PROJECT_NAME)

deinstall:
	@echo "Deinstalling $(PROJECT_NAME)..."
	rm -f $(DESTDIR)$(BINDIR)/$(PROJECT_NAME)

# Create FreeBSD port-style distribution
dist: clean
	@echo "Creating distribution..."
	@mkdir -p $(DIST_DIR)
	@tar -czf $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION).tar.gz \
		--exclude=.git --exclude=$(BUILD_DIR) --exclude=$(DIST_DIR) \
		--transform 's,^,$(PROJECT_NAME)-$(VERSION)/,' *

# Generate FreeBSD port Makefile template
port-makefile:
	@echo "# FreeBSD Port Makefile for $(PROJECT_NAME)"
	@echo "PORTNAME=	$(PROJECT_NAME)"
	@echo "PORTVERSION=	$(VERSION)"
	@echo "CATEGORIES=	x11-wm"
	@echo "MASTER_SITES=	LOCAL"
	@echo ""
	@echo "MAINTAINER=	your-email@example.com"
	@echo "COMMENT=	Wayland compositor"
	@echo ""
	@echo "LICENSE=	MIT"
	@echo ""
	@echo "USES=		gmake"
	@echo "USE_SDL=	sdl2"
	@echo ""
	@echo "PLIST_FILES=	bin/$(PROJECT_NAME)"
	@echo ""
	@echo ".include <bsd.port.mk>"

# Cleanup
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)

distclean: clean
	@echo "Cleaning distribution files..."
	rm -rf $(DIST_DIR)

# Development utilities
run: $(BIN)
	@echo "Running $(PROJECT_NAME)..."
	@./$(BIN)

gdb: debug
	@echo "Starting GDB..."
	@if which gdb >/dev/null 2>&1; then \
		gdb ./$(BIN); \
	else \
		echo "gdb not found. Install with: pkg install gdb"; \
	fi

lldb: debug
	@echo "Starting LLDB..."
	@if which lldb >/dev/null 2>&1; then \
		lldb ./$(BIN); \
	else \
		echo "lldb not found. Install with: pkg install llvm"; \
	fi

# System information
sysinfo:
	@echo "System Information:"
	@echo "OS: $(shell uname -s)"
	@echo "Release: $(shell uname -r)"
	@echo "Architecture: $(shell uname -m)"
	@echo "Compiler: $(CC) $(shell $(CC) --version | head -1)"
	@if which pkg >/dev/null 2>&1; then \
		echo "Package manager: pkg"; \
	fi

# Dependency check
deps-check:
	@echo "Checking dependencies..."
	@echo -n "SDL2: "
	@if $(PKG_CONFIG) --exists sdl2 2>/dev/null; then \
		echo "found ($(shell $(PKG_CONFIG) --modversion sdl2))"; \
	else \
		echo "missing - install with: pkg install sdl2"; \
	fi

# Install FreeBSD development dependencies
deps-install:
	@echo "Installing development dependencies..."
	@if [ "$(shell id -u)" -eq 0 ]; then \
		pkg install -y sdl2 gmake pkgconf; \
	else \
		echo "Run as root or use: sudo make deps-install"; \
	fi

info:
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Compiler: $(CC)"
	@echo "Build type: $(BUILD_TYPE)"
	@echo "Source files: $(SRC)"
	@echo "Object files: $(OBJ)"
	@echo "Binary: $(BIN)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LDLIBS: $(LDLIBS)"
	@echo "Install prefix: $(PREFIX)"

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build the project (default)"
	@echo "  debug         - Build with debug flags"
	@echo "  release       - Build with release flags"
	@echo "  profile       - Build with profiling flags"
	@echo "  clean         - Remove build files"
	@echo "  distclean     - Remove build and distribution files"
	@echo "  install       - Install the binary"
	@echo "  deinstall     - Remove installed binary (FreeBSD style)"
	@echo "  dist          - Create distribution archive"
	@echo "  port-makefile - Generate FreeBSD port Makefile"
	@echo "  test          - Run tests"
	@echo "  analyze       - Run static analysis"
	@echo "  format        - Format source code"
	@echo "  memcheck      - Run memory check with valgrind"
	@echo "  coverage      - Generate coverage report"
	@echo "  run           - Build and run the program"
	@echo "  gdb           - Build debug version and start GDB"
	@echo "  lldb          - Build debug version and start LLDB"
	@echo "  sysinfo       - Show system information"
	@echo "  deps-check    - Check for required dependencies"
	@echo "  deps-install  - Install FreeBSD development dependencies"
	@echo "  info          - Show project information"
	@echo "  help          - Show this help message"

# Include dependency files
-include $(DEP)

# Phony targets
.PHONY: all debug release profile clean distclean install deinstall dist port-makefile test analyze format memcheck coverage run gdb lldb sysinfo deps-check deps-install info help