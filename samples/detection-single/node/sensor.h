#ifndef SENSOR_H
#define SENSOR_H

#include "senscord/c_api/senscord_c_api.h"
#include <stdint.h>

typedef senscord_core_t core_t;
typedef senscord_stream_t stream_t;
typedef struct {
    struct senscord_raw_data_t rawdata;
    struct senscord_image_property_t property;
} frameinfo_t;

typedef struct {
    senscord_frame_t frame;
    frameinfo_t info[1];
} frame_t;

int32_t sensor_open(core_t *core, stream_t *stream, const char *key);
int32_t sensor_start(stream_t stream);
int32_t sensor_stop(stream_t stream);
int32_t sensor_close(core_t core, stream_t stream);

int32_t sensor_get_frame(stream_t stream, frame_t *frame);
int32_t sensor_release_frame(stream_t stream, frame_t frame);

int32_t sensor_set(stream_t stream, const char *key, const char *value, size_t value_size);

#endif
