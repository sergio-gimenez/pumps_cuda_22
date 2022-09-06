include $(OSS2EE_COMMON_DIR)/Makefile_$(shell oss-block-number)

OSS2EE_MKL_LIBS= --Wl,-L$(OSS2EE_MKL_LIB_DIR) -lmkl_sequential -lmkl_core -lmkl_rt -lpthread
OSS2EE_MKL_INCS=-I$(OSS2EE_MKL_INC_DIR)

OSS2EE_TAMPI_LIBS= -L$(OSS2EE_TAMPI_LIB_DIR) -l:libtampi-c.a -lpthread
OSS2EE_TAMPI_INCS= -I$(OSS2EE_TAMPI_INC_DIR)

COMMON_FILES=extrae.xml trace.sh graph.sh run-once.sh multirun.sh
OUTPUT_FILES=*.err *.out *.dot *.pdf
TRACE_FILES=*.prv *.pcf *.row *.mpits set-*


all: $(PROGRAM) $(COMMON_FILES)

OSS2EE_CFLAGS=
OSS2EE_LFLAGS=

OSS2EE_LIBS= $(OSS2EE_LOCAL_DIR)/lib/ee_tools.o
OSS2EE_INCS= -I$(OSS2EE_LOCAL_DIR)/src -include ee_tools.h

OSS2EE_MKL_CHECK:$(OSS2EE_MKL_LIB_DIR)/libmkl_sequential.so $(OSS2EE_MKL_LIB_DIR)/libmkl_core.so $(OSS2EE_MKL_LIB_DIR)/libmkl_rt.so 

MPI_CHECK=$(OSS2EE_MPI_LIB_DIR)/libmpi.so
ATL_CHECK=$(OSS2EE_ATL_LIB_DIR)/libatlas.a
TAMPI_CHECK=$(OSS2EE_TAMPI_LIB_DIR)/libtampi.a


PROGRAM_DIR=$(shell oss-pdir)

extrae.xml:
	@cp $(OSS2EE_COMMON_DIR)/extrae-$(OSS2EE_JOB_SCHED_VERSION).xml extrae.xml
	@echo Creating extrae.xml file

trace.sh:
	@cp $(OSS2EE_COMMON_DIR)/trace-$(OSS2EE_JOB_SCHED_VERSION).sh trace.sh
	@echo Creating trace.sh helper script

graph.sh:
	@cp $(OSS2EE_COMMON_DIR)/graph.sh graph.sh
	@echo Creating graph.sh helper script

run-once.sh: $(OSS2EE_COMMON_DIR)/run-once.sh $(OSS2EE_APP_SETUP_DIR)/files/run-once.sh
	@cp  $(OSS2EE_COMMON_DIR)/run-once.sh .
	@cat $(OSS2EE_CONFIG_DIR)/job-sched-$(OSS2EE_JOB_SCHED_VERSION) >> run-once.sh
	@cat $(OSS2EE_APP_SETUP_DIR)/files/run-once.sh >> run-once.sh
	@echo Creating run-once.sh running script

multirun.sh: $(OSS2EE_COMMON_DIR)/multirun.sh $(OSS2EE_APP_SETUP_DIR)/files/multirun.sh
	@cp  $(OSS2EE_COMMON_DIR)/multirun.sh .
	@cat $(OSS2EE_CONFIG_DIR)/job-sched-$(OSS2EE_JOB_SCHED_VERSION) >> multirun.sh
	@cat $(OSS2EE_APP_SETUP_DIR)/files/multirun.sh >> multirun.sh
	@echo Creating multirun.sh running script


$(MPI_CHECK):
	@echo "=================================================="
	@echo "===           MPI LIBRARY NEEDED               ==="
	@echo "=================================================="
	@false

$(OSS2EE_MKL_LIB_DIR)/libmkl_core.so:
	@echo "=================================================="
	@echo "===  MKL LIBRARY NEEDED: mkl_core              ==="
	@echo "=================================================="
	@false

$(OSS2EE_MKL_LIB_DIR)/libmkl_sequential.so:
	@echo "=================================================="
	@echo "===  MKL LIBRARY NEEDED: mkl_sequential        ==="
	@echo "=================================================="
	@false

$(OSS2EE_MKL_LIB_DIR)/libmkl_rt.so:
	@echo "=================================================="
	@echo "===  MKL LIBRARY NEEDED: mkl_rt                ==="
	@echo "=================================================="
	@false

$(ATL_CHECK):
	@echo "=================================================="
	@echo "===          ATLAS LIBRARY NEEDED              ==="
	@echo "=================================================="
	@false

check: $(PROGRAM) 
	@echo "OmpSs-2 EE (make): Running check for $(PROGRAM) program at $(PROGRAM_DIR)"
	@[ -f $(OSS2EE_APP_SETUP_DIR)/files/run-check.sh ] \
      && $(OSS2EE_APP_SETUP_DIR)/files/run-check.sh \
      || echo "> check script does not exist; ignoring test for $(PROGRAM) program"

wipe: clean
	@rm -f $(OUTPUT_FILES) $(COMMON_FILES)
	@rm -rf $(TRACE_FILES)
	@echo Wiping current directory

reset: wipe
	@cp -fR $(OSS2EE_APP_SETUP_DIR)/reset/* . 2>/dev/null || :
	@echo Resetting current directory

