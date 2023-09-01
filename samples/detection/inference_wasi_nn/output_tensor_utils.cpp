#include "output_tensor_utils.hpp"

#include "flatbuffers/flatbuffers.h"
#include "output_tensor_generated.h"

char *
creat_output_tensor_fb(float *buf, uint32_t buf_size, uint32_t *out_size)
{
    flatbuffers::FlatBufferBuilder builder;

    auto data = builder.CreateVector(buf, buf_size);
    auto ot = output_tensor::CreateOutputTensor(builder, data);
    builder.Finish(ot);
    *out_size = builder.GetSize();

    char *ret_buffer = (char *)malloc(*out_size);
    memcpy(ret_buffer, builder.GetBufferPointer(), *out_size);
    builder.Clear();
    return ret_buffer;
}
