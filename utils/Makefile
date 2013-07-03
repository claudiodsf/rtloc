PROGNAME=stat2ellipse
PREFIX=.

CC=gcc
CFLAGS=-g
LDFLAGS=-lm
INCLUDE=-I../src

SRC1=stat2ellipse.c GridGraphLib.c vector.c
SRC2=../src/GridLib.c ../src/util.c ../src/map_project.c ../src/geo.c ../src/octtree.c ../src/ran1.c ../src/nrutil.c ../src/nrmatrix.c
OBJS1=$(patsubst %.c,%.o,$(SRC1))
OBJS2=$(patsubst %.c,%.o,$(SRC2))

%.o:%.c
	$(CC) -o $@ $(CFLAGS) $(INCLUDE) -c $<

$(PROGNAME): $(OBJS1) $(OBJS2)
	$(CC) -o $@ $(CFLAGS) $(LIB) $(OBJS1) $(OBJS2) $(LDFLAGS)

clean:
	(rm -f $(OBJS1))

install: $(PROGNAME)
	cp $(PROGNAME) $(PREFIX)
