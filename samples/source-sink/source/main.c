#include "evp/sdk.h"
#include "logger.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static const char *module_name = "Source";
static struct EVP_client *h;
static char *g_publish_to = "my-topic";

struct send_message_cb_data {
    char *topic;
    char *payload;
};

static void send_message_cb(EVP_MESSAGE_SENT_CALLBACK_REASON reason, void *userData) {
    struct send_message_cb_data *d = userData;
    assert(d != NULL);
    assert(d->topic != NULL);
    assert(d->payload != NULL);
    free(d->topic);
    free(d->payload);
    free(d);
}

static void send_message() {
    LOG_INFO("In send_message\n");

    struct send_message_cb_data *d = malloc(sizeof(*d));
    assert(d != NULL);

    d->topic = strdup(g_publish_to);
    assert(d->topic != NULL);

    d->payload = (char *)malloc(128);
    assert(d->payload != NULL);
    time_t ltime = time(NULL);
    char *date = asctime(localtime(&ltime));
    date[strlen(date)-1] = 0;
    sprintf(d->payload, "{\"date\":\"%s\",\"my-key\":\"my-value\"}", date);
    free(date);

    EVP_RESULT result = EVP_sendMessage(h, d->topic, d->payload, strlen(d->payload), send_message_cb, d);

    if (EVP_OK != result) {
        LOG_WARN("%s %s %d: calling EVP_sendMessage\n", module_name, d->topic, result);
    }
    assert(result == EVP_OK);
    LOG_INFO("Exiting send_message");
}

int main() {
    LOG_INFO("%s Started!", module_name);
    h = EVP_initialize();

    for (;;) {
        EVP_RESULT result = EVP_processEvent(h, 1000);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("%s: exiting the main loop", module_name);
            break;
        }
        sleep(2);
        send_message();
        result = EVP_processEvent(h, 1000);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("%s: exiting the main loop", module_name);
            break;
        }
    }
    return 0;
}
