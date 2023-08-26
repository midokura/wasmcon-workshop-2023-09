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

#ifndef DRAW_BBOX_H
#define DRAW_BBOX_H

#include "senscord/c_api/senscord_c_api.h"

int32_t frame_bbox(uint8_t *image, uint32_t size,
                   struct senscord_rectangle_region_parameter_t *crop,
                   float cls, float score);

void change_color(uint8_t r, uint8_t g, uint8_t b);

#endif
