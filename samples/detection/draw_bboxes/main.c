#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "detection_utils.hpp"
#include "evp/sdk.h"
#include "logger.h"
#include <assert.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>

struct send_message_cb_data {
    char *topic;
    char *payload;
};

#define INPUT_TOPIC_DETECTIONS "detections"

static const char *module_name = "OPENCV";
static struct EVP_client *h;

// pointer to the image or buffer that contains the native address
static char *image = NULL;
static int32_t image_size = 0;
static char *anns = NULL;
static int32_t anns_size = 0;

typedef struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color;

static color bbox_color = {.r = 255, .g = 255, .b = 0};

static void
config_cb(const char *topic, const void *config, size_t configlen,
          void *userData)
{
    LOG_DBG("Inside config_cb");
    uint32_t hex_value;
    sscanf(config, "%X", &hex_value);
    bbox_color.r = (hex_value >> 16) & 0xFF;
    bbox_color.g = (hex_value >> 8) & 0xFF;
    bbox_color.b = hex_value & 0xFF;
    LOG_INFO("New bounding box color: %d %d %d", bbox_color.r, bbox_color.g,
             bbox_color.b);
}

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

void
send_message(char *topic, char *payload, uint32_t size)
{
    struct send_message_cb_data *d = malloc(sizeof(*d));
    assert(d != NULL);
    d->topic = strdup(topic);
    assert(d->topic != NULL);
    d->payload = payload;
    EVP_RESULT result =
        EVP_sendMessage(h, d->topic, d->payload, size, send_message_cb, d);
    LOG_DBG("%s: send_message topic=%s, size=%d", module_name, d->topic,
             size);
    if (EVP_OK != result) {
        LOG_ERR("%s %s %d: calling EVP_sendMessage", module_name, d->topic,
                result);
    }
    assert(result == EVP_OK);
}

static void
message_cb(const char *topic, const void *msgPayload, size_t msgPayloadLen,
           void *userData)
{
    LOG_DBG("%s: Received Message (topic=%s, size=%zu)", module_name, topic,
             msgPayloadLen);

    char **dst = NULL;
    if (strcmp(topic, INPUT_TOPIC_DETECTIONS) == 0) {
        anns_size = msgPayloadLen;
        dst = (char **)&anns;
    } else {
        image_size = msgPayloadLen;
        dst = (char **)&image;
    }
    if (*dst)
        free(*dst);
    *dst = (char *)malloc(msgPayloadLen);
    memcpy(*dst, msgPayload, msgPayloadLen);

    if (image_size == 0 || anns_size == 0)
        return;

    detections *dets = get_detections(anns);

    CvMat *mat1 = cvCreateMatHeader(300, 300, CV_8UC3);
    cvSetData(mat1, image, CV_AUTOSTEP);

    for (int i = 0; i < dets->size; ++i) {
        CvPoint p1 = {.x = dets->detections[i].x_min,
                      .y = dets->detections[i].y_min};
        CvPoint p2 = {.x = dets->detections[i].x_max,
                      .y = dets->detections[i].y_max};
        CvScalar color;
        color.val[0] = bbox_color.r;
        color.val[1] = bbox_color.g;
        color.val[2] = bbox_color.b;
        color.val[3] = 8;
        cvRectangle(mat1, &p1, &p2, &color, 1, 8, 0);
    }

    cvGetData(mat1, image, 300 * 300 * 3);
    cvReleaseMat(&mat1);
    send_message("postprocessed_image", image, image_size);

    free(dets->detections);
    free(dets);
    free(anns);
    image = NULL;
    anns = NULL;
    image_size = 0;
    anns_size = 0;
}

int
main(int argc, const char *argv[])
{
    LOG_INFO("%s: Started!", module_name);
    h = EVP_initialize();

    EVP_RESULT result = EVP_setMessageCallback(h, message_cb, NULL);
    assert(result == EVP_OK);
    result = EVP_setConfigurationCallback(h, config_cb, NULL);
    assert(result == EVP_OK);

    for (;;) {
        result = EVP_processEvent(h, 1000);

        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("%s: exiting the main loop", module_name);
            break;
        }
    }
    return 0;
}
