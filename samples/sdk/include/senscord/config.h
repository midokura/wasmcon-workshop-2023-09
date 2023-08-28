/*
 * Copyright 2021-2022 Sony Semiconductor Solutions Corporation.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Semiconductor
 * Solutions Corporation.
 * No part of this file may be copied, modified, sold, and distributed in any
 * form or by any means without prior explicit permission in writing from
 * Sony Semiconductor Solutions Corporation.
 *
 */
#ifndef SENSCORD_CONFIG_H_
#define SENSCORD_CONFIG_H_

/* Logger */
#define SENSCORD_LOG_ENABLED

/* Status */
#define SENSCORD_STATUS_MESSAGE_ENABLED
#define SENSCORD_STATUS_TRACE_ENABLED

/* Multi-process */
#define SENSCORD_SERVER
/* #undef SENSCORD_SERVER_ROS */
#define SENSCORD_SERVER_WEBSOCKET
#define SENSCORD_SERVER_SETTING
/* #undef SENSCORD_SERVER_SEARCH_SSDP */
/* #undef SENSCORD_SERVER_SEARCH_UCOM */

/* Allocator */
#define SENSCORD_ALLOCATOR_CONFIG
#define SENSCORD_ALLOCATOR_SHARED_MEMORY

/* Recorder */
#define SENSCORD_RECORDER
#define SENSCORD_RECORDER_RAW
/* #undef SENSCORD_RECORDER_SKV */
#define SENSCORD_RECORDER_LOADER

/* Player */
#define SENSCORD_PLAYER
#define SENSCORD_PLAYER_RAW
/* #undef SENSCORD_PLAYER_SKV */

/* API */
#define SENSCORD_API_C
/* #undef SENSCORD_API_PYTHON */
/* #undef SENSCORD_API_CSHARP */
/* #undef SENSCORD_API_JAVA */

/* Core functions */
#define SENSCORD_STREAM_VERSION
#define SENSCORD_STREAM_EVENT_ARGUMENT
#define SENSCORD_SERIALIZE

#endif  /* SENSCORD_CONFIG_H_ */
