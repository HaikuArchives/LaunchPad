topsrcdir = .

CXX = g++
CFLAGS = -Wall -Wno-multichar -Wno-ctor-dtor-privacy
INCLUDES = -I$(topsrcdir)/config
CXXCOMPILE = $(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES)

OBJS = src/Main.o src/LaunchPadApp.o src/LaunchPadWindow.o src/WindowDragger.o src/WindowTab.o src/TriStateButton.o

all:	LaunchPad

clean:
	-rm -f src/*.o LaunchPad

LaunchPad:	$(OBJS)
	$(CXX) -o LaunchPad $(OBJS) -lbe -lroot

.SUFFIXES:	.cpp .c .rsrc

.cpp.o:
	$(CXXCOMPILE) -o $@ -c $<
