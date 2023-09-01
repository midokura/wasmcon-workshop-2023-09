MODULE_NAME = ppl_detection_ssd

PROJECTDIR = ../../../..
include $(PROJECTDIR)/scripts/rules.mk

OBJS=\
	main.o\
	ppl_detection_ssd.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CXX) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET)
