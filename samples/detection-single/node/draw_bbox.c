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

#include "draw_bbox.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "logger.h"
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color;

static color bbox_color = {.r = 255, .g = 255, .b = 0};

void
change_color(uint8_t r, uint8_t g, uint8_t b)
{
    LOG_INFO("New bbox color = (%d, %d, %d)", r, g, b);
    bbox_color.r = r;
    bbox_color.g = g;
    bbox_color.b = b;
}

int32_t
frame_bbox(uint8_t *image, uint32_t size,
           struct senscord_rectangle_region_parameter_t *crop, float cls,
           float score)
{

    CvMat *mat1 =
        cvCreateMatHeader(INPUT_TENSOR_SIZE, INPUT_TENSOR_SIZE, CV_8UC3);
    cvSetData(mat1, image, CV_AUTOSTEP);
    LOG_DBG("left, top, right, bottom = %d %d %d %d", crop->left, crop->top,
            crop->right, crop->bottom);
    crop->left = MIN(MAX(crop->left, 0), INPUT_TENSOR_SIZE - 1);
    crop->right = MIN(MAX(crop->right, 0), INPUT_TENSOR_SIZE - 1);
    crop->top = MIN(MAX(crop->top, 0), INPUT_TENSOR_SIZE - 1);
    crop->bottom = MIN(MAX(crop->bottom, 0), INPUT_TENSOR_SIZE - 1);
    CvPoint p1 = {.x = crop->left, .y = crop->top};
    CvPoint p2 = {.x = crop->right, .y = crop->bottom};
    CvScalar color;
    color.val[0] = bbox_color.r;
    color.val[1] = bbox_color.g;
    color.val[2] = bbox_color.b;
    color.val[3] = 8;
    cvRectangle(mat1, &p1, &p2, &color, 1, 8, 0);

    cvGetData(mat1, image, INPUT_TENSOR_SIZE * INPUT_TENSOR_SIZE * 3);
    cvReleaseMat(&mat1);
    return 0;
}
