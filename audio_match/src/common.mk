.PHONY: clean distclean

ifndef TOP_DIR
TOP_DIR := $(CURDIR)
endif
# current pwd
TOP_DIR := $(abspath $(TOP_DIR))

# src code directory
ADNA_SRC_DIR :=adna_syncserver

# all source file to build adna generate and push tool
ADNA_SRCS := afpWrap.c fft_utils.c genafp.c global.c hashtable.c utils.c

# object files according to ADNA_SRCS
ADNA_OBJS := $(ADNA_SRCS:.c=.o)
ADNA_OBJS := $(ADNA_OBJS:.cpp=.o)

# directory to placing object files
OBJS_DIR := ../objs

# add prefix derecting to real directory
ADNA_OBJS := $(addprefix $(ADNA_SRC_DIR)/, $(ADNA_OBJS)) # adna_syncserver/ADNA_OBJS
ADNA_OBJS := $(addprefix $(OBJS_DIR)/, $(ADNA_OBJS)) 	# ../objs/adna_syncserver/ADNA_OBJS
ADNA_SRCS := $(addprefix $(ADNA_SRC_DIR)/, $(ADNA_SRCS)) # adna_syncserver/ADNA_SRCS

CFLAGS=-Wall -g -I ./$(ADNA_SRC_DIR)
LDFLAGS=-lm
CC := gcc
CXX := g++

all: build_prep | $(ADNA_OBJS)
	#make flow_match
	#make flow_match_adna
	#make dynamic
	#make static

flow_match: $(ADNA_OBJS) flow_match.o match_utils.o
	$(CC) -o flow_match $(OBJS_DIR)/flow_match.o $(OBJS_DIR)/match_utils.o $(ADNA_OBJS)
flow_match_adna: $(ADNA_OBJS) flow_match_adna.o match_utils.o
	$(CC) -o flow_match_adna $(OBJS_DIR)/flow_match_adna.o $(OBJS_DIR)/match_utils.o $(ADNA_OBJS)
# build dynamic lib with -fPIC and -shared
dynamic: $(ADNA_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(ADNA_OBJS) -o libafp.so -fPIC -shared
# build static lib with ar rc
static: $(ADNA_OBJS)
	ar rc libafp.a $(ADNA_OBJS)
# build target with .so or .a
#flow_match: libafp.so flow_match.o match_utils.o
#	$(CC) -o flow_match $(OBJS_DIR)/flow_match.o $(OBJS_DIR)/match_utils.o -L. -lafp

# prepare the ADNA_SRC_DIR and OBJS_DIR
build_prep: $(TOP_DIR)/$(ADNA_SRC_DIR) $(OBJS_DIR)

# link the real src dir to current directory
$(TOP_DIR)/$(ADNA_SRC_DIR):
	ln -sf $(TOP_DIR)/../../adna_syncserver $(ADNA_SRC_DIR)
# prepare dir for build
$(OBJS_DIR):
	mkdir -p $@/$(ADNA_SRC_DIR)
# build the adna src to object files
$(OBJS_DIR)/$(ADNA_SRC_DIR)/%.o: $(ADNA_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_DIR)/$(ADNA_SRC_DIR)/%.o: $(ADNA_SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@


# build source file to object files in current directory
# first search object files in $(OBJS_DIR),
# because we place the built object file in $(OBJS_DIR)
# avoid always remade built object files only because not exist in current directory
vpath %.o $(OBJS_DIR)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJS_DIR)/$@
.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $(OBJS_DIR)/$@

clean:
	-$(RM) $(OBJS_DIR)/*.o
	-rm flow_match
	-rm flow_match_adna
distclean:
	-$(RM) -r $(OBJS_DIR)
	-rm $(ADNA_SRC_DIR) # rm links
help:
	@echo $(TOP_DIR)
	@echo "ADNA_OBJS = "$(ADNA_OBJS)
	@echo "ADNA_SRCS = "$(ADNA_SRCS)
	@echo "$(ADNA_OBJS)%.o"
	@echo "CFLAGS = "$(CFLAGS)
#include src/Makefile
