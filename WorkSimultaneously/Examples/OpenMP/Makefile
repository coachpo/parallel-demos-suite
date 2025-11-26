#
# Makefile for OpenMP demo programs
#
# "make program" to make one program
# "make" or "make all" to make all executables
# "make clean" to remove executables
#

# gcc compiler
OMPC        = gcc
CFLAGS      = -O2 -fopenmp

OMPCC       = /usr/bin/g++
CCFLAGS     = -O2 -fopenmp

LFLAGS      = -lm

ALL =   omp_hello \
	omp_hello1 \
	omp_hello2 \
	omp_num_threads \
	omp_reduction  \
	omp_sections  \
	omp_critical \
	omp_for \
	omp_get_env_info \
	omp_threadprivate  \
	omp_matrixmult \
	omp_own_workshare \
	omp_pi 

.PHONY:  all
all:  $(ALL)

%: %.c 
	$(OMPC) -o $@ $(CFLAGS) $< $(LFLAGS)

.PHONY:  clean
clean:
	-rm $(ALL)
