VERSION = 3.02
CC = gcc
OBJ = logger.o pagefile.o mmanage.o
OBJ2 =  vmaccess.o vmappl.o
  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
LDFLAGS = -lpthread
BIN_APPL = vmappl
BIN_MMAN = mmanage
VMEM_PAGESIZE = 8

default: all

all: vmappl mmanage 
vmappl:  $(OBJ2) 
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -o vmappl $(OBJ2) $(LDFLAGS)

mmanage: $(OBJ)
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -o mmanage $(OBJ) $(LDFLAGS)

logger.o: logger.c
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -c logger.c

pagefile.o: pagefile.c
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -c  pagefile.c

vmaccess.o: vmaccess.c
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -c vmaccess.c
	
vmappl.o: vmappl.c
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -c  vmappl.c
	
mmanage.o: mmanage.c 
	$(CC) $(CFLAGS) -D VMEM_PAGESIZE=$(VMEM_PAGESIZE) -c mmanage.c
clean:
	rm -rf $(BIN_MMAN) $(BIN_APPL) $(OBJ) $(OBJ2)
