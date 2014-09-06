NAME = mandelbrot

prefix=/usr/local/browndeer
exec_prefix=${prefix}

OPENCL_INCS = -I${exec_prefix}/include
OPENCL_LIBS = -L${exec_prefix}/lib -locl

STDCL_INCS = -I${prefix}/include
STDCL_LIBS = -L${exec_prefix}/lib -lstdcl

INCS = $(OPENCL_INCS) $(STDCL_INCS)
LIBS = $(OPENCL_LIBS) $(STDCL_LIBS) 

CCFLAGS += -O3

BUILD_OS = $(shell uname -s)

ifeq ($(BUILD_OS),Linux)
OBJS += ocl_kernels.o
endif

all: $(NAME).x 

$(NAME).x: $(NAME).o $(OBJS) 
	$(CC) $(CCFLAGS) $(INCS) -o $(NAME).x $(NAME).o $(OBJS) $(LIBS) 

ocl_kernels.o: kernel.cl
	clcc -c -o ocl_kernels.o mandel_kern.cl


.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CCFLAGS) $(INCS) -c $<

clean:
	rm -f *.o 

distclean: clean 
	rm -f *.x
