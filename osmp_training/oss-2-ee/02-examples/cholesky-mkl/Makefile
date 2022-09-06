PROGRAM=cholesky
include $(OSS2EE_COMMON_DIR)/Makefile

CC = mcc

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -std=c99 -O3 -Wall -Wno-unused 
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) $(OSS2EE_MKL_LIBS) -lm
INCS=$(OSS2EE_INCS) $(OSS2EE_MKL_INCS) 

$(PROGRAM): $(PROGRAM).c $(OSS2EE_MKL_CHECK)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $< $(LIBS)

clean:
	@rm -f $(PROGRAM) *.o $(CC)_* *~ 
	@echo Cleaning current directory 
