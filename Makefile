#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
SRC_DIR = src
CFLAGS = -g -Wall -Werror -std=c99 -m64
INCLUDES = -I$(SRC_DIR)
TRANS_OBJ = $(SRC_DIR)/trans.o

all: csim test-trans tracegen

csim: $(SRC_DIR)/csim.c $(SRC_DIR)/cachelab.c $(SRC_DIR)/cachelab.h
	$(CC) $(CFLAGS) $(INCLUDES) -o csim $(SRC_DIR)/csim.c $(SRC_DIR)/cachelab.c -lm 

test-trans: $(SRC_DIR)/test-trans.c $(TRANS_OBJ) $(SRC_DIR)/cachelab.c $(SRC_DIR)/cachelab.h
	$(CC) $(CFLAGS) $(INCLUDES) -o test-trans $(SRC_DIR)/test-trans.c $(SRC_DIR)/cachelab.c $(TRANS_OBJ) 

tracegen: $(SRC_DIR)/tracegen.c $(TRANS_OBJ) $(SRC_DIR)/cachelab.c
	$(CC) $(CFLAGS) $(INCLUDES) -O0 -o tracegen $(SRC_DIR)/tracegen.c $(TRANS_OBJ) $(SRC_DIR)/cachelab.c

$(TRANS_OBJ): $(SRC_DIR)/trans.c
	$(CC) $(CFLAGS) $(INCLUDES) -O0 -c $(SRC_DIR)/trans.c -o $(TRANS_OBJ)

#
# Clean the src dirctory
#
clean:
	rm -rf $(SRC_DIR)/*.o
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
