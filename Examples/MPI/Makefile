# Makefile for MPI example programs

##### User configurable options

CC          = mpicc
CLINKER     = $(CC)
OPTFLAGS    = -O2 -std=gnu11    ####-std=c99
LFLAGS	    = -lm

### End User configurable options

CFLAGS  =   $(OPTFLAGS)
EXECS = hello send-standard send-standard-large send-synchronous send-buffered \
	send-nonblocking-wait send-nonblocking-waitall send-nonblocking-waitany \
	probe scatter gather random_sum scatterv datatype rowcol sendcol cpi_mc \
	test_dims_create

default: $(EXECS)

hello: hello.o 
	$(CLINKER) $(OPTFLAGS)  -o hello hello.o

send-standard: send-standard.o 
	$(CLINKER) $(OPTFLAGS)  -o send-standard send-standard.o

send-standard-large: send-standard-large.o 
	$(CLINKER) $(OPTFLAGS)  -o send-standard-large send-standard-large.o

send-synchronous: send-synchronous.o 
	$(CLINKER) $(OPTFLAGS)  -o send-synchronous send-synchronous.o

send-buffered: send-buffered.o 
	$(CLINKER) $(OPTFLAGS)  -o send-buffered send-buffered.o

send-nonblocking-wait: send-nonblocking-wait.o 
	$(CLINKER) $(OPTFLAGS)  -o send-nonblocking-wait send-nonblocking-wait.o

send-nonblocking-waitall: send-nonblocking-waitall.o 
	$(CLINKER) $(OPTFLAGS)  -o send-nonblocking-waitall send-nonblocking-waitall.o

send-nonblocking-waitany: send-nonblocking-waitany.o 
	$(CLINKER) $(OPTFLAGS)  -o send-nonblocking-waitany send-nonblocking-waitany.o

scatter: scatter.o 
	$(CLINKER) $(OPTFLAGS)  -o scatter scatter.o

gather: gather.o 
	$(CLINKER) $(OPTFLAGS)  -o gather gather.o

probe: probe.o 
	$(CLINKER) $(OPTFLAGS)  -o probe probe.o

random_sum: random_sum.o 
	$(CLINKER) $(OPTFLAGS)  -o random_sum random_sum.o

scatterv: scatterv.o 
	$(CLINKER) $(OPTFLAGS)  -o scatterv scatterv.o

datatype: datatype.o 
	$(CLINKER) $(OPTFLAGS)  -o datatype datatype.o

rowcol: rowcol.o 
	$(CLINKER) $(OPTFLAGS) $(LFLAGS) -o rowcol rowcol.o

sendcol: sendcol.o 
	$(CLINKER) $(OPTFLAGS)  -o sendcol sendcol.o

test_dims_create: test_dims_create.o 
	$(CLINKER) $(OPTFLAGS)  -o test_dims_create test_dims_create.o

cpi_mc: cpi_mc.o 
	$(CLINKER) $(OPTFLAGS) $(LFLAGS) -o cpi_mc cpi_mc.o

clean:
	/bin/rm -f *.o *~ PI* $(EXECS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c
.f.o:
	$(F77) $(FFLAGS) -c $*.f
