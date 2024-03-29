DEBUG_FLAGS=-g -fsanitize=address -DDEBUG
OPTIMIZE_FLAGS=-Os
CC=clang
CFLAGS=-std=c99
CXX=clang++
CXXFLAGS=-std=c++14
CPPFLAGS=-Wall -Werror -Wextra -pedantic -Iinclude -Ilibs -I$(GTEST) -I$(GTEST)/include

BUILD=build
INCLUDE=include
LIBS=libs
SRC=src
TEST=test

CESU8=$(LIBS)/cesu8
DUKTAPE=$(LIBS)/duktape-2.7.0
GTEST=$(LIBS)/googletest
JSI=$(LIBS)/jsi

ifeq ($(GCC), 1)
	CC=gcc
	CXX=g++
endif

ifeq ($(DEBUG), 1)
	CXXFLAGS+=$(DEBUG_FLAGS)
	CFLAGS+=$(DEBUG_FLAGS)
endif

ifeq ($(OPT), 1)
	CFLAGS+=$(OPTIMIZE_FLAGS)
	CXXFLAGS+=$(OPTIMIZE_FLAGS)
endif

all: $(BUILD)/test $(BUILD)/jsi-test

$(BUILD)/jsi-test.o: CPPFLAGS+=-Wno-unknown-warning-option -Wno-unused-parameter -Wno-sign-compare -Wno-catch-value

$(BUILD)/jsi-test: $(TEST)/jsi-test.cpp $(BUILD)/jsi-test.o $(BUILD)/gtest-all.o $(BUILD)/duktape-jsi.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $(BUILD)/jsi-test -lpthread

$(BUILD)/test: $(TEST)/test.cpp  $(BUILD)/gtest-all.o $(BUILD)/duktape-jsi.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $(BUILD)/test -lpthread

$(BUILD)/jsi-test.o: $(JSI)/test/testlib.h $(JSI)/test/testlib.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $(JSI)/test/testlib.cpp -o $(BUILD)/jsi-test.o

$(BUILD)/duktape-jsi.a: $(BUILD)/jsi.o $(BUILD)/duktape.o $(BUILD)/duktape-jsi.o
	$(AR) $(ARFLAGS) $@ $^

$(BUILD)/duktape-jsi.o: $(CESU8)/cesu8.h $(INCLUDE)/duktape-jsi.h $(SRC)/duktape-jsi.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BUILD)/duktape-jsi.o -c $(SRC)/duktape-jsi.cpp

$(BUILD)/jsi.o: $(JSI)/* | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BUILD)/jsi.o -c $(JSI)/jsi.cpp

$(BUILD)/duktape.o: $(DUKTAPE)/* | $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/duktape.o -c $(DUKTAPE)/src/duktape.c

$(BUILD)/gtest-all.o: $(GTEST)/src/* | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $(GTEST)/src/gtest-all.cc

$(BUILD):
	mkdir -p $(BUILD)

debug:
	make clean
	DEBUG=1 OPT=0 make all

opt:
	make clean
	DEBUG=0 OPT=1 make all

check: $(BUILD)/test $(BUILD)/jsi-test
	make debug && ./build/test && ./build/jsi-test

clean:
	rm -rf $(BUILD)



