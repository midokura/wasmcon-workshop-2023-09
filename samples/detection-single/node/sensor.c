/*
 * Copyright 2021 Sony Semiconductor Solutions Corporation.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Semiconductor
 * Solutions Corporation.
 * No part of this file may be copied, modified, sold, and distributed in any
 * form or by any means without prior explicit permission in writing from
 * Sony Semiconductor Solutions Corporation.
 *
 */

#include "logger.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "senscord/c_api/senscord_c_api.h"
#include "sensor.h"

#define INPUT_TENSOR_SIZE   (300)
#define GET_FRAME_WAIT_MSEC (3000)

static void
print_senscord_error(struct senscord_status_t status)
{
    LOG_ERR("status:\n - level  : %d\n - cause  : %d\n - message: %s\n"
            " - block  : %s\n - trace  : %s",
            status.level, status.cause, status.message, status.block,
            status.trace);
}

int32_t
sensor_open(core_t *core, stream_t *stream, const char *key)
{
    int32_t ret = senscord_core_init(core);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_core_init failed");
        return ret;
    }
    ret = senscord_core_open_stream(*core, key, stream);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_core_open_stream failed");
        return ret;
    }
    return ret;
}

int32_t
sensor_start(stream_t stream)
{
    int32_t ret = senscord_stream_start(stream);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_stream_start failed");
        return ret;
    }
    return ret;
}

int32_t
sensor_stop(stream_t stream)
{
    int32_t ret = senscord_stream_stop(stream);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_stream_stop failed");
        return ret;
    }
    return ret;
}

int32_t
sensor_close(core_t core, stream_t stream)
{
    int32_t ret = senscord_core_close_stream(core, stream);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_stream_close failed");
        return ret;
    }
    ret = senscord_core_exit(core);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_core_exit failed");
        return ret;
    }
    return ret;
}


int32_t
sensor_get_frame(stream_t stream, frame_t *frame)
{
    /* Get Frame */
    int32_t ret = senscord_stream_get_frame(stream, &(frame->frame),
                                            GET_FRAME_WAIT_MSEC);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_stream_get_frame failed");
        return ret;
    }

    /* Get Channel */
    senscord_channel_t channel;
    ret = senscord_frame_get_channel(frame->frame, 0, &channel);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_frame_get_channel failed");
        return ret;
    }

    /* Get Raw Data */
    ret = senscord_channel_get_raw_data(channel, &(frame->info[0].rawdata));
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        LOG_ERR("senscord_channel_get_raw_data failed");
        return ret;
    }

    /* Get Image Property */
    ret = senscord_channel_get_property(
        channel, SENSCORD_IMAGE_PROPERTY_KEY, &(frame->info[0].property),
        sizeof(struct senscord_image_property_t));
    return ret;
}

int32_t
sensor_release_frame(stream_t stream, frame_t frame)
{
    int32_t ret = senscord_stream_release_frame(stream, frame.frame);
    if (ret != 0) {
        LOG_ERR("senscord_frame_release failed");
        return ret;
    }
    return ret;
}

int32_t
sensor_set(stream_t stream, const char *key, const char *value, size_t value_size)
{

    int32_t ret = senscord_stream_set_property(stream, key, value, value_size);
    if (ret != 0) {
        LOG_ERR("senscord_stream_set_property failed");
        return ret;
    }
    return ret;
}
