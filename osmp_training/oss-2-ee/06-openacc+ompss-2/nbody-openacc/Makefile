include $(OSS2EE_COMMON_DIR)/Makefile

PREFIX=src

CC=pgimcc
CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -O3 --openacc
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS)
INCS=$(OSS2EE_INCS)

TARGETS=nbody_openacc

COMMON_SOURCES=$(PREFIX)/main.c $(PREFIX)/util.c

all: $(TARGETS)

nbody_openacc: $(COMMON_SOURCES) $(PREFIX)/nbody_openacc.c
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $^ $(LIBS)

clean:
	@rm -f nbody_openacc *.o $(CC)_* *~
	@echo Cleaning current directory

