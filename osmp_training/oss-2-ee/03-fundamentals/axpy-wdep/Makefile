PROGRAM=axpy
include $(OSS2EE_COMMON_DIR)/Makefile

CC = mcxx

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -O3 -Wall -Wno-unused 
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) 
INCS=$(OSS2EE_INCS) 

$(PROGRAM): $(PROGRAM).cpp 
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $< $(LIBS)

clean:
	@rm -f $(PROGRAM) *.o $(CC)_* *~ 
	@echo Cleaning current directory 
