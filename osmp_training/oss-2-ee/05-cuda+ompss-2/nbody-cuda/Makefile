include $(OSS2EE_COMMON_DIR)/Makefile

PREFIX=src

CC=mcc
CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -O3 -Wall -Wno-unused --cuda
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) -lstdc++
INCS=$(OSS2EE_INCS)

ifeq ($(OSS2EE_NANOS6_DIRECTORY),1)
CFLAGS+=-DOSS2EE_NANOS6_DIRECTORY
endif

NVCC=nvcc
NVCCFLAGS=

TARGETS=nbody_cuda

COMMON_SOURCES=$(PREFIX)/main.c $(PREFIX)/util.c
CUDA_SOURCES=$(PREFIX)/kernel.cu

all: $(TARGETS)

nbody_cuda: $(COMMON_SOURCES) kernel.o $(PREFIX)/nbody_cuda.c
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $^ $(LIBS)

kernel.o: $(PREFIX)/kernel.cu $(PREFIX)/kernel.h
	$(NVCC) $(NVCCFLAGS) -c -o $@ $<

clean:
	@rm -f nbody_cuda *.o *~
	@echo Cleaning current directory

