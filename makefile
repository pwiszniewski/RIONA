CC=gcc
LDLIBS=-lm -fopenmp
LDFLAGS=
INCLUDES=-Iincludes -IC:\Users\PW\Downloads\lapack\include -IC:\OpenBLAS\include -I/opt/OpenBLAS/include
CFLAGS=-pedantic -Wall -std=gnu99 -O3 -fopenmp
CUDAOBJ=

SRCDIR=src
NAME = riona

ifneq (,$(findstring vect_debug,$(MAKECMDGOALS)))
    OBJDIR=obj/vect_debug
	EXENAME = $(NAME)_vect_debug.exe
else ifneq (,$(findstring vect,$(MAKECMDGOALS)))
    OBJDIR=obj/vect_release
	EXENAME = $(NAME)_vect.exe
else ifneq (,$(findstring cuda,$(MAKECMDGOALS)))
    OBJDIR=obj/cuda_release
	EXENAME = $(NAME)_cuda.exe
else ifneq (,$(findstring debug,$(MAKECMDGOALS)))
    OBJDIR=obj/debug
	EXENAME = $(NAME)_debug.exe
else
    OBJDIR=obj/release
	EXENAME = $(NAME).exe
endif

SRC_FILES := $(wildcard $(SRCDIR)/*.c)
OBJ_FILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC_FILES))

release: $(EXENAME)

debug: CFLAGS+=-g -DDEBUG
debug: $(EXENAME)


vect: CFLAGS+=-DVECT
vect: LDLIBS+=-lopenblas
vect: LDFLAGS+=-LC:\OpenBLAS\lib -L/opt/OpenBLAS/lib
vect: $(EXENAME)

vect_debug: CFLAGS+=-g -DDEBUG
vect_debug: vect

cuda: CFLAGS+=-g -DCUDA 
cuda: INCLUDES+=-I/usr/local/cuda/5.0.35/include/
cuda: $(OBJ_FILES)
cuda: cuda_compile
cuda: LDLIBS+=-lopenblas -lcuda -lcudart -lcublas -lstdc++
cuda: LDFLAGS+=-LC:\OpenBLAS\lib -L/opt/OpenBLAS/lib -L/usr/local_rwth/sw/cuda/5.0.35/lib64
cuda: CUDAOBJ+=$(OBJDIR)/my_cuda.o
cuda: 
	$(CC) -o $(EXENAME) $(OBJ_FILES) $(CUDAOBJ) $(LDLIBS) $(LDFLAGS)

cuda_compile:
	nvcc  $(INCLUDES) -c $(SRCDIR)/my_cuda.cu -o $(OBJDIR)/my_cuda.o 

cuda_link: 
	$(CC) -o $@ $^ $(OBJ_FILES) $(CUDAOBJ) $(LDLIBS) $(LDFLAGS)

$(EXENAME): $(OBJ_FILES)
	$(CC) -o $@ $^  $(LDFLAGS) $(LDLIBS) 

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR):
	mkdir -p $@

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJDIR)/*.o
