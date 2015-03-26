# $Id: Makefile.dist 186 2000-06-28 14:15:35Z schani $

CC=gcc
CFLAGS=-Wall -g -I.

LISPREADER_OBJS = lispreader.o allocator.o pools.o

all : docexample

docexample : docexample.o $(LISPREADER_OBJS)
	$(CC) -Wall -g -o docexample $(LISPREADER_OBJS) docexample.o

#comment-test: comment-test.o $(LISPREADER_OBJS)
#	$(CC) -Wall -g -o comment-test $(LISPREADER_OBJS) comment-test.o

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f docexample *.o *~
