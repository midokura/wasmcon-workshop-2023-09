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
#ifndef SENSCORD_ERROR_TYPES_H_
#define SENSCORD_ERROR_TYPES_H_

#include "senscord/config.h"

/**
 * @brief Level of error.
 */
enum senscord_error_level_t {
  SENSCORD_LEVEL_UNDEFINED = 0,
  SENSCORD_LEVEL_FAIL,
  SENSCORD_LEVEL_FATAL,
};

/**
 * @brief Cause of error.
 */
enum senscord_error_cause_t {
  SENSCORD_ERROR_NONE = 0,
  SENSCORD_ERROR_NOT_FOUND,
  SENSCORD_ERROR_INVALID_ARGUMENT,
  SENSCORD_ERROR_RESOURCE_EXHAUSTED,
  SENSCORD_ERROR_PERMISSION_DENIED,
  SENSCORD_ERROR_BUSY,
  SENSCORD_ERROR_TIMEOUT,
  SENSCORD_ERROR_CANCELLED,
  SENSCORD_ERROR_ABORTED,
  SENSCORD_ERROR_ALREADY_EXISTS,
  SENSCORD_ERROR_INVALID_OPERATION,
  SENSCORD_ERROR_OUT_OF_RANGE,
  SENSCORD_ERROR_DATA_LOSS,
  SENSCORD_ERROR_HARDWARE_ERROR,
  SENSCORD_ERROR_NOT_SUPPORTED,
  SENSCORD_ERROR_UNKNOWN,
};

#endif  /* SENSCORD_ERROR_TYPES_H_ */
