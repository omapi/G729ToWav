#test pcm, wav and g729 transfer each other

INC_DIR=./include
LIBS=./lib64
OBJDIR = obj
BINDIR = bin

CC = gcc -D_STATIC_PRIM
CFLAGS = -c -Wall -g -o
LDFLAGS = -L$(LIBS) -lg729a -lm
INCLUDE = -I$(INC_DIR)

OBJS = 	$(OBJDIR)/main.o \
	$(OBJDIR)/transfer.o

EXECUTABLE = $(BINDIR)/tconv
 
all:libdir $(EXECUTABLE)

libdir:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

$(EXECUTABLE) : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o:	./src/%.c
	rm -f $@
	$(CC) $(CFLAGS) $@ $(INCLUDE) $<

