CC=g++
INCLUDE=$(shell pkg-config --cflags opencv)
LIBS=$(shell pkg-config --libs opencv)
BINS=gms_match main image_align_gms 
OBJS=
all:
	make gms_match
	make main
	make image_align_gms
gms_match: demo.o $(OBJS)
	$(CC) -O3 -o gms_match demo.o $(OBJS) $(LIBS)
main: main.o $(OBJS)
	$(CC) -O3 -o main main.o $(OBJS) $(LIBS)
image_align_gms: image_align_gms.o $(OBJS)
	$(CC) -O3 -o image_align_gms image_align_gms.o $(OBJS) $(LIBS)
.cpp.o:
	$(CC) $(INCLUDE) -c $< -o $@
.c.o:
	$(CC) $(INCLUDE) -c $< -o $@
clean:
	rm *.o
	rm $(BINS)
