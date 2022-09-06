PROGRAM=heat
include $(OSS2EE_COMMON_DIR)/Makefile

CC = I_MPI_CXX=mcxx MPICH_CXX=mcxx OMPI_CXX=mcxx $(OSS2EE_MPICXX_COMMAND)

COMMON_SRC=main.cpp misc.cpp

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -std=c++11 -O3 -Wall -Wno-unused 
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) $(OSS2EE_TAMPI_LIBS) -lm
INCS=$(OSS2EE_INCS) $(OSS2EE_TAMPI_INCS)

$(PROGRAM): $(PROGRAM).cpp $(COMMON_SRC) $(OSS2EE_TAMPI_CHECK)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $(PROGRAM).cpp $(COMMON_SRC) $(LIBS)

clean:
	@rm -f $(PROGRAM) *.o *~
	@echo Cleaning current directory 
