PRJ    = givens_qr
SRC    = seq_main.c matrix.c
INCL   = common.h matrix.h
OBJ    = $(SRC:%.c=%.o)

CC     = gcc
CFLAGS = -O1
LFLAGS = -lm -g

all: $(OBJ) $(INCL)
	$(CC)  $(CFLAGS) -o $(PRJ) $(SRC) $(LFLAGS)
	

# removes files created during build
clean:
	rm -f $(OBJ) $(PRJ)

