CPP=clang++
CC=clang
CFLAGS=-Wall -Werror -pedantic
JSI_DIR=./jsi
DUKTAPE_DIR=./duktape-2.7.0
BUILDS=./builds

$(BUILDS)/main: test.cpp $(BUILDS)/jsi.o $(BUILDS)/duktape.o
	$(CPP) $(CFLAGS) -o $(BUILDS)/main test.cpp $(BUILDS)/jsi.o $(BUILDS)/duktape.o -I.

$(BUILDS)/jsi.o: $(JSI_DIR)/* | $(BUILDS)
	$(CPP) $(CFLAGS) -o $(BUILDS)/jsi.o -c $(JSI_DIR)/jsi.cpp -I.

$(BUILDS)/duktape.o: $(DUKTAPE_DIR)/* | $(BUILDS)
	$(CC) $(CFLAGS) -o $(BUILDS)/duktape.o -c $(DUKTAPE_DIR)/src/duktape.c

$(BUILDS):
	mkdir -p $(BUILDS)

clean:
	rm -rf builds
