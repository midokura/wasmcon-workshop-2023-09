MODULE_NAME = draw_bboxes

PROJECTDIR = ../../../..
include $(PROJECTDIR)/scripts/rules.mk

OBJS=\
	main.o\
	detection_utils.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CXX) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET)
