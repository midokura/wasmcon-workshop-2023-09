#include "evp/sdk.h"
#include "logger.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *module_name = "Sink";
static struct EVP_client *h;

struct send_message_cb_data {
    char *topic;
    char *payload;
};

struct telemetry_data {
    struct EVP_telemetry_entry entries[1];
};

static void telemetry_cb(EVP_TELEMETRY_CALLBACK_REASON reason, void *userData) {
    assert(userData != NULL);
    struct telemetry_data *d = userData;
    assert(d->entries[0].key != NULL);
    assert(d->entries[0].value != NULL);

    free((void *)d->entries[0].key);
    free((void *)d->entries[0].value);
    free(d);
}

static void message_cb(const char *topic, const void *msgPayload, size_t msgPayloadLen, void *userData) {
    LOG_INFO("%s: Received Message %s (topic=%s, size=%zu)", module_name, (char *)msgPayload, topic,
                   msgPayloadLen);

    struct telemetry_data *d = malloc(sizeof(*d));

    d->entries[0].key = strdup(topic);
    d->entries[0].value = strdup((char *)msgPayload);

    EVP_RESULT result = EVP_sendTelemetry(h, d->entries, 1, telemetry_cb, d);
    assert(result == EVP_OK);
}

int main() {
    LOG_INFO("%s Started!", module_name);
    h = EVP_initialize();

    EVP_RESULT result = EVP_setMessageCallback(h, message_cb, NULL);
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
