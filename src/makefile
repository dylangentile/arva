CXX := clang++
CXXFLAGS := -I../include -std=c++11 -Wall -Wextra -Weffc++ -pedantic 

DEBUGFLAGS := -g
RELEASEFLAGS := -O3

OBJ := main.o lexer.o parser.o ast.o semantic.o optimizer.o codegen.o symbol.o type.o error.o

.PHONY: all debug release clean

all: debug

debug: 	override CXXFLAGS += $(DEBUGFLAGS)
debug: arvac

release: override CXXFLAGS += $(RELEASEFLAGS)
release: clean arvac

arvac: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o arvac


depend:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -E -MM $(OBJ:.o=.cpp) > .depend

clean:
	-rm *.o arvac
	-rm -rf *.dSYM


include .depend