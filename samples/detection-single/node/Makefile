MODULE_NAME = node

PROJECTDIR = ../../
include $(PROJECTDIR)/sdk/rules.mk

CFLAGS += -O0 -g

OBJS=\
	main.o\
	draw_bbox.o\
	nn.o\
	parson.o\
	sensor.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS)
