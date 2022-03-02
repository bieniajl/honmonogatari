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
$(BINDIR)/$(TARGET): $(LINKINGOBJECTS) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(LD) $(LDFLAGS) $(LINKINGOBJECTS) -o $@

# COMPILING TARGET
$(OUTDIR)/$(BLDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(CPPEXT) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(CPP) $(CPPFLAGS) $(COMPILERINCLUDES) -c $< -o $@

# IMGUI COMPILING TARGETS
$(OUTDIR)/$(LIBIMGUI)/%.$(OBJEXT): $(LIBDIR)/$(LIBIMGUI)/%.$(CPPEXT) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(CPP) $(IMGUIFLAGS) -iquote $(LIBDIR)/$(LIBIMGUI) -c $< -o $@
# IMGUI BACKEND COMPILING TARGETS
$(OUTDIR)/$(LIBIMGUI)/%.$(OBJEXT): $(LIBDIR)/$(LIBIMGUI)/$(BACKENDDIR)/%.$(CPPEXT) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(CPP) $(IMGUIFLAGS) -iquote $(LIBDIR)/$(LIBIMGUI) -c $< -o $@
# TEXTEDIT COMPILING TARGETS
$(OUTDIR)/$(LIBTEXTEDIT)/%.$(OBJEXT): $(LIBDIR)/$(LIBTEXTEDIT)/%.$(CPPEXT) $(BUILDCONFIGURATION)
	@dirname $@ | xargs mkdir -p
	$(CPP) $(IMGUIFLAGS) -iquote $(LIBDIR)/$(LIBIMGUI) -c $< -o $@


# -----------------------------------------------------------------------------

# DOCUMENTATION TARGET
.PHONY: doc
doc: $(DOCDIR)/$(DOXYFILE)
	VERSION_TRIM="$(VERSION_TRIM)" doxygen $(DOCDIR)/$(DOXYFILE)

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
	rm -rf $(BUILDCONFIGURATION) $(DOCDIR)/html $(DOCDIR)/latex $(DOCDIR)/man

# -----------------------------------------------------------------------------

# HEADER DEPENDENCY INCLUDES
-include $(OUTDIR)/$(BLDDIR)/$(MAINFILE:%.$(CPPEXT)=%.$(MAKEXT))
-include $(OBJECTS:%.$(OBJEXT)=%.$(MAKEXT))
-include $(IMGUIOBJECTS:%.$(OBJEXT)=%.$(MAKEXT))
-include $(BACKENDOBJECTS:%.$(OBJEXT)=%.$(MAKEXT))
-include $(TEXTEDITOBJECTS:%.$(OBJEXT)=%.$(MAKEXT))
