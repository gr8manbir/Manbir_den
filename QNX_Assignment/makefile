EXE := qnx_assign
CC := gcc

SRCDIR = src
OBJDIR := obj
BINDIR := bin

SRCS := $(wildcard $(SRCDIR)/*.c)
INC := $(wildcard $(SRCDIR)/*.h)
OBJS := $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

CFLAGS := -std=c99 -Wall -D _DEFAULT_SOURCE -Iinc/
LDFLAGS := -lm -lpthread -pthread

$(BINDIR)/$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	
$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $@

.PHONY: clean
	
clean:
	$(RM) $(OBJDIR)/* $(BINDIR)/*