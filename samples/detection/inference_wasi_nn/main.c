#include <assert.h>
#include <math.h>
#define _GNU_SOURCE
#include <stdio.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "evp/sdk.h"
#include "logger.h"
#include "output_tensor_utils.hpp"
#include "wasi_nn.h"
#include "wasi_nn_types.h"

#include <sys/time.h>

/* Topic name definition */
#define OUTPUT_TOPIC  "output_tensor"
#define REQUEST_TOPIC "give_input_tensor"

#define WIDTH  300
#define HEIGHT 300

static char *model_url = NULL;
static char *model_file = NULL;
static bool downloaded_model = false;

#define EPSILON                1e-8
#define MAX_OUTPUT_TENSOR_SIZE 1000000
#define MAX_MODEL_SIZE         85000000
#define MAX_OUTPUT_TENSORS     4

#define DEVICE cpu

struct timeval start, end;
double total;
static int input_size = HEIGHT * WIDTH * 3;

typedef enum {
    DOWNLOAD_MODEL = 0,
    LOAD_MODEL,
    GET_DATA,
    RUN_MODEL
} STATE_MODEL;

static STATE_MODEL state = DOWNLOAD_MODEL;
static graph_execution_context gec;

static const char *module_name = "INFERENCE";
static struct EVP_client *h;
static char *g_publish_to;

struct send_message_cb_data {
    char *topic;
    char *payload;
};

typedef struct {
    char *download;
    char *filename;

    struct EVP_BlobLocalStore localStore;
} module_vars_t;

typedef struct {
    module_vars_t *ctx;
    char *blob_url;
} blob_cb_data_t;

static void
blob_cb(EVP_BLOB_CALLBACK_REASON reason, const void *vp, void *userData)
{
    assert(userData != NULL);
    const struct EVP_BlobResultAzureBlob *result;

    blob_cb_data_t *cb_data = (blob_cb_data_t *)userData;
    module_vars_t *module_vars = cb_data->ctx;

    switch (reason) {
    case EVP_BLOB_CALLBACK_REASON_DONE:
        result = vp;
        LOG_INFO("result=%u "
                 "http_status=%u error=%d",
                 result->result, result->http_status, result->error);
        state = LOAD_MODEL;
        break;
    case EVP_BLOB_CALLBACK_REASON_EXIT:
        assert(vp == NULL);
        break;
    default:
        assert(0);
    }

    free(cb_data->blob_url);
    cb_data->blob_url = NULL;
    free(module_vars->filename);
    module_vars->filename = NULL;
    module_vars->localStore.filename = NULL;
}

static void
send_message_cb(EVP_MESSAGE_SENT_CALLBACK_REASON reason, void *userData)
{
    struct send_message_cb_data *d = userData;
    assert(d != NULL);
    assert(d->topic != NULL);
    free(d->topic);
    if (d->payload)
        free(d->payload);
    free(d);
}

static void
send_message(char *topic, char *buf, int size)
{
    LOG_DBG("entering send_inference");
    struct send_message_cb_data *d = malloc(sizeof(*d));
    assert(d != NULL);

    d->topic = strdup(topic);
    d->payload = buf;
    EVP_RESULT result =
        EVP_sendMessage(h, d->topic, d->payload, size, send_message_cb, d);

    if (EVP_OK != result) {
        LOG_DBG("%s %s %d: calling EVP_sendMessage", module_name, d->topic,
                result);
    }
    static int outs = 1;
    LOG_DBG("%s: OUTPUT (%d) (topic=%s, size=%d)", module_name, outs++,
             d->topic, (int)size);
}

static char *
run_inference(char *image_buf, int image_buf_size, uint32_t *out_size)
{
    uint32_t dim[] = {1, HEIGHT, WIDTH, 3};

    tensor_dimensions dims;
    dims.size = 4;
    dims.buf = (uint32_t *)malloc(dims.size * sizeof(uint32_t));

    tensor tensor;
    tensor.dimensions = &dims;
    for (int i = 0; i < tensor.dimensions->size; ++i)
        tensor.dimensions->buf[i] = dim[i];
    tensor.type = fp32;
    tensor.data = (uint8_t *)image_buf;
    error err = set_input(gec, 0, &tensor);

    free(dims.buf);
    struct timeval start, end;
    gettimeofday(&start, NULL);

    compute(gec);

    gettimeofday(&end, NULL);

    float seconds = end.tv_sec - start.tv_sec +
                    (float)(end.tv_usec - start.tv_usec) / 1000000.0;
    LOG_DBG("Running model time is %fs", seconds);

    float *output_tensor =
        (float *)malloc(sizeof(float) * MAX_OUTPUT_TENSOR_SIZE);

    uint32_t offset = 0;
    for (int i = 0; i < MAX_OUTPUT_TENSORS; ++i) {
        *out_size = MAX_OUTPUT_TENSOR_SIZE - *out_size;
        error err =
            get_output(gec, i, (uint8_t *)&output_tensor[offset], out_size);
        if (err != success)
            break;
        offset += *out_size;
    }
    *out_size = offset;

    char *fb = creat_output_tensor_fb(output_tensor, *out_size, out_size);
    free(output_tensor);
    LOG_DBG("exiting run_inference");
    return fb;
}

static void
load_model()
{
    LOG_DBG("Loading model...");
    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE *pFile = fopen(model_file, "r");
    assert(pFile != NULL);

    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * MAX_MODEL_SIZE);
    if (buffer == NULL) {
        LOG_ERR("Memory error");
        exit(2);
    }
    size_t result = fread(buffer, 1, MAX_MODEL_SIZE, pFile);
    graph_builder_array arr;
    arr.buf = (graph_builder *)malloc(sizeof(graph_builder));
    arr.size = 1;
    arr.buf[0].buf = buffer;
    arr.buf[0].size = result;

    graph graph;
    error err = load(&arr, tensorflowlite, DEVICE, &graph);
    LOG_DBG("Status of load: %d", err);

    fclose(pFile);
    free(arr.buf);
    free(buffer);

    init_execution_context(graph, &gec);
    gettimeofday(&end, NULL);

    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    state = GET_DATA;
    LOG_DBG("Loading model time is %ld seconds and %ld micros", seconds,
            micros);
}

static void
download_model()
{
    LOG_DBG("Loading model from: %s", model_url);
    module_vars_t module_vars;
    module_vars.download = strdup(model_url);
    module_vars.filename = strdup(model_file);
    module_vars.localStore.filename = module_vars.filename;
    module_vars.localStore.io_cb = NULL;
    module_vars.localStore.blob_len = 0;

    blob_cb_data_t cb_data;
    cb_data.blob_url = module_vars.download;
    cb_data.ctx = &module_vars;

    struct EVP_BlobRequestAzureBlob request;
    request.url = module_vars.download;
    EVP_RESULT result = EVP_blobOperation(
        h, EVP_BLOB_TYPE_AZURE_BLOB, EVP_BLOB_OP_GET, &request,
        &module_vars.localStore, blob_cb, &cb_data);

    assert(result == EVP_OK);
}

static void
message_cb(const char *topic, const void *msgPayload, size_t msgPayloadLen,
           void *userData)
{
    static int inps = 1;
    LOG_INFO("%s: Received Message: (%d) (topic=%s, size=%zu)", module_name,
             inps++, topic, msgPayloadLen);

    if (state != RUN_MODEL) {
        LOG_INFO("Model not loaded, skipping!");
        return;
    }

    uint32_t out_size;
    float *input_tensor = (float *)malloc(input_size * sizeof(float));
    uint8_t *input_tensor_n = (uint8_t *)msgPayload;
    for (int i = 0; i < input_size; ++i) {
        input_tensor[i] = ((float)input_tensor_n[i]) / 255;
    }
    char *output_tensor_fb =
        run_inference((char *)input_tensor, input_size, &out_size);
    free(input_tensor);
    gettimeofday(&end, NULL);
    total = (end.tv_sec - start.tv_sec) +
            (end.tv_usec - start.tv_usec) / 1000000.0;
    LOG_DBG("Total time: %f seconds", total);
    gettimeofday(&start, NULL);

    send_message(OUTPUT_TOPIC, output_tensor_fb, out_size);

    state = GET_DATA;
}

void
rpc_callback(EVP_RPC_ID id, const char *methodName, const char *params,
             void *userData)
{
    LOG_DBG("RPC: methodName=%s params=%s", methodName, params);
    if (strcmp(methodName, "config") == 0) {
        model_url = strdup(params);
    } else {
        LOG_WARN("Invalid RPC.");
    }
}

int
main(int argc, const char *argv[])
{
    LOG_DBG("%s: Started!", module_name);
    h = EVP_initialize();

    EVP_RESULT result = EVP_setMessageCallback(h, message_cb, NULL);
    assert(result == EVP_OK);
    result = EVP_setRpcCallback(h, rpc_callback, NULL);
    assert(result == EVP_OK);

    while (model_url == NULL) {
        result = EVP_processEvent(h, 10);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("Exiting the main loop");
            goto END;
        }
    }

    const char *workspace =
        EVP_getWorkspaceDirectory(h, EVP_WORKSPACE_TYPE_DEFAULT);
    asprintf(&model_file, "%s/%s", workspace, "model.tflite");
    download_model();

    for (;;) {
        EVP_RESULT result = EVP_processEvent(h, 1000);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("%s: exiting the main loop", module_name);
            break;
        }
        if (state == LOAD_MODEL) {
            load_model();
            state = GET_DATA;

        } else if (state == GET_DATA) {
            LOG_INFO("Requesting tensors...");
            send_message(REQUEST_TOPIC, NULL, 0);
            state = RUN_MODEL;
        }
    }
END:
    free(model_url);
    free(model_file);
    return 0;
}
