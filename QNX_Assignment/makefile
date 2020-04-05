EXE = qnx_assign
CC = gcc

SRCDIR = src
OBJDIR := obj
BINDIR := bin

SRCS := $(wildcard $(SRCDIR)/*.c)
INC := $(wildcard $(SRCDIR)/*.h)
OBJS := $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
 
CFLAGS := -std=c99 -Wall -Iinc/
LDFLAGS := -lm -lpthread -l

$(EXE): $(OBJS)
	@echo $(SRCS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	
$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(OBJDIR):
	mkdir $@

.PHONY: clean
	
clean:
	$(RM) $(OBJS) 