include $(OSS2EE_COMMON_DIR)/Makefile

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 --openacc --cuda -O3
LDFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS)
INCS=$(OSS2EE_INCS)

CC=pgimcc
CXX=pgimcxx

ifeq ($(OSS2EE_NANOS6_DIRECTORY),1)
KERNELS=kernels/openacc_kernels_directory.c
CFLAGS+=-DOSS2EE_NANOS6_DIRECTORY
endif



PREFIX=src
BIN=multisaxpy_openacc

.PHONY: all
 
all: $(BIN)

% : $(PREFIX)/%.c $(PREFIX)/main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCS) $^ -o $@ $(LIBS)

clean:
	@rm -f multisaxpy_openacc *.o *~
	@echo Cleaning current directory

