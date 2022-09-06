PROGRAM=cholesky
include $(OSS2EE_COMMON_DIR)/Makefile

CC = mcc

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -std=c99 -O3 -Wall -Wno-unused -DVERBOSE
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) $(OSS2EE_OPENBLAS_LIBS) -lm
INCS=$(OSS2EE_INCS) $(OSS2EE_OPENBLAS_INCS) 

$(PROGRAM): $(PROGRAM).c $(PROGRAM).h $(OSS2EE_OPENBLAS_CHECK)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $< $(LIBS)

clean:
	@rm -f $(PROGRAM) *.o $(CC)_* *~ 
	@echo Cleaning current directory 
