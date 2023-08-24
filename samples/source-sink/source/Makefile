MODULE_NAME = $(shell basename $(CURDIR))
PROJECTDIR = ../..

include $(PROJECTDIR)/sdk/rules.mk

OBJS = \
	main.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CC) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET) *.o
