CC=clang
CFLAGS=-Wall -Werror -Wextra -pedantic -std=c17 -g -fsanitize=address
CXX=clang++
CXXFLAGS=-Wall -Werror -Wextra -pedantic -std=c++14 -g -fsanitize=address
CPPFLAGS=-Iinclude -Ilibs -I$(GTEST) -I$(GTEST)/include

BUILD=build
INCLUDE=include
LIBS=libs
SRC=src
TEST=test

CESU8=$(LIBS)/cesu8
DUKTAPE=$(LIBS)/duktape-2.7.0
GTEST=$(LIBS)/googletest
JSI=$(LIBS)/jsi

all: $(BUILD)/test $(BUILD)/jsi-test

$(BUILD)/jsi-test: $(TEST)/jsi-test.cpp $(BUILD)/jsi-test.o $(BUILD)/gtest-all.o $(BUILD)/duktape-jsi.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $(BUILD)/jsi-test -lpthread

$(BUILD)/test: $(TEST)/test.cpp  $(BUILD)/gtest-all.o $(BUILD)/duktape-jsi.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $(BUILD)/test -lpthread

# testlib.cpp has some warnings that aren't suppressed by -isystem, so not enabling warnings
$(BUILD)/jsi-test.o: $(JSI)/test/testlib.h $(JSI)/test/testlib.cpp | $(BUILD)
	$(CXX) -g -isystem . -isystem $(CPPFLAGS) -c $(JSI)/test/testlib.cpp -o $(BUILD)/jsi-test.o

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

clean:
	rm -rf $(BUILD)
