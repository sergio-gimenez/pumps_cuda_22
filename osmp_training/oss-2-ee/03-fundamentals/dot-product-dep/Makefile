PROGRAM=dot-product
include $(OSS2EE_COMMON_DIR)/Makefile

CC = mcc

CFLAGS=$(OSS2EE_CFLAGS) --ompss-2 -std=c99 -O3 -Wall -Wno-unused 
LFLAGS=$(OSS2EE_LFLAGS)

LIBS=$(OSS2EE_LIBS) 
INCS=$(OSS2EE_INCS) 

$(PROGRAM): $(PROGRAM).c
	$(CC) $(CFLAGS) $(LFLAGS) $(INCS) -o $@ $< $(LIBS)

clean:
	@rm -f $(PROGRAM) *.o $(CC)_* *~ 
	@echo Cleaning current directory 
