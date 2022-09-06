PROGRAM=matmul
KERNEL=kernel
PREFIX=.

TARGETS=$(PROGRAM)-p $(PROGRAM)-i $(PROGRAM)-d

JOB_SCHED_VERSION=-smp

CC = mcc
KC = nvcc

CFLAGS =  -O3 --ompss-2 --cuda --std=c99 -lstdc++
CFLAGS_P =
CFLAGS_I = --instrument
CFLAGS_D = --debug

NVCFLAGS = -O3

LIBS  =
INCS  = -I$(PREFIX)

PRECISION=-DDP
EXTRA = -O3 -Wall -Wno-unused
OBJECTS= check.o gendat.o prtspeed.o cclock.o

$(PROGRAM)-p: $(PROGRAM)-p.o $(KERNEL).o $(OBJECTS)
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_P) $(EXTRA) -o $@ $< $(LIBS) $(KERNEL).o $(OBJECTS)

$(PROGRAM)-i: $(PROGRAM)-i.o $(KERNEL).o $(OBJECTS)
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_I) $(EXTRA) -o $@ $< $(LIBS) $(KERNEL).o $(OBJECTS)

$(PROGRAM)-d: $(PROGRAM)-d.o $(KERNEL).o $(OBJECTS)
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_D) $(EXTRA) -o $@ $< $(LIBS) $(KERNEL).o $(OBJECTS)


$(PROGRAM)-p.o: $(PROGRAM).c $(PREFIX)/$(KERNEL).h
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_P) $(EXTRA) $(PRECISION) $(INCS) -o $@ -c $<

$(PROGRAM)-i.o: $(PROGRAM).c $(PREFIX)/$(KERNEL).h
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_I) $(EXTRA) $(PRECISION) $(INCS) -o $@ -c $<

$(PROGRAM)-d.o: $(PROGRAM).c $(PREFIX)/$(KERNEL).h
	$(CC) $(GFLAGS) $(CFLAGS) $(CFLAGS_D) $(EXTRA) $(PRECISION) $(INCS) -o $@ -c $<


$(KERNEL).o: $(KERNEL).cu $(PREFIX)/$(KERNEL).h
	$(KC) $(INCS) $(PRECISION) -o $@ -c $<

.c.o:
	$(CC) --no-openmp --ompss-2 $(EXTRA) $(PRECISION) -c $<

clean:
	rm -f $(CC)_* *.o *~ $(TARGETS)
	

