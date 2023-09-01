#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "evp/sdk.h"
#include "logger.h"
#include "ppl_public.h"

#define OUTPUT_TOPIC "detections"

static const char *module_name = "PPL_DETECTION_SSD";
static struct EVP_client *h = NULL;

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
send_message(char *topic, char *payload, size_t size)
{
    LOG_INFO("%s: Sending Message (topic=%s, size=%zu)", module_name, topic,
             size);
    struct send_message_cb_data *d = malloc(sizeof(*d));
    assert(d != NULL);
    d->topic = strdup(topic);
    assert(d->topic != NULL);
    d->payload = payload;
    assert(d->payload != NULL);
    EVP_RESULT result =
        EVP_sendMessage(h, d->topic, d->payload, size, send_message_cb, d);
    if (EVP_OK != result) {
        LOG_DBG("%s %s %d: calling EVP_sendMessage", module_name, d->topic,
                result);
    }
    assert(result == EVP_OK);
}

static void
message_cb(const char *topic, const void *msgPayload, size_t msgPayloadLen,
           void *userData)
{
    LOG_INFO("%s: Received Message (topic=%s, size=%zu)", module_name, topic,
             msgPayloadLen);

    uint32_t p_out_size = 0;
    bool p_upload_flag = false;
    void *pp_out_buf = NULL;
    EPPL_RESULT_CODE res =
        PPL_Analyze((float *)msgPayload, msgPayloadLen, &pp_out_buf,
                    &p_out_size, &p_upload_flag);

    LOG_DBG("Finished analyzing: %d", p_out_size);

    send_message(OUTPUT_TOPIC, (char *)pp_out_buf, p_out_size);
}

int
main(int argc, const char *argv[])
{
    LOG_DBG("%s: Started!", module_name);
    h = EVP_initialize();
    EVP_RESULT result = EVP_setMessageCallback(h, message_cb, NULL);
    assert(result == EVP_OK);

    for (;;) {
        result = EVP_processEvent(h, 1000);
        if (result == EVP_SHOULDEXIT) {
            LOG_DBG("%s: exiting the main loop", module_name);
            break;
        }
    }
    return 0;
}
