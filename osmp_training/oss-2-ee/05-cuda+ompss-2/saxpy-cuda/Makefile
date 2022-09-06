
include $(OSS2EE_COMMON_DIR)/Makefile

NVCC=nvcc
NVCCFLAGS=

CXX=mcxx

CXXFLAGS=$(OSS2EE_CFLAGS) --ompss-2 --cuda -O3 -Wall -Wno-unused
LDFLAGS=$(OSS2EE_LFLAGS)

ifeq ($(OSS2EE_NANOS6_DIRECTORY),1)
CXXFLAGS+=-DOSS2EE_NANOS6_DIRECTORY
endif

LIBS=$(OSS2EE_LIBS) -lrt
INCS=$(OSS2EE_INCS)

PREFIX=src
OBJ=kernel.o

BIN=multisaxpy_cuda

all: $(BIN)

% : $(PREFIX)/%.cpp $(PREFIX)/util.cpp $(PREFIX)/main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INCS) $^ -o $@ $(LIBS)

$(OBJ): $(PREFIX)/kernel.cu $(PREFIX)/kernel.h
	$(NVCC) $(NVCCFLAGS) $< -c -o $@ 

clean:
	@rm -f multisaxpy_cuda *.o *~
	@echo Cleaning current directory

