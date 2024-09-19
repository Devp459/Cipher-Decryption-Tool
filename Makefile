CXX = clang++
# Unused: warn, but annoying to block compilation on
# Sign compare: noisy
# Command line arg: noisy, not relevant to students
CXXFLAGS = \
	-Wall -Wextra -Werror \
	-Wno-error=unused-function \
	-Wno-error=unused-parameter \
	-Wno-error=unused-variable \
	-Wno-error=unused-but-set-variable \
	-Wno-error=unused-value \
	-Wno-sign-compare \
	-Wno-unused-command-line-argument \
	-std=c++2a -I. -g -fno-omit-frame-pointer \
	-fsanitize=address,undefined

# On Ubuntu and WSL, googletest is installed to /usr/include or
# /usr/local/include, which are used by default.

# On Mac, we need to manually include them in our path. Brew installs to
# different locations on Intel/Silicon, so ask brew where things live.
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	GTEST_PREFIX := $(shell brew --prefix googletest)
	LLVM_PREFIX := $(shell brew --prefix llvm)
	CXX := $(LLVM_PREFIX)/bin/clang++
	CXXFLAGS += -I$(GTEST_PREFIX)/include
	CXXFLAGS += -L$(GTEST_PREFIX)/lib
endif

# Test content won't change, so saving object files helps a lot with
# googletest macro processing time
build/%.o: tests/%.cpp utils.h tests/test_utils.h
	mkdir -p build && $(CXX) $(CXXFLAGS) -c $< -o $@

TEST_NAMES := $(basename $(notdir $(wildcard tests/*.cpp)))
TEST_OBJS := $(addprefix build/,$(addsuffix .o,$(TEST_NAMES)))

ciphers_tests: $(TEST_OBJS) ciphers.cpp
	$(CXX) $(CXXFLAGS) $^ -DCOMPILED_FOR_GTEST -lgtest -lgtest_main -o $@

test_caesar_enc: ciphers_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="CaesarEnc_*"

test_caesar_dec: ciphers_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="CaesarDec_*"

test_subst_enc: ciphers_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="SubstEnc_*"

test_subst_dec: ciphers_tests
	$(ENV_VARS) ./$< --gtest_color=yes --gtest_filter="SubstDec_*"

test_all: ciphers_tests
	$(ENV_VARS) ./$<

ciphers_main: ciphers.cpp utils.h
	$(CXX) $(CXXFLAGS) $< -o $@

run_ciphers: ciphers_main
	$(ENV_VARS) ./$<

clean:
	rm -f ciphers_tests ciphers_main build/*
	# MacOS symbol cleanup
	rm -rf *.dSYM

.PHONY: clean run_ciphers test_all test_ciphers_enc test_caesar_dec test_subst_enc test_subst_dec
