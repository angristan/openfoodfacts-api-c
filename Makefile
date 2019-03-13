CC = cc
SRC = get_off_product.c
BIN = get_off_product
LIBS = -l curl -l jansson
WFLAGS = -pedantic-errors -Wall -Wextra

.PHONY: clean

all: $(BIN)

%: %.c
	$(CC) $(LIBS) $(WFLAGS) -o $@ $<

clean: 
	rm -f $(BIN)
