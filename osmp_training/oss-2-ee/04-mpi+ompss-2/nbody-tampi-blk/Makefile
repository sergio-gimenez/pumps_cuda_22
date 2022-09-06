PROGRAM=nbody
include $(OSS2EE_COMMON_DIR)/Makefile

CC = I_MPI_CC=mcc MPICH_CC=mcc OMPI_CC=mcc $(OSS2EE_MPICC_COMMAND)

COMMON_SRC=main.c misc.c

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -std=gnu11 -O3 -Wall -Wno-unused
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) $(OSS2EE_TAMPI_LIBS) -lstdc++ -lm
INCS=$(OSS2EE_INCS) $(OSS2EE_TAMPI_INCS)

$(PROGRAM): $(PROGRAM).c $(COMMON_SRC) $(OSS2EE_TAMPI_CHECK)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $(PROGRAM).c $(COMMON_SRC) $(LIBS)

clean:
	@rm -f $(PROGRAM) *.o *~
	@echo Cleaning current directory 
