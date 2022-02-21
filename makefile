#
# Mee's C/C++ Makefile Supreme
#

# -----------------------------------------------------------------------------

include version
include .config/constants
-include $(BUILDCONFIGURATION)
include .config/functions

# -----------------------------------------------------------------------------

# special make variables
.DEFAULT_GOAL  := default
.EXTRA_PREREQS := makefile

# -----------------------------------------------------------------------------

# SPECIAL TARGETS
.PHONY: all
all: code doc

.PHONY: default
default: $(BINDIR)/$(TARGET)

# -----------------------------------------------------------------------------

# CONFIG TARGET
.PHONY: config
.NOTPARALLEL: config
config:
	@.config/select_config.sh

# VERSION TARGET
.PHONY: printversion
printversion:
	@echo $(VERSION_TRIM)

# -----------------------------------------------------------------------------

# LINKING TARGET
$(BINDIR)/$(TARGET): $(OBJECTS) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# COMPILING TARGET
$(OUTDIR)/$(BLDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(CPPEXT) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(CPP) $(CPPFLAGS) -iquote ./$(SRCDIR) -I ./$(LIBDIR) -c $< -o $@

# -----------------------------------------------------------------------------

# DOCUMENTATION TARGET
.PHONY: doc
doc: $(DOCDIR)/$(DOXYFILE)
	doxygen $(DOCDIR)/$(DOXYFILE)

# -----------------------------------------------------------------------------

# CLEANUP TARGET
.PHONY: clean
.NOTPARALLEL: clean
clean:
	rm -rf $(OUTDIR) $(BINDIR) $(DOCDIR)/doxygen.log

# RESET TARGET
.PHONY: reset
.NOTPARALLEL: reset
reset: clean
	rm -rf $(BUILDCONFIGURATION) $(DOCDIR)/html $(DOCDIR)/latex

# -----------------------------------------------------------------------------

# HEADER DEPENDENCY INCLUDES
-include $(OUTDIR)/$(BLDDIR)/$(MAINFILE:%.$(CPPEXT)=%.$(MAKEXT))
-include $(OBJECTS:%.$(OBJEXT)=%.$(MAKEXT))
