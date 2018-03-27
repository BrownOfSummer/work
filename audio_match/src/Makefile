.PHONY: clean
CFLAGS=-Wall -g -I../../adna_syncserver
#LIBS=-L../objs
OBJS :=genafp.o global.o utils.o hashtable.o fft_utils.o afpWrap.o
OBJS_DIR := ../objs/adna_syncserver
OBJS := $(addprefix $(OBJS_DIR)/, $(OBJS))
CC=gcc
CPP=g++
all:
	make seg_match
	make flow_match
	make flow_match_adna
	make mul_on_mul
	make dissimilar
	make similar
	#make test_skew
	#make test_gen_read
	#make test_cnt_forward
	make test_struct
	make print_adna_info
	make print_table_cnt

seg_match: seg_match.o match_utils.o $(OBJS)
	$(CC) -o seg_match seg_match.o match_utils.o $(LIBS) $(OBJS)
flow_match: flow_match.o match_utils.o $(OBJS)
	$(CC) -o flow_match flow_match.o match_utils.o $(LIBS) $(OBJS)
flow_match_adna: flow_match_adna.o match_utils.o $(OBJS)
	$(CC) -o flow_match_adna flow_match_adna.o match_utils.o$(LIBS) $(OBJS)
mul_on_mul: mul_on_mul.o $(OBJS)
	$(CC) -o mul_on_mul mul_on_mul.o $(LIBS) $(OBJS)
dissimilar: dissimilar.o match_utils.o $(OBJS)
	$(CC) -o dissimilar dissimilar.o match_utils.o $(OBJS)
similar: similar.o match_utils.o $(OBJS)
	$(CC) -o similar similar.o match_utils.o $(OBJS)
test_skew: test_skew.o $(OBJS)
	$(CC) -o test_skew test_skew.o $(LIBS) $(OBJS)
test_struct: test_struct.o $(OBJS)
	$(CC) -o test_struct test_struct.o $(LIBS) $(OBJS)
test_gen_read: test_gen_read.o $(OBJS)
	$(CC) -o test_gen_read test_gen_read.o $(LIBS) $(OBJS)
test_cnt_forward: test_cnt_forward.o match_utils.o $(OBJS)
	$(CC) -o test_cnt_forward test_cnt_forward.o match_utils.o $(LIBS) $(OBJS)
print_adna_info: print_adna_info.o match_utils.o $(OBJS)
	$(CC) -o print_adna_info print_adna_info.o match_utils.o $(LIBS) $(OBJS)
print_table_cnt: print_table_cnt.o match_utils.o $(OBJS)
	$(CC) -o print_table_cnt print_table_cnt.o match_utils.o $(LIBS) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $@
clean:
	@echo "Clean project..."
	-rm *.o
	-rm seg_match mul_on_mul 
	-rm test_gen_read test_skew test_cnt_forward test_struct 
	-rm dissimilar similar
	-rm print_table_cnt print_adna_info
	-rm flow_match flow_match_adna
	@echo "Done"

#include common.mk
