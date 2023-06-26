# #############################################################################
#
# This file is part of the "Luna HA-Bench" tool.
#
# The "Luna HA-Bench" tool is provided under the MIT license (see the
# following Web site for further details: https://mit-license.org/ ).
#
# Copyright © 2023 Thales Group
#
# #############################################################################

# ############################################################################
# Build parameters
# ############################################################################

ifndef TARGET
    $(error "TARGET is not defined.")
endif

PLATFORM?=OS_LINUX

LUNA_CLIENT_DIRECTORY?=/usr/safenet/lunaclient

INPUT_DIRECTORY?=./src
BUILD_DIRECTORY?=./build
OUTPUT_DIRECTORY?=./out

# Note: 
#   - Asserts are part of defensive and are preventing unexpected behaviors ... when
#     properly captured by appropriate exception handlers. Otherwise, the programs
#     terminates with a core dump. 
#   - Sanitization relies on Google's ASAN libraries. It requires the 
#     'libasan' and 'ibubsan' packages to be installed on the host that runs
#     the binaries produced by this Makefile. The following environment 
#     variables should be set before running these binaries:
#     - ASAN options (https://github.com/google/sanitizers/wiki/AddressSanitizerFlags#run-time-flags), e.g. 
# 			export ASAN_OPTIONS=\
# 				check_initialization_order=1: \
# 				detect_stack_use_after_return=1: \
# 				strict_init_order=1 \
# 				strict_string_checks=1: \
# 				verbosity=1
#     - UBSAN options (https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)), e.g. 
# 			export UBSAN_OPTIONS=\
# 				sanitize=alignment=1 \
# 				sanitize=bounds=1 \
# 				sanitize=nullability=1 \
# 				sanitize=pointer-overflow=1 \
# 				sanitize=shift=1 \
# 				sanitize=undefined=1 \
# 				sanitize=unreachable=1 \
# 				sanitize=vla-bound=1
#   	- Thread sanitizer options (https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
#
#   - Remind that most sanitization features have a significant performance
#     impact on both CPU and memory resources. 
WITH_ASSERTS=true
WITH_DEBUG=false
WITH_HARDENINGS=true
WITH_SANITIZATION=false


# ############################################################################
# Definitions for debugging
# ############################################################################
COMMON_DEBUG_DEFINES:=\
	-DDEBUG

COMMON_DEBUG_FLAGS:=\
	-ggdb \
	-g3 \
	-Og

# ############################################################################
# Common definitions for source code compilation (works at least with GCC)
# ############################################################################
COMMON_DEFINES+=\
	-D$(PLATFORM)

ifeq ($(WITH_ASSERTS), "false")
	COMMON_DEFINES+=\
		-DNDEBUG
endif

ifeq ($(WITH_DEBUG), "true")
	COMMON_DEFINES+=\
		$(COMMON_DEBUG_DEFINES)
endif

COMMON_WARNINGS+=\
	-Wall \
	-Wextra \
	\
	-Wcast-align \
	-Wconversion \
	-Wdisabled-optimization \
	-Wempty-body \
	-Wfatal-errors \
	-Wfloat-equal \
	-Wformat=2 \
	-Wformat-nonliteral \
	-Wformat-security \
	-Winline \
	-Wmissing-format-attribute \
	-Wno-format-zero-length \
	-Wno-unused-parameter \
	-Wpacked \
	-Wpointer-arith \
	-Wsign-compare \
	-Wtype-limits \
	-Wtrampolines \
	-Wuninitialized \
	-Wunused-parameter \
	-Wwrite-strings
# For recent versions of C++:
#	-Wdangling-else
	
COMMON_HARDENINGS+=\
	-fno-delete-null-pointer-checks \
	-fno-strict-aliasing \
	-fPIE -pie \
	-fPIC -shared \
	-fstack-protector-all \
	-Wl,-z,relro -Wl,-z,now \
	-Wl,-z,noexecstack
# For recent versions of C++:
#	-fharden-compares \
	-fharden-conditional-branches \
	-fcf-protection=full \
	-fstack-clash-protection \
	-fstackprotector-strong \
	-msecure-plt -Wl --secureplt

# Avoid setting D_FORTIFY_SOURCE when using ASAN (Google's AddressSanitizer).
ifneq ($(WITH_HARDENINGS), "true")
	COMMON_HARDENINGS+=\
		-D_FORTIFY_SOURCE=3
endif

COMMON_SANITIZE=\
	-fno-omit-frame-pointer \
	-fsanitize=address \
	-fstack-check 
# For recent versions of C++:
#	-fsanitize=alignment \
	-fsanitize=hwaddress \
	-fsanitize=leak \
	-fsanitize=thread \
	-fsanitize=pointer-compare \
	-fsanitize=pointer-subtract \
	-fsanitize=safe-stack \
	-fsanitize=shadow-call-stack \
	-fsanitize=undefined \
	-fsanitize-address-use-after-scope

ifeq ($(WITH_DEBUG), "true")
	COMMON_OTHERS+=\
		$(COMMON_DEBUG_FLAGS)
else
	COMMON_OTHERS+=\
		-O3
endif

# ############################################################################
# Definitions for C source code compilation using GCC
# ############################################################################
C_COMPILER?=gcc

# POSIX style is required to support some C language extensions.
C_STYLE?=\
	-std=gnu11 \
	-pedantic

C_DEFINES+=\
	$(COMMON_DEFINES)

C_WARNINGS+=\
	$(COMMON_WARNINGS) \
	-Wmissing-prototypes \
	-Wstrict-prototypes

C_HARDENINGS+=\
	$(COMMON_HARDENINGS)

C_SANITIZE+=\
	$(COMMON_SANITIZE)

C_OTHERS+=\
	$(COMMON_OTHERS)

C_FLAGS+=\
	$(C_STYLE) \
	$(C_WARNINGS) \
	$(C_DEFINES) \
	$(C_OTHERS)

ifeq ($(WITH_HARDENINGS), "true")
	C_FLAGS+=\
		$(C_HARDENINGS)
endif

ifeq ($(WITH_SANITIZATION), "true")
	C_FLAGS+=\
		$(C_SANITIZE)
endif

# ############################################################################
# Definitions for C++ source code compilation using GCC
# ############################################################################
CPP_COMPILER?=g++

CPP_STYLE?=\
	-std=c++11 \
	-pedantic

CPP_WARNINGS+=\
	$(COMMON_WARNINGS) \
	-Wctor-dtor-privacy \
	-Weffc++ \
	-Wmissing-declarations \
	-Wshadow \
	-Wsign-promo
# For recent versions of C++:
#	-Wold-style-cast

CPP_HARDENINGS+=\
	$(COMMON_HARDENINGS)

CPP_DEFINES+=\
	$(COMMON_DEFINES)

CPP_SANITIZE+=\
	$(COMMON_SANITIZE)

CPP_OTHERS+=\
	$(COMMON_OTHERS)

CPP_FLAGS+=\
	$(CPP_STYLE) \
	$(CPP_WARNINGS) \
	$(CPP_DEFINES) \
	$(CPP_OTHERS)

ifeq ($(WITH_HARDENINGS), "true")
	CPP_FLAGS+=\
		$(C_HARDENINGS)
endif

ifeq ($(WITH_SANITIZATION), "true")
	CPP_FLAGS+=\
		$(CPP_SANITIZE)
endif

# ############################################################################
# Definitions for dependencies
# ############################################################################
INCLUDES+=\
	-I $(LUNA_CLIENT_DIRECTORY)/samples/include \
	-I $(INPUT_DIRECTORY)

# ############################################################################
# Definitions for link edition using LINKER
# ############################################################################
LINKER?=g++

LD_FLAGS+=\
	-L $(LUNA_CLIENT_DIRECTORY)/lib

ifeq ($(WITH_SANITIZATION), "true")
	LD_FLAGS+=\
		$(COMMON_SANITIZE)
endif

LD_LIBS+=\
	-lpthread \
    -lCryptoki2_64

# ############################################################################
# Artefacts inventory
# ############################################################################
INPUT_SUB_DIRECTORIES=$(shell (cd $(INPUT_DIRECTORY) && find * -type d -not \( -path $(BUILD_DIRECTORY) -prune -o -path $(OUTPUT_DIRECTORY) -prune \)))
BUILD_SUB_DIRECTORIES=$(addprefix $(BUILD_DIRECTORY)/,$(INPUT_SUB_DIRECTORIES))

INPUT_FILES=$(shell (cd $(INPUT_DIRECTORY) && find * -type f))

HEADER_FILES=$(filter %.h,$(INPUT_FILES))
HEADER_FILES+=$(filter %.hpp,$(INPUT_FILES))

C_SOURCE_FILES=$(filter %.c,$(INPUT_FILES))
CPP_SOURCE_FILES=$(filter %.cpp,$(INPUT_FILES))
SOURCE_FILES=$(C_SOURCE_FILES) $(CPP_SOURCE_FILES)

OBJECT_FILES=$(C_SOURCE_FILES:%.c=$(BUILD_DIRECTORY)/%.o) $(CPP_SOURCE_FILES:%.cpp=$(BUILD_DIRECTORY)/%.o)

VPATH=$(INPUT_DIRECTORY)

# ############################################################################
# Build rules
# ############################################################################
.PHONY: all
all: info clean $(TARGET)
	@echo "Done."

.PHONY: clean
clean:
	@echo "- ----------------------------------------------------------------------------"
	@echo "- Clean the environment"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""
	rm -rf $(BUILD_DIRECTORY)
	rm -rf $(OUTPUT_DIRECTORY)
	@echo ""

.PHONY: info
info:
	@echo "- ----------------------------------------------------------------------------"
	@echo "- Configuration"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""

ifeq ($(WITH_ASSERTS), "true")
	@echo "   With asserts evaluation."
else
	@echo "   Without asserts evaluation."
endif

ifeq ($(WITH_DEBUG), "true")
	@echo "   With debug parameters."
else
	@echo "   Without debug parameters."
endif

ifeq ($(WITH_HARDENINGS), "true")
	@echo "   With hardening parameters."
else
	@echo "   Without hardening parameters."
endif

ifeq ($(WITH_SANITIZATION), "true")
	@echo "   With sanitization parameters."
else
	@echo "   Without sanitization parameters."
endif

	@echo ""
	@echo "   - PWD                   = $(shell pwd)"
	@echo ""
	@echo "   - TARGET                = $(TARGET)"
	@echo ""
	@echo "   - TARGET                = $(TARGET)"
	@echo "   - PLATFORM              = $(PLATFORM)"
	@echo ""
	@echo "   - LUNA_CLIENT_DIRECTORY = $(LUNA_CLIENT_DIRECTORY)"
	@echo ""
	@echo "   - INPUT_DIRECTORY       = $(INPUT_DIRECTORY)"
	@echo "   - BUILD_DIRECTORY       = $(BUILD_DIRECTORY)"
	@echo "   - OUTPUT_DIRECTORY      = $(OUTPUT_DIRECTORY)"
	@echo ""
	@echo "   - C_COMPILER            = $(C_COMPILER)"
	@echo "   - C_FLAGS               = $(C_FLAGS)"
	@echo ""
	@echo "   - CPP_COMPILER          = $(CPP_COMPILER)"
	@echo "   - CPP_FLAGS             = $(CPP_FLAGS)"
	@echo ""
	@echo "   - INCLUDES              = $(INCLUDES)"
	@echo ""
	@echo "   - LINKER                = $(LINKER)"
	@echo "   - LD_FLAGS              = $(LD_FLAGS)"
	@echo "   - LD_LIBS               = $(LD_LIBS)"
	@echo ""

$(BUILD_DIRECTORY)/%.o: %.c | $(BUILD_DIRECTORY) $(BUILD_SUB_DIRECTORIES)
	@echo "- ----------------------------------------------------------------------------"
	@echo "# Compile C code"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""
	$(C_COMPILER) $(C_FLAGS) $(INCLUDES) -c $< -o $@
	@echo ""

$(BUILD_DIRECTORY)/%.o: %.cpp | $(BUILD_DIRECTORY) $(BUILD_SUB_DIRECTORIES)
	@echo "- ----------------------------------------------------------------------------"
	@echo "# Compile C++ code"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""
	$(CPP_COMPILER) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@
	@echo ""

$(BUILD_SUB_DIRECTORIES): | $(BUILD_DIRECTORY)
	mkdir -p $@
	@echo ""
	
$(BUILD_DIRECTORY):
	@echo "- ----------------------------------------------------------------------------"
	@echo "# Create a build context"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""
	mkdir -p $@
	@echo ""

$(OUTPUT_DIRECTORY)/$(TARGET): $(OBJECT_FILES) | $(OUTPUT_DIRECTORY)
	@echo "- ----------------------------------------------------------------------------"
	@echo "# Link the object files"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""
	$(LINKER) $(LD_FLAGS) $(LD_LIBS) $(OBJECT_FILES) -o $@
	@echo ""

$(OUTPUT_DIRECTORY):
	@echo "- ----------------------------------------------------------------------------"
	@echo "# Create an output context"
	@echo "- ----------------------------------------------------------------------------"
	@echo ""
	mkdir -p $@
	@echo ""

$(TARGET): $(OUTPUT_DIRECTORY)/$(TARGET)
	@echo "Target built."