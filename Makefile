CPP=clang++
CC=clang
CFLAGS=-Wall -Werror -pedantic
JSI_DIR=./jsi
DUKTAPE_DIR=./duktape-2.7.0
GTEST_DIR=./googletest
BUILDS=./builds

all: $(BUILDS)/main $(BUILDS)/test

$(BUILDS)/main: main.cpp $(BUILDS)/duktape-jsi.a
	$(CPP) $(CFLAGS) -o $(BUILDS)/main $^ -I.

$(BUILDS)/test: test.cpp $(BUILDS)/gtest-all.o $(BUILDS)/duktape-jsi.a
	$(CPP) $(CFLAGS) -Igoogletest/include -I. $^ -o $(BUILDS)/test -lpthread

$(BUILDS)/duktape-jsi.a: $(BUILDS)/jsi.o $(BUILDS)/duktape.o $(BUILDS)/duktape-jsi.o
	$(AR) $(ARFLAGS) $@ $^

$(BUILDS)/duktape-jsi.o: duktape-jsi.h duktape-jsi.cpp | $(BUILDS)
	$(CPP) $(CFLAGS) -o $(BUILDS)/duktape-jsi.o -c duktape-jsi.cpp -I.

$(BUILDS)/jsi.o: $(JSI_DIR)/* | $(BUILDS)
	$(CPP) $(CFLAGS) -o $(BUILDS)/jsi.o -c $(JSI_DIR)/jsi.cpp -I.

$(BUILDS)/duktape.o: $(DUKTAPE_DIR)/* | $(BUILDS)
	$(CC) $(CFLAGS) -o $(BUILDS)/duktape.o -c $(DUKTAPE_DIR)/src/duktape.c

$(BUILDS)/gtest-all.o: $(GTEST_DIR)/src/* | $(BUILDS)
	$(CPP) $(CFLAGS) -o $@ -c $(GTEST_DIR)/src/gtest-all.cc -I$(GTEST_DIR)/include -I$(GTEST_DIR)

$(BUILDS):
	mkdir -p $(BUILDS)

clean:
	rm -rf builds
