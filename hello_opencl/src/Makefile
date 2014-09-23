NAME         = hello_opencl

OBJDIR       = ../obj
EXEDIR       = ../bin

SOURCES      = $(wildcard *.c)
CLSOURCES    = $(wildcard *.cl)
INCLUDES     = $(wildcard *.h)

CLKERNEL     = ocl_kernels.o

OBJECTS      = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
EXECUTABLE   = $(addprefix $(EXEDIR)/, $(NAME).x)
MKTARGETDIR  = @mkdir -p $(@D)

prefix=/usr/local/browndeer

INCS += -I${prefix}/include
LIBS += -L${prefix}/lib -lstdcl -locl

CCFLAGS += -O3

BUILD_OS = $(shell uname -s)

ifeq ($(BUILD_OS),Linux)
	CLOBJECTS += $(OBJDIR)/ocl_kernels.o
endif

###########################
#$(info $(SOURCES))
#$(info $(CLSOURCES))
#$(info $(OBJECTS))
#$(info $(OBJS))
#$(info $(EXECUTABLE))
################################
all: $(EXECUTABLE) 

$(EXECUTABLE): $(OBJDIR)/$(NAME).o $(CLOBJECTS) 
	$(MKTARGETDIR)
	$(CC) $(CCFLAGS) $(INCS) -o $(EXECUTABLE) $(OBJECTS) $(CLOBJECTS) $(LIBS) 

$(CLOBJECTS) : $(CLSOURCES)
	$(MKTARGETDIR)
	clcc -c -o $(CLOBJECTS) $(CLSOURCES)

$(OBJDIR)/%.o : %.c $(INCLUDES)
	$(MKTARGETDIR)
	$(CC) $(CCFLAGS) $(INCS) -c $<  -o $@

clean:
	rm -f *.o 
	rm -f $(CLOBJECTS) 
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
	rm -f $(EXEDIR)/core 

realclean: clean
	-rmdir $(OBJDIR)
	-rmdir $(EXEDIR)
	-rm -f *~
