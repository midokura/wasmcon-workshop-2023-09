MODULE_NAME = senscord_sink

PROJECTDIR = ../../
include $(PROJECTDIR)/sdk/rules.mk

CFLAGS += -DWINDOW_NAME=\"$(GITHUB_USER)\"

OBJS=\
	main.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET)
