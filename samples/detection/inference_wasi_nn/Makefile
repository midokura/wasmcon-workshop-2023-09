MODULE_NAME = inference_wasi_nn

PROJECTDIR = ../../
include $(PROJECTDIR)/sdk/rules.mk

OBJS=\
	main.o\
	output_tensor_utils.o

TARGET=$(BINDIR)/$(MODULE_NAME).wasm

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CXX) $(PROJ_LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(TARGET)
