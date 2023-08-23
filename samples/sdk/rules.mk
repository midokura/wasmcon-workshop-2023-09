CC = /opt/wasi-sdk/bin/clang
CXX = /opt/wasi-sdk/bin/clang++

PROJ_LDFLAGS = \
	-Wl,-allow-undefined \
	-z stack-size=32768 \
	-Wl,--initial-memory=655360 \
	-Wl,--export=malloc -Wl,--export=free \
	-Wl,--export=__data_end -Wl,--export=__heap_base

BINDIR = ../bin

CFLAGS =
CINCLUDES = \
	-I$(PROJECTDIR)/sdk/include
PROJ_CFLAGS = \
	$(CFLAGS) \
	$(CINCLUDES)

CXXFLAGS = \
	$(CFLAGS)
CXXINCLUDES = \
	$(CINCLUDES)
PROJ_CFLAGS = \
	$(CXXFLAGS) \
	$(CXXINCLUDES)

all:

FORCE:

.SUFFIXES: .c .o .cpp

.c.o:
	$(CC) $(PROJ_CFLAGS) -c $<

.cpp.o:
	$(CXX) $(PROJ_CXXFLAGS) -c $<

$(DIRS): FORCE
	+@cd $@ && $(MAKE)

clean: clean-dirs

clean-dirs:
	@+set -e;\
	for i in $(DIRS);\
	do\
		cd $$i;\
		$(MAKE) clean;\
		cd -;\
	done
