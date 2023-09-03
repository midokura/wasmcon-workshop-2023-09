#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _POSIX_C_SOURCE 199309L
#include <time.h>

#include "evp/sdk.h"
#include "logger.h"
#include "senscord/c_api/senscord_c_api.h"
#include "senscord_wasm.h"
#include <opencv2/imgproc/imgproc_c.h>

#define OUTPUT_TOPIC1 "input_tensor"
#define OUTPUT_TOPIC2 "image"
#define INPUT_TOPIC   "give_input_tensor"

#ifndef WIDTH
#define WIDTH 300
#endif
#ifndef HEIGHT
#define HEIGHT 300
#endif

static const char *module_name = "senscord_source";
static struct EVP_client *h;

// "webcam_image_stream.0" or "raspicam_image_stream.0"
static char *stream_key = NULL;
static senscord_core_t core = 0;
static senscord_stream_t stream = 0;

static uint32_t cam_width = 0;
static uint32_t cam_height = 0;
static bool is_yuv = false;
static bool do_resize = false;

static bool ready_receive = false;

struct senscord_raw_data_wasm_t {
    uint32_t address;
    uint32_t size;
    uint32_t type;
    uint64_t timestamp;
};

struct send_message_cb_data {
    char *topic;
    char *payload;
};

static void
send_message_cb(EVP_MESSAGE_SENT_CALLBACK_REASON reason, void *userData)
{
    struct send_message_cb_data *d = userData;
    assert(d != NULL);
    assert(d->topic != NULL);
    assert(d->payload != NULL);
    free(d->topic);
    free(d->payload);
    free(d);
}

static void
send_message(char *topic, char *rgbdata, uint32_t outsize)
{
    LOG_DBG("Sending message to topic %s with size %d", topic, outsize);
    struct send_message_cb_data *d = malloc(sizeof(*d));
    assert(d != NULL);
    d->topic = strdup(topic);
    assert(d->topic != NULL);
    d->payload = rgbdata;
    EVP_RESULT result =
        EVP_sendMessage(h, d->topic, d->payload, outsize, send_message_cb, d);
    assert(result == EVP_OK);
}

static void
print_senscord_error(struct senscord_status_t status)
{
    LOG_INFO("status:\n - level  : %d\n - cause  : %d\n - message: %s\n"
             " - block  : %s\n - trace  : %s",
             status.level, status.cause, status.message, status.block,
             status.trace);
}

int32_t
get_and_update_image_property()
{
    int32_t ret = 0;
    struct senscord_image_property_t image_property;
    ret =
        senscord_stream_get_property(stream, SENSCORD_IMAGE_PROPERTY_KEY,
                                     &image_property, sizeof(image_property));
    LOG_INFO("senscord_stream_get_property(): ret=%d", ret);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }
    LOG_DBG("image_property height = %d", image_property.height);
    LOG_DBG("image_property width = %d", image_property.width);
    LOG_DBG("image_property stride_bytes = %d", image_property.stride_bytes);
    LOG_DBG("image_property pixel_format = %s", image_property.pixel_format);
    do_resize =
        image_property.height != HEIGHT | image_property.width != WIDTH;
    is_yuv = strcmp(image_property.pixel_format, "image_nv16") == 0;

    cam_height = image_property.height;
    cam_width = image_property.width;
    return 0;
}

void
convert_nv16_to_rgb(const uint8_t *nv16_data, uint8_t *rgb_data)
{
    int size = cam_width * cam_height;
    int rgb_data_size = HEIGHT * WIDTH * 3;

    LOG_DBG("is_yuv = %d, do-resize = %d", is_yuv, do_resize);
    CvMat *mat2 = NULL;
    if (is_yuv) {
        uint8_t *yuv = (uint8_t *)malloc(size * 2);
        for (int i = 0; i < size; ++i) {
            yuv[i * 2] = nv16_data[i];
            yuv[i * 2 + 1] = nv16_data[size + i];
        }

        CvMat *mat1 = cvCreateMat(cam_height, cam_width, CV_8UC2);
        cvSetData(mat1, yuv, CV_AUTOSTEP);
        mat2 = cvCreateMat(cam_height, cam_width, CV_8UC3);
        cvCvtColor(mat1, mat2, CV_YUV2BGR_YUYV);
        cvCvtColor(mat2, mat2, CV_BGR2RGB);
        cvReleaseMat(&mat1);
        free(yuv);
    } else {
        mat2 = cvCreateMat(cam_height, cam_width, CV_8UC3);
        cvSetData(mat2, (void *)nv16_data, CV_AUTOSTEP);
    }

    if (do_resize) {
        CvMat *mat3 = cvCreateMat(HEIGHT, WIDTH, CV_8UC3);
        cvResize(mat2, mat3, CV_INTER_NN);

        cvGetData(mat3, rgb_data, rgb_data_size);
        cvReleaseMat(&mat3);
    } else {
        cvGetData(mat2, rgb_data, rgb_data_size);
    }
    cvReleaseMat(&mat2);
    LOG_INFO("Conversion to rgb done");
}

static int
get_frame(unsigned char **rgbdata)
{
    int32_t ret = 0;

    senscord_frame_t frame;
    ret = senscord_stream_get_frame(stream, &frame, 3000);
    LOG_DBG("senscord_stream_get_frame(): ret=%d\n", ret);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }

    uint32_t count = 0;
    ret = senscord_frame_get_channel_count(frame, &count);
    assert(count == 1);

    senscord_channel_t channel;
    ret = senscord_frame_get_channel(frame, 0, &channel);
    LOG_DBG("senscord_frame_get_channel(): ret=%d, index=%u", ret, 0);

    struct senscord_raw_data_t rawdata;
    char format[64] = {0};
    ///rawdata.type = (uint32_t)format;
    ret = senscord_channel_get_raw_data(channel, &rawdata);
    LOG_DBG("senscord_channel_get_raw_data(): ret=%d", ret);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }

    LOG_DBG("rawdata address = %" PRIu64 " size = %zu timestamp = %" PRIu64
            " type = %s",
            rawdata.address, rawdata.size, rawdata.timestamp,
            (char *)rawdata.type);

    uint8_t *nv16_data = (uint8_t *)malloc(rawdata.size);
    senscord_memcpy((uint32_t)nv16_data, (uint64_t)rawdata.address,
                    rawdata.size);

    *rgbdata = (unsigned char *)malloc(640 * 480 * 3);
    convert_nv16_to_rgb(nv16_data, *rgbdata);
    free(nv16_data);

    ret = senscord_stream_release_frame(stream, frame);
    LOG_DBG("senscord_stream_release_frame(): ret=%d", ret);
    if (ret != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }
    return 0;
}

static void
message_cb(const char *topic, const void *msgPayload, size_t msgPayloadLen,
           void *userData)
{
    LOG_INFO("%s: INPUT (topic=%s, size=%zu)", module_name, topic,
             msgPayloadLen);
    if (strcmp(INPUT_TOPIC, topic) == 0)
        ready_receive = true;
}

static void
send_frame()
{
    unsigned char *buf = NULL;
    get_frame(&buf);

    uint32_t size = WIDTH * HEIGHT * 3;
    char *buf_aux = (char *)malloc(size);
    memcpy(buf_aux, buf, size);
    send_message(OUTPUT_TOPIC1, (char *)buf, size);
    send_message(OUTPUT_TOPIC2, buf_aux, size);
    ready_receive = false;
}

void
rpc_callback(EVP_RPC_ID id, const char *methodName, const char *params,
             void *userData)
{
    LOG_DBG("RPC: methodName=%s params=%s", methodName, params);
    if (strcmp(methodName, "config") == 0) {
        stream_key = strdup(params);
    } else {
        LOG_WARN("Invalid RPC.");
    }
}

int
main(int argc, const char *argv[])
{
    int32_t res = 0;
    LOG_DBG("%s: Started!", module_name);
    h = EVP_initialize();
    EVP_RESULT result = EVP_setMessageCallback(h, message_cb, NULL);
    assert(result == EVP_OK);
    result = EVP_setRpcCallback(h, rpc_callback, NULL);
    assert(result == EVP_OK);

    while (stream_key == NULL) {
        result = EVP_processEvent(h, 10);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("Exiting the main loop");
            goto END2;
        }
    }

    res = senscord_core_init(&core);
    if (res != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }

    res = senscord_core_open_stream(core, stream_key, &stream);
    LOG_DBG("senscord_core_open_stream(): ret=%d, stream=%ju", res,
            (uintmax_t)stream);
    if (res != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }

    res = senscord_stream_start(stream);
    LOG_DBG("senscord_stream_start(): ret=%d", res);
    if (res != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }
    if (get_and_update_image_property() != 0)
        return -1;

    LOG_DBG("Starting...");
    for (;;) {
        if (ready_receive)
            send_frame();

        for (int i = 0; i < 10; ++i) {
            result = EVP_processEvent(h, 10);
            if (result == EVP_SHOULDEXIT) {
                LOG_DBG("%s: exiting the main loop", module_name);
                goto END;
            }
        }
    }
END:
    res = senscord_stream_stop(stream);
    LOG_DBG("senscord_stream_stop(): ret=%d", res);
    if (res != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }

    res = senscord_core_close_stream(core, stream);
    LOG_DBG("senscord_core_close_stream(): ret=%d", res);
    if (res != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }

    res = senscord_core_exit(core);
    LOG_DBG("senscord_core_exit(): ret=%d", res);
    if (res != 0) {
        print_senscord_error(senscord_get_last_error());
        return -1;
    }
END2:
    free(stream_key);
    return 0;
}
