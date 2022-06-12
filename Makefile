CPP=clang++
CC=clang
CFLAGS=-Wall -Werror -pedantic
JSI_DIR=./jsi
DUKTAPE_DIR=./duktape-2.7.0
GTEST_DIR=./googletest
BUILDS=./builds

all: $(BUILDS)/test $(BUILDS)/jsi-test

$(BUILDS)/jsi-test: jsi-test.cpp $(BUILDS)/jsi-test.o $(BUILDS)/gtest-all.o $(BUILDS)/duktape-jsi.a
	$(CPP) $(CFLAGS) -Igoogletest/include -I. $^ -o $(BUILDS)/jsi-test -lpthread

$(BUILDS)/test: test.cpp  $(BUILDS)/gtest-all.o $(BUILDS)/duktape-jsi.a
	$(CPP) $(CFLAGS) -Igoogletest/include -I. $^ -o $(BUILDS)/test -lpthread

# testlib.cpp has some warnings that aren't suppressed by -isystem, so not enabling warnings
$(BUILDS)/jsi-test.o: jsi/test/testlib.h jsi/test/testlib.cpp | $(BUILDS)
	$(CPP) -isystem . -isystem $(GTEST_DIR)/include -c jsi/test/testlib.cpp -o $(BUILDS)/jsi-test.o

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
