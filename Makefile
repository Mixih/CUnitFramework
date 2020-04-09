# sample Makefile
CC             := gcc
CFLAGS         := -std=c99 -pedantic -Wall -Wextra 
LDLIBS         := -lm

BUILDIR        := build
CUFDIR         := cuf

# object deps for the executables. Specify without the `.o`.
# CUF testing library objects
CUFOBJS        := cuf cuf_dep cuf_util
TESTDEPS       := $(CUFOBJS) test_main

# Specify "project" as the default target
.DEFAULT_GOAL  := all


all: testrunner

test: testrunner
	./testrunner

# Executable linking targets
testrunner: $(addprefix $(BUILDIR)/,$(addsuffix .o,$(TESTDEPS)))
	$(CC) -o $@ $^ $(LDLIBS)

# Build rules for all object files 
$(BUILDIR)/%.o: $(CUFDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

# build rules to autogen dependecy makefiles using technique described in the
# GNU make docs. Appearently GCC itself can do this now, but the docs are still
# sparse
$(BUILDIR)/%.mk: $(CUFDIR)/%.c
	$(call autogen_deps,$@,$<,)

clean:
	rm -rf build/ testrunner

# Register all, clean, and test as fake targets so they still get run even if a
# file of the same name exists in the tree
.PHONY: all clean test


# make the build directory if not present
$(shell   mkdir -p $(BUILDIR))

# Reusable make function to autogen dependencies
# call with $(call [arg1],[arg2],[arg3])
# First arg is output dependency file, second arg is the source file with the
# dep, and third arg is special flags to pass to gcc for compilation
define autogen_deps
	@set -e; rm -f $1; \
	$(CC) $(CFLAGS) $3 -M $2 > $1; \
	sed -ri "s|(.*)\.o|$(dir $1)\1.o $1|" $1
endef

# include generated dependency files so objects are remade correctly
include $(patsubst $(CUFDIR)/%.c,$(BUILDIR)/%.mk,$(wildcard $(CUFDIR)/*.c))
