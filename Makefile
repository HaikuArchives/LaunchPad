topsrcdir = .
PACKAGE = LaunchPad
VERSION = 2.0d1
DISTNAME = $(PACKAGE)-$(VERSION)

CXX = g++
CFLAGS = -Wall -Wno-multichar -Wno-ctor-dtor-privacy
INCLUDES = -I$(topsrcdir)/config
CXXCOMPILE = $(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES)
LINK = $(CXX)

OBJS = src/Main.o src/LaunchPadApp.o src/LaunchPadWindow.o src/WindowDragger.o src/WindowTab.o src/TriStateButton.o src/LRowColumn.o src/PaneView.o src/DockItem.o src/EntryItem.o
RSRCS =

.SUFFIXES:	.cpp .c .rsrc

.cpp.o:
	$(CXXCOMPILE) -o $@ -c $<

all:	LaunchPad

clean:	dist-clean
	-rm -f src/*.o LaunchPad

dist-clean:
	-rm -rf $(DISTNAME) $(DISTNAME).zip

LaunchPad:	$(OBJS)
	$(LINK) -o $@ $(OBJS) -lbe -lroot
	xres -o $@ $(RSRCS)
	strip $@
	mimeset -f $@

dist:	all
	@echo 'building distribution'
	-rm -rf $(DISTNAME)
	mkdir $(DISTNAME)
	cp LaunchPad $(DISTNAME)
	cp README $(DISTNAME)
	cp LICENSE $(DISTNAME)
	mkdir $(DISTNAME)/doc
	mimeset -f $(DISTNAME)/*
	zip -r $(DISTNAME).zip $(DISTNAME)
	-rm -rf $(DISTNAME)
