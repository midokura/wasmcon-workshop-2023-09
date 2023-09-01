MODULE_NAME = senscord_sink

PROJECTDIR = ../../../..
include $(PROJECTDIR)/scripts/rules.mk

OBJS=\
	main.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CC) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET)
