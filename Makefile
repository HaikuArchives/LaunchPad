topsrcdir = .
PACKAGE = LaunchPad
VERSION = 2.0d1
DISTNAME = $(PACKAGE)-$(VERSION)

CXX = g++
CFLAGS = -Wall -Wno-multichar -Wno-ctor-dtor-privacy
INCLUDES = -I$(topsrcdir)/config
CXXCOMPILE = $(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES)

OBJS = src/Main.o src/LaunchPadApp.o src/LaunchPadWindow.o src/WindowDragger.o src/WindowTab.o src/TriStateButton.o

.SUFFIXES:	.cpp .c .rsrc

.cpp.o:
	$(CXXCOMPILE) -o $@ -c $<

all:	LaunchPad

clean:	dist-clean
	-rm -f src/*.o LaunchPad

dist-clean:
	-rm -rf $(DISTNAME) $(DISTNAME).zip

LaunchPad:	$(OBJS)
	$(CXX) -o LaunchPad $(OBJS) -lbe -lroot

dist:	all
	@echo 'building distribution'
	-rm -rf $(DISTNAME)
	mkdir $(DISTNAME)
	cp LaunchPad $(DISTNAME)
	cp README $(DISTNAME)
	mkdir $(DISTNAME)/doc
	mimeset -f $(DISTNAME)/*
	zip -r $(DISTNAME).zip $(DISTNAME)
	-rm -rf $(DISTNAME)
