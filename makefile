#
# Main makefile for honmonogatari
#

CC          := gcc
CFLAGS      := `pkg-config --cflags gtk+-3.0`
CPP         := g++
CPPFLAGS    := -std=c++17 -g -Og -MMD -MP -Wall `pkg-config --cflags gtk+-3.0`
LD          := g++
LDFLAGS     := `pkg-config --libs gtk+-3.0` -export-dynamic

TARGET      := honmono

CEXT        := c
CPPEXT      := cpp
SRCDIR      := src
OUTDIR      := out
OUTEXT      := o
INCDIR      := inc
INCEXT      := d
BINDIR      := bin

CSOURCES    := $(shell find $(SRCDIR) -type f -name *.$(CEXT))
COBJECTS    := $(patsubst $(SRCDIR)/%,$(OUTDIR)/%,$(CSOURCES:.$(CEXT)=.$(OUTEXT)))

CPPSOURCES  := $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))
CPPOBJECTS  := $(patsubst $(SRCDIR)/%,$(OUTDIR)/%,$(CPPSOURCES:.$(CPPEXT)=.$(OUTEXT)))


all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(CPPOBJECTS) $(COBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

$(OUTDIR)/%.$(OUTEXT): $(SRCDIR)/%.$(CEXT)
	@dirname $@ | xargs mkdir -p
	$(CC) $(CFLAGS) -I ./$(SRCDIR) -I ./$(INCDIR) -c $< -o $@

$(OUTDIR)/%.$(OUTEXT): $(SRCDIR)/%.$(CPPEXT)
	@dirname $@ | xargs mkdir -p
	$(CPP) $(CPPFLAGS) -I ./$(SRCDIR) -I ./$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OUTDIR)/* $(BINDIR)/$(TARGET)

-include $(COBJECTS:%.$(OUTEXT)=%.$(INCEXT))
-include $(CPPOBJECTS:%.$(OUTEXT)=%.$(INCEXT))
