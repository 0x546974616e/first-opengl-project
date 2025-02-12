# :set noexpandtab

# ╔═╗┬ ┬┌┐┌┌─┐┌┬┐┬┌─┐┌┐┌┌─┐
# ╠╣ │ │││││   │ ││ ││││└─┐
# ╚  └─┘┘└┘└─┘ ┴ ┴└─┘┘└┘└─┘

# Recursive wildcard.
RWILDCARD = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call RWILDCARD,$d/,$2))

# ╔╦╗┌─┐┬─┐┌─┐┌─┐┌┬┐┌─┐
#  ║ ├─┤├┬┘│ ┬├┤  │ └─┐
#  ╩ ┴ ┴┴└─└─┘└─┘ ┴ └─┘

ROOT_DIR = $(CURDIR)
BUILD_DIR = $(ROOT_DIR)/build
RESOURCES_DIR = $(ROOT_DIR)/resources
SOURCES_DIR = $(ROOT_DIR)/sources
VENDOR_DIR = $(ROOT_DIR)/vendor

BINARY = $(BUILD_DIR)/main

CCC_SUFFIX = c
CXX_SUFFIX = cpp

CCC_SOURCES = $(call RWILDCARD,$(SOURCES_DIR)/,*.$(CCC_SUFFIX)) $(call RWILDCARD,$(VENDOR_DIR)/,*.$(CCC_SUFFIX))
CXX_SOURCES = $(call RWILDCARD,$(SOURCES_DIR)/,*.$(CXX_SUFFIX)) $(call RWILDCARD,$(VENDOR_DIR)/,*.$(CXX_SUFFIX))

CCC_OBJECTS = $(CCC_SOURCES:$(ROOT_DIR)/%.$(CCC_SUFFIX)=$(BUILD_DIR)/%.o)
CXX_OBJECTS = $(CXX_SOURCES:$(ROOT_DIR)/%.$(CXX_SUFFIX)=$(BUILD_DIR)/%.o)

CCC_DEPENDENCIES = $(CCC_OBJECTS:.o=.d)
CXX_DEPENDENCIES = $(CXX_OBJECTS:.o=.d)

# ╔═╗┬  ┌─┐┌─┐┌─┐
# ╠╣ │  ├─┤│ ┬└─┐
# ╚  ┴─┘┴ ┴└─┘└─┘

CCC = clang-19
CXX = clang++-19

# TODO: See OpenSSF, -pedantic
MACRO_EXPORT = ROOT_DIR RESOURCES_DIR
COMMON_FLAGS = -Wall -Wextra -Wconversion -Werror -O0 \
	-Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field \
	$(foreach macro,$(MACRO_EXPORT), -D TR_$(macro)='"$($(macro))"')

CCC_FLAGS = $(COMMON_FLAGS) -std=c23
CXX_FLAGS = $(COMMON_FLAGS) -std=c++23

CCC_INCLUDE = -iquote $(SOURCES_DIR) -I $(VENDOR_DIR)
CXX_INCLUDE = -iquote $(SOURCES_DIR) -I $(VENDOR_DIR)

CCC_PREPROCESSOR = -MMD -MP -MT $@ -MF $(@:.o=.d)
CXX_PREPROCESSOR = -MMD -MP -MT $@ -MF $(@:.o=.d)

LD_FLAGS = -z noexecstack $(shell pkg-config --static --libs glfw3)

# ╔╗ ┬ ┬┬ ┬  ┌┬┐
# ╠╩╗│ ││ │   ││
# ╚═╝└─┘┴ ┴─┘╶┴┘

.PHONY: all

all: $(BINARY)
	@echo "-->" ./$(<:$(CURDIR)/%=%)

$(BINARY): $(CCC_OBJECTS) $(CXX_OBJECTS)
	@echo Generating Code...
	@$(CXX) $^ -o $@ $(LD_FLAGS)

$(CCC_OBJECTS): $(BUILD_DIR)/%.o: $(ROOT_DIR)/%.$(CCC_SUFFIX)
	@mkdir -p $(dir $@)
	@echo $(<:$(ROOT_DIR)/%=%)
	@$(CCC) -c $< -o $@ $(CCC_FLAGS) $(CCC_INCLUDE) $(CCC_PREPROCESSOR)

$(CXX_OBJECTS): $(BUILD_DIR)/%.o: $(ROOT_DIR)/%.$(CXX_SUFFIX)
	@mkdir -p $(dir $@)
	@echo $(<:$(ROOT_DIR)/%=%)
	@$(CXX) -c $< -o $@ $(CXX_FLAGS) $(CXX_INCLUDE) $(CXX_PREPROCESSOR)

-include $(CCC_DEPENDENCIES)
-include $(CXX_DEPENDENCIES)

# ╔═╗┬  ┌─┐┌─┐┌┐┌
# ║  │  ├┤ ├─┤│││
# ╚═╝┴─┘└─┘┴ ┴┘└┘

.PHONY: clean cleanall mrproper

clean:
	@rm -f $(CCC_OBJECTS) $(CXX_OBJECTS)

cleanall: clean
	@rm -f $(CCC_DEPENDENCIES) $(CXX_DEPENDENCIES)

mrproper : cleanall
	@rm -f $(BINARY)

# ╦═╗┬ ┬┌┐┌
# ╠╦╝│ ││││
# ╩╚═└─┘┘└┘

.PHONY: crun run

crun: all run

run:
	@$(BINARY)
