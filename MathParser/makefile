VERSION = 3.02
CC      = g++
CFLAGS  = -Wall -g -std=c++1y
LDFLAGS = 

SRCDIR = src
OBJDIR = obj

SRCFILES := $(shell find $(SRCDIR) -name '*.cpp')
OBJFILES := $(subst $(SRCDIR),$(OBJDIR),$(SRCFILES:%.cpp=%.o))

compile: $(OBJFILES)
	$(CC) $(CFLAGS) -o MathParser $^

clean:
	-rm -r $(OBJDIR)/*

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.cpp)
	$(CC) -c $< -o $@ $(CFLAGS) 
