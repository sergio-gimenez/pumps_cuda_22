include $(OSS2EE_COMMON_DIR)/Makefile

PREFIX=src


CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -O3 -Wall -Wno-unused --cuda
LFLAGS=$(OSS2EE_LFLAGS)

CC=mcc

ifeq ($(OSS2EE_NANOS6_DIRECTORY),1)
CFLAGS+=-DOSS2EE_NANOS6_DIRECTORY
endif

LIBS=$(OSS2EE_LIBS) -lstdc++
INCS=-I$(PREFIX)

TARGETS=matmul-p

all: $(TARGETS)

matmul-p:  $(PREFIX)/*.c $(PREFIX)/*.cu
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~ $(TARGETS)
	
