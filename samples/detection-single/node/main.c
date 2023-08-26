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

#define _GNU_SOURCE /* asprintf */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"
#include "draw_bbox.h"
#include "evp/sdk.h"
#include "logger.h"
#include "nn.h"
#include "parson.h"
#include "senscord/c_api/senscord_c_api.h"
#include "senscord_wasm.h"
#include "sensor.h"
#include "user_bridge_c.h"
#include <opencv2/imgproc/imgproc_c.h>

static struct EVP_client *h = NULL;

static senscord_core_t core = 0;
// "webcam_image_stream.0" or "raspicam_image_stream.0"
static char *stream_key = NULL;
static senscord_stream_t stream = 0;

static uint64_t rgb_handle = 0;
static graph_execution_context ctx;

static char *model_url = NULL;
static char *model_file = NULL;
static bool downloaded_model = false;

static uint32_t cam_width = 0;
static uint32_t cam_height = 0;
static struct senscord_image_sensor_function_property_t picture_quality = {
    .brightness = 0,
    .gain_value = 255,
    .saturation = 255,
    .sharpness = 255,
};

static bool is_yuv = false;
static bool do_resize = false;

typedef struct {
    float *bbox;
    float *score;
    float *class;
    float numofdetections;
} inference_data_t;

struct telemetry_data {
    struct EVP_telemetry_entry entries[1];
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

void
convert_nv16_to_rgb(const uint8_t *nv16_data, uint8_t *rgb_data)
{
    int size = cam_width * cam_height;
    int rgb_data_size = INPUT_TENSOR_SIZE * INPUT_TENSOR_SIZE * 3;

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
        CvMat *mat3 =
            cvCreateMat(INPUT_TENSOR_SIZE, INPUT_TENSOR_SIZE, CV_8UC3);
        cvResize(mat2, mat3, CV_INTER_NN);

        cvGetData(mat3, rgb_data, rgb_data_size);
        cvReleaseMat(&mat3);
    } else {
        cvGetData(mat2, rgb_data, rgb_data_size);
    }
    cvReleaseMat(&mat2);
    LOG_INFO("Conversion to rgb done");
}

static void
telemetry_cb(EVP_TELEMETRY_CALLBACK_REASON reason, void *userData)
{
    assert(userData != NULL);
    struct telemetry_data *d = userData;
    assert(d->entries[0].key != NULL);
    assert(d->entries[0].value != NULL);
    free((void *)d->entries[0].key);
    free((void *)d->entries[0].value);
    free(d);
}

static uint64_t
get_microsecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((uint64_t)ts.tv_sec) * 1000 * 1000 + ((uint64_t)ts.tv_nsec) / 1000;
}

static void
cleanup_inference_data(inference_data_t *inference_data)
{
    if (inference_data->bbox != NULL)
        free(inference_data->bbox);
    if (inference_data->score != NULL)
        free(inference_data->score);
    if (inference_data->class != NULL)
        free(inference_data->class);
}

static int32_t
inference(frame_t frame, inference_data_t *inference_data)
{
    uint32_t raw_image_size = frame.info[0].rawdata.size;
    uint8_t *raw_image = (uint8_t *)malloc(raw_image_size);
    if (raw_image == NULL) {
        LOG_INFO("malloc failed");
        return -1;
    }

    senscord_memcpy((uint32_t)raw_image,
                    (uint64_t)frame.info[0].rawdata.address, raw_image_size);

    uint32_t rgb_image_size = INPUT_TENSOR_SIZE * INPUT_TENSOR_SIZE * 3;
    uint8_t *rgb_image = malloc(rgb_image_size);
    convert_nv16_to_rgb(raw_image, rgb_image);

    free(raw_image);
    float *input_tensor = (float *)malloc(rgb_image_size * sizeof(float));
    for (int i = 0; i < rgb_image_size; ++i) {
        input_tensor[i] = ((float)rgb_image[i]) / 255;
    }

    uint64_t start_time = get_microsecond();
    uint32_t dim[] = {1, INPUT_TENSOR_SIZE, INPUT_TENSOR_SIZE, 3};
    if (wasm_set_input(ctx, (uint8_t *)input_tensor, (uint32_t *)dim) !=
        success) {
        fprintf(stderr, "Error when setting input tensor.");
        exit(1);
    }
    free(input_tensor);
    if (wasm_compute(ctx) != success) {
        fprintf(stderr, "Error when running inference.");
        exit(1);
    }
    uint64_t end_time = get_microsecond();
    LOG_INFO("Inference time: %f ms", (end_time - start_time) / 1000.0);
    uint32_t output_size = 1;

    if (wasm_get_output(ctx, 2, &inference_data->numofdetections,
                        &output_size) != success) {
        fprintf(stderr, "Error when setting output tensor.");
        exit(1);
    }

    LOG_INFO("numofdetections %f", inference_data->numofdetections);
    if (inference_data->numofdetections > 0) {
        LOG_INFO("Drawing bboxes");
        output_size = MAX_BBOXES * 4;
        inference_data->bbox = malloc(sizeof(float) * output_size);
        if (wasm_get_output(ctx, 1, inference_data->bbox, &output_size) !=
            success) {
            LOG_ERR("Error while extracting bboxes");
            exit(1);
        }

        output_size = MAX_BBOXES;
        inference_data->score = malloc(sizeof(float) * output_size);
        if (wasm_get_output(ctx, 0, inference_data->score, &output_size) !=
            success) {
            LOG_ERR("Error while extracting scores");
            exit(1);
        }

        output_size = MAX_BBOXES;
        inference_data->class = malloc(sizeof(float) * output_size);
        if (wasm_get_output(ctx, 2, inference_data->class, &output_size) !=
            success) {
            LOG_ERR("Error while extracting classes");
            exit(1);
        }

        for (int i = 0; i < (uint8_t)inference_data->numofdetections; i++) {
            LOG_INFO("Bbox %i-th:", i);
            LOG_INFO("Score: %f", inference_data->score[i]);
            LOG_INFO("Class: %f", inference_data->class[i]);
            if (inference_data->score[i] < 0.7) {
                continue;
            }
            struct senscord_rectangle_region_parameter_t crop_prop = {
                inference_data->bbox[i * 4] * INPUT_TENSOR_SIZE,
                inference_data->bbox[i * 4 + 1] * INPUT_TENSOR_SIZE,
                inference_data->bbox[i * 4 + 2] * INPUT_TENSOR_SIZE,
                inference_data->bbox[i * 4 + 3] * INPUT_TENSOR_SIZE};
            frame_bbox(rgb_image, rgb_image_size, &crop_prop,
                       inference_data->score[i], inference_data->class[i]);
        }
    }
    senscord_ub_send_data(rgb_handle, rgb_image);
    free(rgb_image);
    return 0;
}

static int32_t
telemetry(inference_data_t inference_data)
{
    struct telemetry_data *d = malloc(sizeof(*d));

    /* Create text data */
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char bbox_info[100];
    for (int i = 0; i < (uint8_t)inference_data.numofdetections; i++) {
        sprintf(bbox_info, "ymin %f, xmin %f, ymax %f, xmax %f",
                inference_data.bbox[i * 4], inference_data.bbox[i * 4 + 1],
                inference_data.bbox[i * 4 + 2],
                inference_data.bbox[i * 4 + 3]);

        json_object_dotset_number(root_object, "score",
                                  inference_data.score[i]);
        json_object_dotset_string(root_object, "bbox", bbox_info);
    }
    if ((uint8_t)inference_data.numofdetections > 0) {
        char *result_upload = json_serialize_to_string_pretty(root_value);

        d->entries[0].key = strdup("top-1_detection");
        d->entries[0].value = strdup((char *)result_upload);
        EVP_RESULT result =
            EVP_sendTelemetry(h, d->entries, 1, telemetry_cb, d);
        assert(result == EVP_OK);
        LOG_INFO("Send output tensor into Hub");

        json_free_serialized_string(result_upload);
        json_value_free(root_value);
    }
    return 0;
}

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
    downloaded_model = true;
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

void
rpc_callback(EVP_RPC_ID id, const char *methodName, const char *params,
             void *userData)
{
    LOG_DBG("RPC: methodName=%s params=%s", methodName, params);
    bool is_device_setting = false;
    if (strcmp(methodName, "config") == 0) {
        JSON_Value *schema = json_parse_string(params);
        JSON_Object *object = json_value_get_object(schema);
        stream_key = strdup(json_object_dotget_string(object, "stream"));
        model_url = strdup(json_object_dotget_string(object, "model"));
        json_value_free(schema);
    } else if (strcmp(methodName, "rgb") == 0) {
        int r, g, b;
        sscanf(params, "%02x%02x%02x", &r, &g, &b);
        change_color((uint8_t)r, (uint8_t)g, (uint8_t)b);
    } else if (strncmp(methodName, "brightness", 7) == 0) {
        picture_quality.brightness= atoi(params);
        is_device_setting = true;
    } else if (strncmp(methodName, "contrast", 7) == 0) {
        picture_quality.gain_value=atoi(params);
        is_device_setting = true;
    } else if (strncmp(methodName, "saturation", 10) == 0) {
        picture_quality.saturation=atoi(params);
        is_device_setting = true;
    } else if (strncmp(methodName, "sharpness", 9) == 0) {
        picture_quality.sharpness=atoi(params);
        is_device_setting = true;
    }

    if (stream != 0 && is_device_setting)
        sensor_set(stream, SENSCORD_IMAGE_SENSOR_FUNCTION_PROPERTY_KEY, (char *)&picture_quality, sizeof(picture_quality));
}

int
main()
{
    int32_t ret = 0;
    EVP_RESULT result = EVP_OK;
    LOG_INFO("Started!");
    h = EVP_initialize();
    result = EVP_setRpcCallback(h, rpc_callback, NULL);
    assert(result == EVP_OK);

    const char *workspace =
        EVP_getWorkspaceDirectory(h, EVP_WORKSPACE_TYPE_DEFAULT);

    ret = asprintf(&model_file, "%s/%s", workspace, "detection.tflite");
    if (ret == -1) {
        LOG_ERR("Error while creating model file name");
        return -1;
    }

    while (stream_key == NULL || model_url == NULL) {
        result = EVP_processEvent(h, 10);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("Exiting the main loop");
            break;
        }
    }

    download_model();
    while (!downloaded_model) {
        result = EVP_processEvent(h, 10);
        if (result == EVP_SHOULDEXIT) {
            LOG_INFO("Exiting the main loop");
            break;
        }
    }
    LOG_INFO("Model is downloaded!");

    /* prepare inference*/
    graph graph;
    if (wasm_load(model_file, &graph, cpu) != success) {
        LOG_ERR("Error when loading model.");
        goto END;
    }

    if (wasm_init_execution_context(graph, &ctx) != success) {
        LOG_ERR("Error when initialixing execution context.");
        goto END;
    }

    /* prepare camera */
    if (sensor_open(&core, &stream, stream_key) != 0) {
        LOG_ERR("sensor_open failed");
        goto END;
    }
    LOG_INFO("Sensor opened!");

    rgb_handle = senscord_ub_create_stream(
        WINDOW_NAME, INPUT_TENSOR_SIZE, INPUT_TENSOR_SIZE,
        INPUT_TENSOR_SIZE * 3, SENSCORD_PIXEL_FORMAT_RGB24);
    if (rgb_handle == 0) {
        LOG_ERR("CreateStream failed.");
        goto END;
    }

    /* sensor start working */
    ret = sensor_start(stream);
    if (ret != 0) {
        LOG_INFO("senscord_stream_start failed");
        goto END;
    }
    LOG_INFO("Sensor started!");

    for (;;) {
        for (int i = 0; i < 10; ++i) {
            result = EVP_processEvent(h, 1);
            if (result == EVP_SHOULDEXIT) {
                LOG_INFO("Exiting the main loop");
                goto END;
            }
        }

        frame_t frame;
        ret = sensor_get_frame(stream, &frame);
        if (ret != 0) {
            LOG_INFO("Skipping frame");
            continue;
        }

        LOG_INFO("image_property height = %d", frame.info[0].property.height);
        LOG_INFO("image_property width = %d", frame.info[0].property.width);
        LOG_INFO("image_property stride_bytes = %d",
                 frame.info[0].property.stride_bytes);
        LOG_INFO("image_property pixel_format = %s",
                 frame.info[0].property.pixel_format);

        do_resize = frame.info[0].property.height != INPUT_TENSOR_SIZE |
                    frame.info[0].property.width != INPUT_TENSOR_SIZE;
        is_yuv =
            strcmp(frame.info[0].property.pixel_format, "image_nv16") == 0;

        cam_height = frame.info[0].property.height;
        cam_width = frame.info[0].property.width;

        inference_data_t inference_data = {0};
        ret = inference(frame, &inference_data);
        if (ret != 0) {
            LOG_ERR("Inference error");
            goto END;
        };
        ret = telemetry(inference_data);
        if (ret != 0) {
            LOG_ERR("Telemetry error");
            goto END;
        };
        cleanup_inference_data(&inference_data);
        ret = sensor_release_frame(stream, frame);
        if (ret != 0) {
            LOG_ERR("Release frame error");
            goto END;
        };
    }
END:
    if (model_file != NULL)
        free(model_file);
    if (rgb_handle != 0)
        senscord_ub_destroy_stream(rgb_handle);
    if (stream != 0 && core != 0) {
        sensor_stop(stream);
        sensor_close(core, stream);
    }
    return 0;
}
