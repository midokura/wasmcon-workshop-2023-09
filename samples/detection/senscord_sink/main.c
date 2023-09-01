#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evp/sdk.h"
#include "logger.h"
#include "user_bridge_c.h"

#define STREAM_NAME "Viewer"

#ifndef WIDTH
#define WIDTH 300
#endif
#ifndef HEIGHT
#define HEIGHT 300
#endif

static char *module_name = "senscord_sink";
static struct EVP_client *h;

static uint64_t stream_handler = 0;

static void
message_cb(const char *topic, const void *msgPayload, size_t msgPayloadLen,
           void *userData)
{
    LOG_INFO("%s: INPUT (topic=%s, size=%zu)", module_name, topic,
             msgPayloadLen);
    int32_t res = senscord_ub_send_data(stream_handler, (uint8_t *)msgPayload);
    if (res != 0)
        LOG_WARN("senscord_ub_send_data failed.");
}

int
main(int argc, const char *argv[])
{
    int32_t res = 0;
    LOG_DBG("%s: Started!", module_name);
    h = EVP_initialize();
    EVP_RESULT result = EVP_setMessageCallback(h, message_cb, NULL);
    assert(result == EVP_OK);

    LOG_DBG("Creating stream...");
    stream_handler = senscord_ub_create_stream(
        STREAM_NAME, WIDTH, HEIGHT, WIDTH * 3, SENSCORD_PIXEL_FORMAT_RGB24);
    if (stream_handler == 0) {
        LOG_DBG("senscord_ub_create_stream failed.");
        return -1;
    }

    for (;;) {
        result = EVP_processEvent(h, 1000);
        if (result == EVP_SHOULDEXIT) {
            LOG_DBG("%s: exiting the main loop", module_name);
            break;
        }
    }

    res = senscord_ub_destroy_stream(stream_handler);
    if (res != 0) {
        LOG_DBG("senscord_ub_destroy_stream failed.");
        return -1;
    }

    return 0;
}
