include $(OSS2EE_COMMON_DIR)/Makefile

PREFIX=src


CC=pgimcc
CXX=pgimcxx

KERNELS=kernels/openacc_kernels_unified.c
ifeq ($(OSS2EE_NANOS6_DIRECTORY),1)
KERNELS=kernels/openacc_kernels_directory.c
endif

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -O3 --openacc --cuda
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS)
INCS=-I$(PREFIX)

TARGETS=matmul_openacc

.PHONY: all

all: pre_build $(TARGETS)

pre_build:
	cp $(KERNELS) src/openacc_kernels.c


matmul_openacc:  $(PREFIX)/*.c  
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~ $(TARGETS)
	
